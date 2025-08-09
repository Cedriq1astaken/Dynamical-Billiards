#include <iostream>
#include <cmath>
#include "Billiard.h"
#include "Vec2.h"
#include <iostream>
#include <fstream>

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

int main() {
    fstream log_file("../../data/data.csv");

    Billiard billiard(200, 200, 200, 0);
    vector<Vec2> ds = {{cos(M_PI/4),  sin(M_PI/4)}, {cos(M_PI/4 + M_PI/360),  sin(M_PI/4 + + M_PI/360)}};
    vector<Vec2> ps = {{0, 0}, {0, 0}};

    for (int i = 0; i < ps.size(); i++) {
        log_file << "p" << i << ((i == ps.size() - 1) ? "\n" : ",");
    }
    for (int i = 0; i < ps.size(); i++) {
        log_file << ps[i] << ((i == ps.size() - 1) ? "\n" : ",");
    }
    for (int i = 0; i < MAX_POINTS; i++) {
        for (int j = 0; j < ps.size(); j++) {
            Vec2 p = ps[j];
            Vec2 d = ds[j];
            p = billiard.getIntersectionPointHelper(p, d);
            d = next_reflection(billiard, d, p);

            ds[j] = d;
            ps[j] = p;
            log_file << p << ((j == ps.size() - 1) ? "\n" : ",");
        }
    }

    return 0;
}
