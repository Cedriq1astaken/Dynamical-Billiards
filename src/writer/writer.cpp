#include <iostream>
#include <cmath>
#include "Vec2.h"
#include <fstream>
#include "SinaiBilliard.h"
#include "Schrodinger.h"
#include "Utils.h"
#include <algorithm>
#include "raylib.h"
#include "writer.h"

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

Vec2 move(int i, int t, vector<Vec2> points, int total_frames) {
    Vec2 p1 = points[i];
    Vec2 p0 = points[i - 1];

    double x = p0.x + (p1.x - p0.x) * t / total_frames;
    double y = p0.y + (p1.y - p0.y) * t / total_frames;

    return {x, y};
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

Color probability_to_rgb(float p) {
    Color color;

    if (p < 0.5){
        double t = p / 0.5;
        color.r = 0;
        color.g = 0;
        color.b = static_cast<unsigned char>(128 * t);
    }
    else {
        double t = (p - 0.5) / 0.5;
        color.r = static_cast<unsigned char>(173 * t);
        color.g = static_cast<unsigned char>(216 * t);
        color.b = static_cast<unsigned char>(128 + (127 * t));
    }

    color.a = 255;
    return color;
}

vector<vector<Vec2>> write_classical(SinaiBilliard billiard, Vec2 p0, double angle, int count) {
    ofstream bin_file("../../data/classical_data.bin", ios::binary);

    vector<Vec2> ds;
    vector<Vec2> ps;
    vector<vector<Vec2>> trajectories(count, vector<Vec2>(MAX_POINTS));

    for (int i = 0; i < count; i++) {
        ps.emplace_back(p0);
        ds.emplace_back(cos(angle + (M_PI * i) / 720), sin(angle + (M_PI * i) / 720));
        trajectories[i][0] = ps[i];
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
            trajectories[j][t] = p;
            bin_file.write(reinterpret_cast<const char*>(coords), sizeof(coords));
        }
    }
    return trajectories;
}

vector<vector<float>> write_quantum(double dh, double dt, double sigma, double x0, double y0, double k, double theta,
                   const SinaiBilliard& billiard) {
    ofstream bin_file("./data/quantum_data.bin", ios::binary);

    int nx = static_cast<int>(WIDTH / dh);
    int ny = static_cast<int>(HEIGHT / dh);
    Schrodinger schrodinger(nx, ny, dh, dt, sigma);

    vector<int> boundary = billiard.getBoundary(WIDTH, HEIGHT, dh);
    vector<complex<double>> psi = schrodinger.gaussian_packet(nx, ny, x0, y0, k, theta);
    vector<vector<float>> densities;

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
    densities.emplace_back(prob_density);

    // Subsequent timesteps
    for (int t = 0; t < MAX_POINTS; t++) {
        for (int j = 0; j < 10; j++) {
            psi = schrodinger.RK4_Schrodinger(psi, boundary, nx, ny);
        }

        for (size_t i = 0; i < psi.size(); i++) {
            prob_density[i] = pow(abs(psi[i]), 2);
        }
        normalize(prob_density);
        densities.emplace_back(prob_density);
        bin_file.write(reinterpret_cast<const char*>(prob_density.data()), prob_density.size() * sizeof(float));
    }
    return densities;
}