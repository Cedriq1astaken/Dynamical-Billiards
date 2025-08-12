#include <iostream>
#include <cmath>
#include "Billiard.h"
#include "Vec2.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "SinaiBilliard.h"
using namespace std;

const double epsilon = 1e-8;
const int MAX_POINTS = 1000;

ostream& operator<<(ostream& os, const Vec2& v) {
    os << v.x << "|" << v.y;
    return os;
};

vector<Circle> parseCircles(const string& s) {
    vector<Circle> circles;
    string trimmed = s;

    // Remove brackets
    if (!trimmed.empty() && trimmed.front() == '[') trimmed.erase(0, 1);
    if (!trimmed.empty() && trimmed.back() == ']') trimmed.pop_back();

    // Replace "),(" with ")|(" to split easily
    size_t pos = 0;
    while ((pos = trimmed.find("),(")) != string::npos) {
        trimmed.replace(pos, 3, ")|(");
    }

    stringstream ss(trimmed);
    string token;
    while (getline(ss, token, '|')) {
        // token looks like "(1.0,2.0,0.5)" or " (3.0,4.0,1.2)"
        // Remove parentheses and spaces
        if (!token.empty() && token.front() == '(') token.erase(0,1);
        if (!token.empty() && token.back() == ')') token.pop_back();

        stringstream token_ss(token);
        string num;
        vector<double> nums;
        while (getline(token_ss, num, ',')) {
            nums.push_back(std::stod(num));
        }

        if (nums.size() == 3) {
            circles.push_back({{nums[0], nums[1]}, nums[2]});
        }
    }
    for (int i = 0; i < circles.size(); i++) {
        cout << circles[i].center << " " << circles[i].radius << endl;
    }
    return circles;
}


Vec2 next_reflection(SinaiBilliard b,Vec2 d, Vec2 p_i) { //p_i == point of intersection
    Vec2 n = b.getNormal(p_i);
    Vec2 n_normalized = n.normalize();

    double dot = d.dot(n_normalized);
    return d - n_normalized * (2 * dot);
}

void write(double a, double b, double l, double h, Vec2 p0, double angle, int count, vector<Circle> scatterer){
    ofstream log_file("./data/data.csv");
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
}
// int main() {
//
//     SinaiBilliard b(0, 0, 200, 200);
//     b.addScatterer({0,0}, 50);
//
//     Vec2 p = b.getIntersectionPoint({137.609, 200},{0.761823, -0.647785});
//     Vec2 d = next_reflection(b, {0.761823, -0.647785}, p);
//
//     cout << p << "  " << d << endl;
//     return 0;
// }

int main(int argc, char* argv[]) {
    if (argc < 10) {
        std::cerr << "Please provide 9 arguments.\n";
        return 1;
    }
    if (argc > 10) {
        std::cerr << "Too many arguments.\n";
        return 1;
    }

    try {
        double a     = stod(argv[1]);
        double b     = stod(argv[2]);
        double l     = stod(argv[3]);
        double h     = stod(argv[4]);
        double x0    = stod(argv[5]);
        double y0    = stod(argv[6]);
        double angle = stod(argv[7]) * M_PI / 180.0;
        int count    = stoi(argv[8]);
        cout << "Raw circles arg: '" << argv[9] << "'" << std::endl;
        vector<Circle> circles = parseCircles(argv[9]);
        write(a, b, l, h, {x0, y0}, angle, count, circles);
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing numeric arguments: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
