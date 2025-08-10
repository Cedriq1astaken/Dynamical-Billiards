#include <iostream>
#include <cmath>
#include "Billiard.h"
#include "Vec2.h"
#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std;

const double epsilon = 1e-8;
const int MAX_POINTS = 10000;

ostream& operator<<(ostream& os, const Vec2& v) {
    os << v.x << "|" << v.y;
    return os;
};

Vec2 next_reflection(Billiard b,Vec2 d, Vec2 p_i) { //p_i == point of intersection
    Vec2 n = b.getNormal(p_i);
    Vec2 n_normalized = n.normalize();

    double dot = d.dot(n_normalized);
    return d - n_normalized * (2 * dot);
}

void write(double a, double b, double l, double h, Vec2 p0, double angle, int count){
    std::cout << "Writing file in: "
              << filesystem::current_path() << "\n";
    fstream log_file( "./data/data.csv");

    Billiard billiard(a, b, l, h);
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
            p = billiard.getIntersectionPointHelper(p, d);
            d = next_reflection(billiard, d, p);

            ds[j] = d;
            ps[j] = p;
            log_file << p << ((j == ps.size() - 1) ? "\n" : ",");
        }
    }

}

int main(int argc, char* argv[]) {
    if (argc < 9) {
        std::cerr << "Please provide 8 arguments.\n";
        return 1;
    }
    if (argc > 9) {
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

        write(a, b, l, h, Vec2(x0, y0), angle, count);
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing numeric arguments: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
