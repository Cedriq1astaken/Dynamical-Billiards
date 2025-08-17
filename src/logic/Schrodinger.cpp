#include "Schrodinger.h"
#include "complex.h"
#include <cmath>
#include <complex>

using namespace std;
using Complex = complex<double>;

Complex Schrodinger::getPsiSafe(
    const vector<vector<Complex>>& psi,
    const vector<vector<int>>& boundary,
    int i, int j
) {
  int Nx = psi.size();
  int Ny = psi[0].size();

  if (i < 0 || i >= Nx || j < 0 || j >= Ny) return Complex(0.0, 0.0);
  if (boundary[i][j] == 1) return Complex(0.0, 0.0);
  return psi[i][j];
}

Complex Schrodinger::laplacian(
    const vector<vector<Complex>>& psi, vector<vector<int>>& boundary,
    int i, int j
    ) {
      Complex center = psi[i][j];

      Complex d2x = (getPsiSafe(psi, boundary, i+1, j)
                    - 2.0*center
                    + getPsiSafe(psi, boundary, i-1, j)) / (dh*dh);

      Complex d2y = (getPsiSafe(psi, boundary, i, j+1)
                    - 2.0*center
                    + getPsiSafe(psi, boundary, i, j-1)) / (dh*dh);

      return d2x + d2y;
}

vector<vector<Complex>>  Schrodinger::RK4_Schrodinger(vector<vector<Complex>> map, vector<vector<int>> boundary){
  int Nx = map.size();
  int Ny = map[0].size();

  vector<vector<Complex>> k1(Nx, vector<Complex>(Ny));
  vector<vector<Complex>> k2(Nx, vector<Complex>(Ny));
  vector<vector<Complex>> k3(Nx, vector<Complex>(Ny));
  vector<vector<Complex>> k4(Nx, vector<Complex>(Ny));
  vector<vector<Complex>> map_temp = map;

  Complex im(0,1);

  // 1. Compute k1
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      k1[i][j] = im * 0.5 * laplacian(map, boundary, i, j);
    }
  }

  // 2. Compute k2
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      map_temp[i][j] = map[i][j] + dt/2.0 * k1[i][j];
    }
  }
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      k2[i][j] = im * 0.5 * laplacian(map_temp, boundary, i, j);
    }
  }

  // 3. Compute k3
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      map_temp[i][j] = map[i][j] + dt/2.0 * k2[i][j];
    }
  }
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      k3[i][j] = im * 0.5 * laplacian(map_temp, boundary, i, j);
    }
  }

  // 4. Compute k4
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      map_temp[i][j] = map[i][j] + dt * k3[i][j];
    }
  }
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      k4[i][j] = im * 0.5 * laplacian(map_temp, boundary, i, j);
    }
  }

  // 5. Combine
  vector<vector<Complex>> new_map(Nx, vector<Complex>(Ny));
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      new_map[i][j] = map[i][j] + dt/6.0 * (k1[i][j] + 2.0*k2[i][j] + 2.0*k3[i][j] + k4[i][j]);
    }
  }
  return new_map;
}