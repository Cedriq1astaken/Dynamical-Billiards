#include "Billiard.h"
#include <vector>
#include <iostream>
#include "Vec2.h"

double epsilon = 1e-9;

Billiard::Billiard(double a, double b, double l, double h) {
    this->a = a; // Horizontal radius
    this->b = b; // Vertical radius
    this->l = l; // Horizontal component of the rectangular area
    this->h = h; // Vertical component of the rectangular area
}

//Getters
double Billiard::getA() const {
    return a;
}
double Billiard::getB() const {
    return b;
}
double Billiard::getL() const {
    return l;
}
double Billiard::getH() const {
    return h;
}

//Methods
Vec2 Billiard::getIntersectionPointHelper(Vec2 p, Vec2 d) const {
    Vec2 result = getIntersectionPointLines(p, d);

    if (result != p)
        return result;
    result = getIntersectionPointCircle(p, d);

    return result;
}

Vec2 Billiard::getIntersectionPointLines(Vec2 p, Vec2 d) const {
    double top = h + a;
    double bottom = -h - a;
    double left = -l - b;
    double right = l + b;

    double t = 0.0;
    double k = 0.0;

    double cross;
    Vec2 sub;

    vector<Vec2> qs = {{left + b, top}, {left + b, bottom},
                            {left, top - a}, {right, top - a}};
    vector<Vec2> us = {{2 * l, 0}, {2 * l, 0},
                            {0,- 2 * h}, {0, -2 * h}};


    for (int i = 0; i < 4; i++) {
        Vec2 q = qs[i];
        Vec2 u = us[i];
        cross = d * u;
        sub = q - p;

        if (abs(cross) < 1e-8) continue;

        t = sub * u / cross;
        k = sub * d / cross;
        if (0 <= t && 0 <= k && k <= 1) {
            Vec2 p1 = p + d * t;
            if (p1 != p) return p1;
        }

    }
    return p;
}

Vec2 Billiard::getIntersectionPointCircle(Vec2 p, Vec2 d) const {
    vector<Vec2> centers = {
        {-l,  h},  // top-left
        { l,  h},  // top-right
        { l, -h},  // bottom-right
        {-l, -h}   // bottom-left
    };

    double A = 0;
    double B = 0;
    double C = 0;
    double D = 0;

    for (int i = 0; i < 4; i++) {
        Vec2 c = centers[i];

        A = b * b * d.x * d.x + a * a * d.y * d.y;
        B = 2 * (b * b * d.x * (p.x - c.x) + a * a * d.y * (p.y - c.y));
        C = b * b * pow(p.x - c.x, 2) + a * a * pow(p.y - c.y, 2) - pow(a * b, 2);
        D = sqrt(B * B - 4 * A * C);

        if (D < 0) continue;

        double t1 = (-B + D) / (2 * A);
        double t2 = (-B - D) / (2 * A);

        if (t1 > 0) {
            Vec2 new_p = p + d * t1;
            bool inQuarter = false;
            switch (i) {
                case 0: inQuarter = (new_p.x <= c.x && new_p.y <= c.y); break; // top-left
                case 1: inQuarter = (new_p.x >= c.x && new_p.y <= c.y); break; // top-right
                case 2: inQuarter = (new_p.x >= c.x && new_p.y >= c.y); break; // bottom-right
                case 3: inQuarter = (new_p.x <= c.x && new_p.y >= c.y); break; // bottom-left
            }
            if (inQuarter) {
                return new_p;
            }
        }
        else if (t2 > 0) {
            Vec2 new_p = p + d * t2;
            bool inQuarter = false;
            switch (i) {
                case 0: inQuarter = (new_p.x <= c.x && new_p.y <= c.y); break; // top-left
                case 1: inQuarter = (new_p.x >= c.x && new_p.y <= c.y); break; // top-right
                case 2: inQuarter = (new_p.x >= c.x && new_p.y >= c.y); break; // bottom-right
                case 3: inQuarter = (new_p.x <= c.x && new_p.y >= c.y); break; // bottom-left
            }
            if (inQuarter) {
                return new_p;
            }
        }
    }
    return p;
}

Vec2 Billiard::getNormal(Vec2 p) const {
    // Lines
    if (p.x > - l && p.x < l) {
        double th = -h;
        if (p.y >= h - epsilon) return {0, 1};
        if (p.y <= -h + epsilon) return {0, -1};
    }
    if (p.y > -h && p.y < h) {
        if (p.x >= l - epsilon) return {1, 0};
        if (p.x <= -l + epsilon) return {-1, 0};
    }

    vector<Vec2> centers = {
        {-l, h},  // top-left
        { l, h},  // top-right
        { l,  -h},  // bottom-right
        {-l,  -h}   // bottom-left
    };

    for (int i = 0; i < 4; i++) {
        Vec2 c = centers[i];
        bool inQuarter = false;
        switch (i) {
            case 0: inQuarter = (p.x <= c.x && p.y >= c.y); break; // top-left
            case 1: inQuarter = (p.x >= c.x && p.y >= c.y); break; // top-right
            case 2: inQuarter = (p.x >= c.x && p.y <= c.y); break; // bottom-right
            case 3: inQuarter = (p.x <= c.x && p.y <= c.y); break; // bottom-left
        }
        if (inQuarter) {
            if (abs(p.x - c.x) <= epsilon && abs(p.y - c.y) <= epsilon) {
                return {(p.x > 0 ? 1.0 : -1.0),
                       (p.y > 0 ? 1.0 : -1.0) };
            }
            return {(b * b * (p.x - c.x)), (a * a * (p.y - c.y))};
        }
    }
}






