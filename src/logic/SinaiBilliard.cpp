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

vector<int> SinaiBilliard::getBoundary(double width, double height, double dh) const {
    int m = static_cast<int>(width / dh);
    int n = static_cast<int>(height / dh);

    // Center of boundary
    int cx = m / 2;
    int cy = n / 2;

    vector<int> boundary(n * m, 0); // flattened 2D array
    int lx = static_cast<int>(outer.getL() / dh);
    int hy = static_cast<int>(outer.getH() / dh);
    int ax = static_cast<int>(outer.getA() / dh);
    int by = static_cast<int>(outer.getB() / dh);

    auto idx = [&](int i, int j) {
        return i * n + j;  // row-major: x=i, y=j
    };

    if (lx > 0) {
        for (int x = cx - lx; x <= cx + lx; x++) {
            if (cy + hy + by >= 0 && cy + hy + by < n)
                boundary[idx(x, cy + hy + by)] = 1;
            if (cy - hy - by >= 0 && cy - hy - by < n)
                boundary[idx(x, cy - hy - by)] = 1;
        }
    }
    // Left and right
    if (hy > 0) {
        for (int y = cy - hy; y <= cy + hy; y++) {
            if (cx - lx - ax >= 0 && cx - lx - ax < m)
                boundary[idx(cx - lx - ax, y)] = 1;
            if (cx + lx + ax >= 0 && cx + lx + ax < m)
                boundary[idx(cx + lx + ax, y)] = 1;
        }
    }

    boundary[idx(cx, cy)] = 0;

    auto set_pixel = [&](int x, int y) {
        if (x >= 0 && x < m && y >= 0 && y < n) {
            boundary[idx(x, y)] = 1;
        }
    };

    // Rounded corners
    if (outer.getA() != 0 || outer.getB() != 0) {
        double step = M_PI / (8.0 * max(ax, by));
        vector<pair<int, int>> centers = {
            {cx + lx, cy - hy},  // Q1
            {cx - lx, cy - hy},  // Q2
            {cx - lx, cy + hy},  // Q3
            {cx + lx, cy + hy}   // Q4
        };

        vector<pair<double, double>> quadrant_ranges = {
            {0, M_PI / 2},
            {M_PI / 2, M_PI},
            {M_PI, 3 * M_PI / 2},
            {3 * M_PI / 2, 2 * M_PI}
        };

        for (int i = 0; i < 4; i++) {
            auto [start, end] = quadrant_ranges[i];
            auto [cx_, cy_] = centers[i];
            for (double t = start; t < end; t += step) {
                int x = static_cast<int>(round(cx_ + ax * cos(t)));
                int y = static_cast<int>(round(cy_ - by * sin(t)));
                set_pixel(x, y);
            }
        }
    }

    // Scatterers (circles)
    for (auto [c, r] : inner) {
        double step = M_PI / (8.0 * r);
        for (double t = 0; t <= 2 * M_PI; t += step) {
            int x = static_cast<int>(round(cx + (c.x + r * cos(t)) / dh));
            int y = static_cast<int>(round(cy + (c.y - r * sin(t)) / dh));
            set_pixel(x, y);
        }
    }

    return boundary; // flattened n*m grid
}


