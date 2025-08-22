#include <iostream>
#include <cmath>
#include "Vec2.h"
#include <filesystem>
#include <fstream>
#include "SinaiBilliard.h"
#include "Schrodinger.h"
#include "Utils.h"
#include <algorithm>

namespace fs = std::filesystem;

using namespace std;

const double epsilon = 1e-8;
const int MAX_POINTS = 500;

ostream& operator<<(ostream& os, const Vec2& v) {
    os << v.x << "|" << v.y;
    return os;
};

float maximum(vector<float> v) {
    float ans = 0.0;
    for (auto i: v)
        ans = max(i, ans);

    return  ans;
}

vector<Circle> parseCircles(const string& s) {
    vector<Circle> circles;
    string trimmed = s;

    if (!trimmed.empty() && trimmed.front() == '[') trimmed.erase(0, 1);
    if (!trimmed.empty() && trimmed.back() == ']') trimmed.pop_back();

    size_t pos = 0;
    while ((pos = trimmed.find("),(")) != string::npos) {
        trimmed.replace(pos, 3, ")|(");
    }

    stringstream ss(trimmed);
    string token;
    while (getline(ss, token, '|')) {

        if (!token.empty() && token.front() == '(') token.erase(0,1);
        if (!token.empty() && token.back() == ')') token.pop_back();

        stringstream token_ss(token);
        string num;
        vector<double> nums;
        while (getline(token_ss, num, ',')) {
            nums.push_back(stod(num));
        }

        if (nums.size() == 3) {
            circles.push_back({{nums[0], nums[1]}, nums[2]});
        }
    }

    return circles;
}


Vec2 next_reflection(SinaiBilliard b,Vec2 d, Vec2 p_i) { //p_i == point of intersection
    Vec2 n = b.getNormal(p_i);
    Vec2 n_normalized = n.normalize();

    double dot = d.dot(n_normalized);
    return d - n_normalized * (2 * dot);
}

SinaiBilliard write(double a, double b, double l, double h, Vec2 p0, double angle, int count, vector<Circle> scatterer) {
    ofstream bin_file("./data/classical_data.bin", ios::binary);

    SinaiBilliard billiard(a, b, l, h);
    for (auto &i : scatterer) {
        billiard.addScatterer(i.center, i.radius);
    }

    vector<Vec2> ds;
    vector<Vec2> ps;
    for (int i = 0; i < count; i++) {
        ps.emplace_back(p0);
        ds.emplace_back(cos(angle + (M_PI * i) / 720), sin(angle + (M_PI * i) / 720));
    }

    // Write metadata: count and max points
    int max_points = MAX_POINTS;
    bin_file.write(reinterpret_cast<const char*>(&count), sizeof(int));
    bin_file.write(reinterpret_cast<const char*>(&max_points), sizeof(int));

    for (int j = 0; j < count; j++) {
        bin_file.write(reinterpret_cast<const char*>(&ps[j].x), sizeof(double));
        bin_file.write(reinterpret_cast<const char*>(&ps[j].y), sizeof(double));
    }

    // Simulate and write positions
    for (int t = 0; t < max_points; t++) {
        for (int j = 0; j < count; j++) {
            Vec2 p = ps[j];
            Vec2 d = ds[j];
            p = billiard.getIntersectionPoint(p, d);
            d = next_reflection(billiard, d, p);
            ds[j] = d;
            ps[j] = p;

            double coords[2] = { p.x, p.y };
            bin_file.write(reinterpret_cast<const char*>(coords), sizeof(coords));
        }
    }

    return billiard;
}

void write_quantum(double dh, double dt, double sigma, double x0, double y0, double k, double theta,
                   double width, double height, const SinaiBilliard& billiard) {
    ofstream bin_file("./data/quantum_data.bin", ios::binary);
    ofstream test("./data/test.txt", ios::binary);

    Schrodinger schrodinger(dh, dt, sigma);
    int nx = static_cast<int>(width / dh);
    int ny = static_cast<int>(height / dh);

    vector<int> boundary = billiard.getBoundary(width, height, dh);
    vector<complex<double>> psi = schrodinger.gaussian_packet(nx, ny, x0, y0, k, theta);

    // Metadata: nx, ny, MAX_POINTS
    int max_points = MAX_POINTS;
    bin_file.write(reinterpret_cast<const char*>(&nx), sizeof(int));
    bin_file.write(reinterpret_cast<const char*>(&ny), sizeof(int));
    bin_file.write(reinterpret_cast<const char*>(&max_points), sizeof(int));

    // Probability density buffer
    vector<float> prob_density(psi.size());

    auto normalize = [](vector<float>& v) {
        float max_v = *max_element(v.begin(), v.end());
        for (auto& p : v) p /= max_v;
    };

    // First timestep
    for (size_t i = 0; i < psi.size(); i++) {
        prob_density[i] = pow(abs(psi[i]), 2);
    }
    normalize(prob_density);
    bin_file.write(reinterpret_cast<const char*>(prob_density.data()), prob_density.size() * sizeof(float));

    // Subsequent timesteps
    for (int t = 0; t < MAX_POINTS; t++) {
        for (int j = 0; j < 25; j++) {
            psi = schrodinger.RK4_Schrodinger(psi, boundary, nx, ny);
        }

        for (size_t i = 0; i < psi.size(); i++) {
            prob_density[i] = pow(abs(psi[i]), 2);
        }
        normalize(prob_density);

        bin_file.write(reinterpret_cast<const char*>(prob_density.data()), prob_density.size() * sizeof(float));
    }
}

int main(int argc, char* argv[]) {
    if (argc < 16) {
        cout << "Please provide 15 arguments.\n";
        return 1;
    }
    if (argc > 16) {
        cout << "Too many arguments.\n";
        return 1;
    }
    cout << "Starting..\n";

    try {
        cout << "Classical running" << "\n";
        double a     = stod(argv[1]);
        double b     = stod(argv[2]);
        double l     = stod(argv[3]);
        double h     = stod(argv[4]);
        double x0    = stod(argv[5]);
        double y0    = stod(argv[6]);
        double angle = stod(argv[7]) * M_PI / 180.0;
        int count    = stoi(argv[8]);

        vector<Circle> circles = parseCircles(argv[9]);
        SinaiBilliard billiard = write(a, b, l, h, {x0, y0}, angle, count, circles);
        cout << "Classical done" << "\n";

        cout << "Quantum running" << "\n";


        double WIDTH  = stod(argv[10]);
        double HEIGHT = stod(argv[11]);
        double dh     = stod(argv[12]);
        double dt     = stod(argv[13]);
        double sigma  = stod(argv[14]);
        double k      = stod(argv[15]);

        write_quantum(dh, dt, sigma, x0, y0, k, angle, WIDTH, HEIGHT, billiard);
        cout << "Quantum done" << "\n";

    }

    catch (const std::exception& e) {
        std::cerr << "Error parsing numeric arguments: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
