import numpy as np
from math import cos, sin

class Schrodinger:
    def __init__(self, dh, dt, sigma):
        self.dh = dh
        self.dt = dt
        self.im = 1j
        self.sigma = sigma


    def getPsiSafe(self, psi, boundary, i, j):
        Nx, Ny = psi.shape
        i = i % Nx  # wrap around
        j = j % Ny
        if boundary[i, j] == 1:
            return 0.0 + 0.0j
        return psi[i, j]

    def laplacian(self, psi, boundary, i, j):
        center = psi[i, j]

        d2x = (self.getPsiSafe(psi, boundary, i+1, j)
               - 2.0*center
               + self.getPsiSafe(psi, boundary, i-1, j)) / (self.dh**2)

        d2y = (self.getPsiSafe(psi, boundary, i, j+1)
               - 2.0*center
               + self.getPsiSafe(psi, boundary, i, j-1)) / (self.dh**2)

        return d2x + d2y

    def RK4_Schrodinger(self, psi, boundary):
        Nx, Ny = psi.shape

        k1 = np.zeros_like(psi, dtype=np.complex128)
        k2 = np.zeros_like(psi, dtype=np.complex128)
        k3 = np.zeros_like(psi, dtype=np.complex128)
        k4 = np.zeros_like(psi, dtype=np.complex128)

        # 1. k1
        for i in range(Nx):
            for j in range(Ny):
                k1[i, j] = self.im * 0.5 * self.laplacian(psi, boundary, i, j)

        # 2. k2
        map_temp = psi + (self.dt/2.0) * k1
        for i in range(Nx):
            for j in range(Ny):
                k2[i, j] = self.im * 0.5 * self.laplacian(map_temp, boundary, i, j)

        # 3. k3
        map_temp = psi + (self.dt/2.0) * k2
        for i in range(Nx):
            for j in range(Ny):
                k3[i, j] = self.im * 0.5 * self.laplacian(map_temp, boundary, i, j)

        # 4. k4
        map_temp = psi + self.dt * k3
        for i in range(Nx):
            for j in range(Ny):
                k4[i, j] = self.im * 0.5 * self.laplacian(map_temp, boundary, i, j)

        # 5. Combine
        new_map = psi + (self.dt/6.0) * (k1 + 2*k2 + 2*k3 + k4)

        return new_map

    def gaussian_packet(self, nx, ny, x0, y0, k, theta):
        kx = k * cos(theta)
        ky = k * sin(theta)

        x = (np.arange(nx) - nx // 2) * self.dh
        y = (np.arange(ny) - ny // 2) * self.dh

        X, Y = np.meshgrid(x, y, indexing="xy")
        env = np.exp(-((X - x0) ** 2 + (Y - y0) ** 2) / (4 * self.sigma ** 2))
        plane_wave = np.exp(1j*(kx*(X-x0) + ky*(Y-y0)))
        psi = env * plane_wave

        # --- normalize ---
        prob_density = np.abs(psi) ** 2
        norm = np.sum(prob_density) * (self.dh ** 2)
        psi /= np.sqrt(norm)

        return psi

