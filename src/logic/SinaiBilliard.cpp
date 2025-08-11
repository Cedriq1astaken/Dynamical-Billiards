#include "SinaiBilliard.h"
#include "Billiard.h"
#include "Vec2.h"
#include <limits>
#include <cmath>
#include <vector>

using namespace std;

SinaiBilliard::SinaiBilliard(double a, double b, double l, double h)
    : outer(a, b, l, h) {}

void SinaiBilliard::addScatterer(Vec2 center, double radius) {
    inner.push_back({center, radius});
}

Vec2 SinaiBilliard::getIntersectionPoint(Vec2 p, Vec2 d) const {
    d = d.normalize();
    double t_best = numeric_limits<double>::infinity();
    Vec2 bestHit;

    // --- Outer boundary ---


    Vec2 candidate = outer.getIntersectionPointHelper(p, d);

    if (candidate != p) {
        double t = (candidate - p).mag();
        if (t > 1e-9 && t < t_best) {
            t_best = t;
            bestHit = candidate;
        }
    }


    // --- All scatterers ---
    for (const auto& c : inner) {
        Vec2 f = p - c.center;
        double A = d.dot(d);
        double B = 2 * f.dot(d);
        double C = f.dot(f) - c.radius * c.radius;
        double disc = B * B - 4 * A * C;

        if (disc >= 0) {
            double sqrtD = sqrt(disc);
            double t1 = (-B - sqrtD) / (2 * A);
            double t2 = (-B + sqrtD) / (2 * A);

            if (t1 > 1e-9 && t1 < t_best) {
                t_best = t1;
                bestHit = p + d * t1;
            }
            if (t2 > 1e-9 && t2 < t_best) {
                t_best = t2;
                bestHit = p + d * t2;
            }
        }
    }

    return bestHit;
}

Vec2 SinaiBilliard::getNormal(Vec2 p) const {
    // Check if point lies on a scatterer
    for (const auto& c : inner) {
        if (std::abs((p - c.center).mag() - c.radius) < 1e-8) {
            return (p - c.center).normalize();
        }
    }
    // Otherwise, it's on the outer boundary
    return outer.getNormal(p);
}