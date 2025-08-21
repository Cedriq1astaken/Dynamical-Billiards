#include "Schrodinger.h"
#include <cmath>
#include <numeric>
#include "Utils.h"

using namespace std;

Schrodinger::Schrodinger(double dh, double dt, double sigma)
    : dh(dh), dt(dt), sigma(sigma) {}

complex<double> Schrodinger::getPsiSafe(
    const vector<complex<double>> & psi,
    const vector<int>& boundary,
    int i, int j, int Nx, int Ny) const {
        i = (i % Nx + Nx) % Nx;
        j = (j % Ny + Ny) % Ny;

        if (boundary[idx(i, j, Ny)] == 1) {
            return complex<double>(0.0, 0.0);
        }
        return psi[idx(i, j, Ny)];
}

vector<complex<double>> Schrodinger::laplacian(
    const vector<complex<double>>& psi,
    const vector<int>& boundary,
    int Nx, int Ny) const {
    vector<complex<double>> lap(Nx * Ny, {0.0, 0.0});

    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int id = idx(i, j, Ny);
            if (boundary[id] == 1) {
                lap[id] = {0.0, 0.0};
                continue;
            }
            lap[id] = (
                getPsiSafe(psi, boundary, i+1, j, Nx, Ny) +
                getPsiSafe(psi, boundary, i-1, j, Nx, Ny) +
                getPsiSafe(psi, boundary, i, j+1, Nx, Ny) +
                getPsiSafe(psi, boundary, i, j-1, Nx, Ny) -
                complex<double>(4.0, 0.0) * psi[id]
            ) / (dh * dh);
        }
    }
    return lap;
}

vector<complex<double>> Schrodinger::RK4_Schrodinger(
    const vector<complex<double>>& psi,
    const vector<int>& boundary, int Nx, int Ny) const {
    auto f = [&](const vector<complex<double>>& state) {
        auto lap = laplacian(state, boundary, Nx, Ny);

        vector<complex<double>> res(Nx * Ny);
        for (int id = 0; id < Nx * Ny; id++) {
            res[id] = im * lap[id];
        }
        return res;
    };

    auto add_scaled = [&](const vector<complex<double>>& A,
                          const vector<complex<double>>& B,
                          double scale, int Nx, int Ny) {
        vector<complex<double>> res(Nx * Ny, {0.0, 0.0});
            for (int i = 0; i < Nx; i++) {
                for (int j = 0; j < Ny; j++) {
                    int id = idx(i, j, Ny);
                    res[id] = A[id] + scale * B[id];
                }
            }
            return res;
    };

    auto k1 = f(psi);
    auto k2 = f(add_scaled(psi, k1, 0.5 * dt, Nx, Ny));
    auto k3 = f(add_scaled(psi, k2, 0.5 * dt, Nx, Ny));
    auto k4 = f(add_scaled(psi, k3, dt, Nx, Ny));


    vector<complex<double>> res(Nx *Ny);
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int id = idx(i, j, Ny);
            res[id] = psi[id] + (dt/6.0) * (k1[id] + 2.0*k2[id] + 2.0*k3[id] + k4[id]);
        }
    }


    return res;
}

vector<complex<double>> Schrodinger::gaussian_packet(
    int nx, int ny, double x0, double y0, double k, double theta
    ) const {
        double kx = -100 * k * cos(theta);
        double ky = -100 * k * sin(theta);

        vector<double> x(nx), y(ny);
        for (int i = 0; i < nx; i++) x[i] = (i - nx / 2) * dh;
        for (int j = 0; j < ny; j++) y[j] = (j - ny / 2) * dh;

        vector<complex<double>> psi(nx * ny, {0.0, 0.0});

        double norm = 0.0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                double X = x[i], Y = y[j];
                double env = exp(-((X - x0)*(X - x0) + (Y - y0)*(Y - y0)) / (4.0 * sigma * sigma));
                complex<double> plane_wave = exp(im * (kx*(X - x0) + ky*(Y - y0)));
                psi[idx(i, j, ny)] = env * plane_wave;
                norm += std::norm(psi[idx(i, j, ny)]);
            }
        }

        norm *= dh * dh;
        double scale = 1.0 / sqrt(norm);

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                psi[idx(i, j, ny)] *= scale;
            }
        }

        return psi;
}
