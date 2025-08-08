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
    os << v.x << ", " << v.y << endl;
    return os;
};

Vec2 next_reflection(Billiard b,Vec2 d, Vec2 p_i) { //p_i == point of intersection
    Vec2 n = b.getNormal(p_i);
    Vec2 n_normalized = n.normalize();

    double dot = d.dot(n_normalized);
    return d - n_normalized * (2 * dot);
}

int main(int argc, char *argv[]) {
    double a = atof(argv[0]);
    double b = atof(argv[1]);
    double l = atof(argv[2]);
    double h = atof(argv[3]);

    double dx = atof(argv[4]);
    double dy = atof(argv[5]);

    double x0 = atof(argv[6]);
    double y0 = atof(argv[7]);

    cout << a << endl;
    cout << b << endl;
    cout << l << endl;
    cout << h << endl;
    cout << dx << endl;
    cout << dy << endl;
    cout << x0 << endl;
    cout << y0 << endl;

    fstream log_file("../../data/data.csv");
    vector<Vec2> points = {{0, 0}};
    Billiard billiard(25, 25, 100, 0);
    Vec2 d = {1, 0};
    Vec2 p = Vec2();


    log_file << "x,y" << endl;
    for (int i = 0; i < MAX_POINTS; i++) {
        p = billiard.getIntersectionPointHelper(points.back(), d);
        points.push_back(p);
        d = next_reflection(billiard, d, p);
        log_file << p << "," << '\n';
    }

    return 0;
}
