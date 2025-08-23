#ifndef SCHRODINGER_H
#define SCHRODINGER_H

#include <vector>
#include <complex>

using std::complex;
using namespace std;
class Schrodinger {
private:
    mutable vector<complex<double>> k1, k2, k3, k4, temp_state;
public:
    Schrodinger(int Nx, int Ny, double dh, double dt, double sigma);
    complex<double> getPsiSafe(
        const vector<complex<double>>& psi,
        const vector<int>& boundary,
        int i, int j, int Nx, int Ny
    ) const;

    void laplacian_inplace(const vector<complex<double>>& psi,
                          const vector<int>& boundary,
                          vector<complex<double>>& result,
                          int Nx, int Ny) const;

    void add_scaled_inplace(vector<complex<double>> &result, const vector<complex<double>> &A,
                            const vector<complex<double>> &B, double scale, int size) const;

    vector<complex<double>> RK4_Schrodinger(
        const vector<complex<double>>& psi,
        const vector<int>& boundary, int Nx, int Ny
    ) const;

    vector<complex<double>> gaussian_packet(
        int nx, int ny, double x0, double y0, double k, double theta
    ) const;

private:
    double dh;
    double dt;
    double sigma;
    const complex<double> im = complex<double>(0.0, 1.0);
};

#endif // SCHRODINGER_H
