#include "Schrodinger.h"
#include <cmath>
#include <numeric>
#include "Utils.h"
#include <iostream>
using namespace std;

Schrodinger::Schrodinger(int Nx, int Ny, double dh, double dt, double sigma)
    : dh(dh), dt(dt), sigma(sigma), k1(Nx*Ny), k2(Nx*Ny), k3(Nx*Ny), k4(Nx*Ny), temp_state(Nx*Ny)  {}

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

void Schrodinger::laplacian_inplace(
    const vector<complex<double>>& psi,
    const vector<int>& boundary,
    vector<complex<double>>& result,
    int Nx, int Ny) const {
    double dh_sq = dh * dh;

    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int id = idx(i, j, Ny);

            if (boundary[id] == 1) {
                result[id] = {0.0, 0.0};
                continue;
            }

            // Direct boundary checks instead of getPsiSafe
            complex<double> left, right, up, down;

            // Left neighbor
            if (i > 0 && boundary[idx(i-1, j, Ny)] == 0) {
                left = psi[idx(i-1, j, Ny)];
            } else {
                left = {0.0, 0.0};
            }

            // Right neighbor
            if (i < Nx-1 && boundary[idx(i+1, j, Ny)] == 0) {
                right = psi[idx(i+1, j, Ny)];
            } else {
                right = {0.0, 0.0};
            }

            // Down neighbor
            if (j > 0 && boundary[idx(i, j-1, Ny)] == 0) {
                down = psi[idx(i, j-1, Ny)];
            } else {
                down = {0.0, 0.0};
            }

            // Up neighbor
            if (j < Ny-1 && boundary[idx(i, j+1, Ny)] == 0) {
                up = psi[idx(i, j+1, Ny)];
            } else {
                up = {0.0, 0.0};
            }

            result[id] = (left + right + up + down - 4.0 * psi[id]) / dh_sq;
        }
    }
}

vector<complex<double>> Schrodinger::RK4_Schrodinger(
    const vector<complex<double>>& psi,
    const vector<int>& boundary, int Nx, int Ny) const {

    int size = Nx * Ny;

    // Helper function that computes the derivative in-place
    auto compute_derivative = [&](const vector<complex<double>>& state, vector<complex<double>>& result) {
        laplacian_inplace(state, boundary, result, Nx, Ny);  // Use in-place version
        for (int id = 0; id < size; id++) {
            result[id] *= -im * 0.5;  // Apply the physics factor
        }
    };

    // Compute k1 = f(psi)
    compute_derivative(psi, k1);

    // Compute k2 = f(psi + 0.5*dt*k1)
    add_scaled_inplace(temp_state, psi, k1, 0.5 * dt, size);
    compute_derivative(temp_state, k2);

    // Compute k3 = f(psi + 0.5*dt*k2)
    add_scaled_inplace(temp_state, psi, k2, 0.5 * dt, size);
    compute_derivative(temp_state, k3);

    // Compute k4 = f(psi + dt*k3)
    add_scaled_inplace(temp_state, psi, k3, dt, size);
    compute_derivative(temp_state, k4);

    // Final result: psi + (dt/6)*(k1 + 2*k2 + 2*k3 + k4)
    vector<complex<double>> result(size);
    for (int id = 0; id < size; id++) {
        result[id] = psi[id] + (dt/6.0) * (k1[id] + 2.0*k2[id] + 2.0*k3[id] + k4[id]);
    }

    return result;
}

void Schrodinger::add_scaled_inplace(vector<complex<double>>& result,
                                     const vector<complex<double>>& A,
                                     const vector<complex<double>>& B,
                                     double scale, int size) const {
    for (int i = 0; i < size; ++i) {
        result[i] = A[i] + scale * B[i];
    }
}
vector<complex<double>> Schrodinger::gaussian_packet(
    int nx, int ny, double x0, double y0, double k, double theta
    ) const {
        double kx = k * cos(theta);
        double ky = k * sin(theta);

        vector<double> x(nx), y(ny);
        for (int i = 0; i < nx; i++) x[i] = (i - nx / 2) * dh;
        for (int j = 0; j < ny; j++) y[j] = (j - ny / 2) * dh;

        vector<complex<double>> psi(nx * ny, {0.0, 0.0});

        double norm = 0.0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                double X = x[i], Y = y[j];
                double env = exp(-((X - x0)*(X - x0) + (Y - y0)*(Y - y0)) / (2.0 * sigma * sigma));
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
