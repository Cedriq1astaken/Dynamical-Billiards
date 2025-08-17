#include "SinaiBilliard.h"
#include "Billiard.h"
#include "Vec2.h"
#include <limits>
#include <cmath>
#include <vector>
#include <algorithm>

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

void drawLine(vector<vector<int>>& boundary, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        boundary[x0][y0] = 1; // mark pixel
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

vector<vector<int>> SinaiBilliard::getBoundary(double WIDTH, double HEIGHT, int depth) const {
    int cols = (int)(WIDTH / dh);
    int rows = (int)(HEIGHT / dh);
    vector<vector<int>> boundary(cols, vector<int>(rows, 0));

    double d_theta = 2 * M_PI / depth;
    double theta = 0.0;

    Vec2 p_first = outer.getIntersectionPointHelper({}, {cos(theta), sin(theta)});
    int i_prev = (int)((p_first.x + WIDTH / 2) / dh);
    int j_prev = (int)((p_first.y + HEIGHT / 2) / dh);

    for (int k = 1; k <= depth; k++) {
        theta = k * d_theta;
        Vec2 p = outer.getIntersectionPointHelper({}, {cos(theta), sin(theta)});
        int i = (int)((p.x + WIDTH / 2) / dh);
        int j = (int)((p.y + HEIGHT / 2) / dh);

        drawLine(boundary, i_prev, j_prev, i, j);

        i_prev = i;
        j_prev = j;
    }

    for (const Circle& circle : inner) {
        double radius = circle.radius;
        // Choose depth to get ~1 pixel per arc length
        int depth = std::max((int)(2 * M_PI * radius / dh), 8);
        d_theta = 2 * M_PI / depth;

        Vec2 p_first = getIntersectionPoint(circle.center, {cos(0), sin(0)});
        i_prev = (int)((p_first.x + WIDTH / 2) / dh);
        j_prev = (int)((p_first.y + HEIGHT / 2) / dh);

        for (int l = 1; l <= depth; l++) {
            double theta = l * d_theta;
            Vec2 p = getIntersectionPoint(circle.center, {cos(theta), sin(theta)});
            int i = (int)((p.x + WIDTH / 2) / dh);
            int j = (int)((p.y + HEIGHT / 2) / dh);

            drawLine(boundary, i_prev, j_prev, i, j);

            i_prev = i;
            j_prev = j;
        }
    }

    return boundary;
}

