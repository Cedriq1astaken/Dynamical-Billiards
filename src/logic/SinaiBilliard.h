#ifndef SINAIBILLIARD_H
#define SINAIBILLIARD_H
#include "Billiard.h"
#include "Vec2.h"
#include <vector>

struct Circle {
    Vec2 center;
    double radius;
};

class SinaiBilliard {
private:
    Billiard outer;              // Outer boundary
    std::vector<Circle> inner;   // Inner scatterers

public:
    SinaiBilliard(double a, double b, double l, double h);
    void addScatterer(Vec2 center, double radius);
    Vec2 getIntersectionPoint(Vec2 p, Vec2 d) const;
    Vec2 getNormal(Vec2 p) const;
};



#endif //SINAIBILLIARD_H
