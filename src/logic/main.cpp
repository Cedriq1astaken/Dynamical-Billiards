#include <iostream>
#include <cmath>
#include "Billiard.h"
#include "Vec2.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "SinaiBilliard.h"
#include "Schrodinger.h"
#include "Utils.h"
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

using namespace std;

const double epsilon = 1e-8;
const int MAX_POINTS = 100;

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

SinaiBilliard write(double a, double b, double l, double h, Vec2 p0, double angle, int count, vector<Circle> scatterer){
    ofstream log_file("./data/classical_data.csv");

    SinaiBilliard billiard(a, b, l, h);
    for (auto & i : scatterer) {
        billiard.addScatterer(i.center, i.radius);
    }
    vector<Vec2> ds;
    vector<Vec2> ps;
    for (int i = 0; i < count; i++) {
        ps.emplace_back(p0);
        ds.emplace_back(cos(angle + (M_PI * i)/720), sin(angle + (M_PI * i)/720));
    }
    for (int i = 0; i < count; i++) {
        log_file << "p" << i << ((i == ps.size() - 1) ? "\n" : ",");
    }
    for (int i = 0; i < count; i++) {
        log_file << ps[i] << ((i == ps.size() - 1) ? "\n" : ",");
    }
    for (int i = 0; i < MAX_POINTS; i++) {
        for (int j = 0; j < count; j++) {
            Vec2 p = ps[j];
            Vec2 d = ds[j];
            p = billiard.getIntersectionPoint(p, d);
            d = next_reflection(billiard, d, p);
            ds[j] = d;
            ps[j] = p;
            log_file << p << ((j == ps.size() - 1) ? "\n" : ",");
        }
    }
    return billiard;
}

void write_quantum(double dh, double dt, double sigma, double x0, double y0, double k, double theta, double width, double height, const SinaiBilliard& billiard) {
    ofstream log_file("./data/quantum_data.csv");

    Schrodinger schrodinger(dh, dt, sigma);
    int nx = static_cast<int>(width / dh);
    int ny = static_cast<int>(height / dh);

    vector<int> boundary = billiard.getBoundary(width, height);
    vector<complex<double>> psi = schrodinger.gaussian_packet(nx, ny, x0, y0, k, theta);

    // (3) Use resize instead of assign for clarity
    vector<float> prob_density(psi.size(), 0.0f);
    for (size_t i = 0; i < psi.size(); i++) {
        prob_density[i] = pow(abs(psi[i]), 2);
    }

    float max_p = maximum(prob_density);
    for (float& p : prob_density) {
        p /= max_p;
    }

    // header row: indices
    for (int i = 0; i < nx * ny; i++) {
        log_file << i << ",";
    }
    log_file << '\n';

    // first timestep
    for (auto p : prob_density) {
        log_file << p << ",";
    }
    log_file << '\n';

    // subsequent timesteps
    for (int i = 0; i < MAX_POINTS; i++) {
        switch (static_cast<int>(i * 100.0 / MAX_POINTS)) {
            case 0:
                cout << "0%" << "\n";
                break;
            case 25:
                cout << "25%" << "\n";
                break;
            case 50:
                cout << "50%" << "\n";
                break;
            case 75:
                cout << "75%" << "\n";
                break;
            default:
                break;
        }
        for (int j = 0; j < 25; j++) {
            psi = schrodinger.RK4_Schrodinger(psi, boundary, nx, ny);
        }

        prob_density.clear();
        prob_density.reserve(psi.size());
        for (auto com : psi) {
            prob_density.emplace_back(pow(abs(com), 2));
        }

        max_p = maximum(prob_density);
        for (float& p : prob_density) {
            p /= max_p;
        }

        for (auto p : prob_density) {
            log_file << p << ",";
        }
        log_file << '\n';
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
