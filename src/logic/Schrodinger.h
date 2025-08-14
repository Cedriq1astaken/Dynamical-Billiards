#ifndef SCHODINGER_H
#define SCHODINGER_H
#include <vector>
#include <complex>

using namespace std;
using Complex = complex<double>;


class Schrodinger {
  private:
    double mass;
    double planck;
    double dh = 0.01;
    double dt = 5e-5;
  public:
    vector<vector<Complex>>  RK4_Schrodinger(vector<vector<Complex>> map, vector<vector<double>> boundary);
  Complex laplacian(
        const vector<vector<Complex>>& psi,
        int i, int j);
};



#endif //SCHODINGER_H
