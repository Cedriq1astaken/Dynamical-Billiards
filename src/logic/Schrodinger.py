import numpy as np
from math import cos, sin, pi

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

    def laplacian(self, psi, boundary, dh):
        lap = (
                np.roll(psi, 1, axis=0) + np.roll(psi, -1, axis=0) +
                np.roll(psi, 1, axis=1) + np.roll(psi, -1, axis=1) -
                4 * psi
        ) / (dh ** 2)
        lap[boundary == 1] = 0
        return lap

    def RK4_Schrodinger(self, psi, boundary):
        def f(psi):
            return self.im * self.laplacian(psi, boundary, self.dh)

        k1 = f(psi)
        k2 = f(psi + 0.5 * self.dt * k1)
        k3 = f(psi + 0.5 * self.dt * k2)
        k4 = f(psi + self.dt * k3)

        return  psi + (self.dt/6) * (k1 + 2*k2 + 2*k3 + k4)

    def gaussian_packet(self, nx, ny, x0, y0, k, theta):
        kx = -100 * k * cos(theta)
        ky = -100 * k * sin(theta)
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

