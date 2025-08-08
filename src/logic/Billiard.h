#ifndef STADIUM_H
#define STADIUM_H

#include <vector>
#include "Vec2.h"

using namespace std;

class Billiard {
    private:
        double a, b; // ellipse radii (or circle if a == b)
        double l;    // length of the flat section
        double h;    // length of the vertical section (usually == 0)
    public:
        Billiard(double a, double b, double l, double h);

        // Getters
        double getA() const;
        double getB() const;
        double getL() const;
        double getH() const;

        // Methods
        Vec2 getIntersectionPointHelper(Vec2 p, Vec2 d) const;
        Vec2 getIntersectionPointLines(Vec2 p, Vec2 d) const;
        Vec2 getIntersectionPointCircle(Vec2 p, Vec2 d) const;
        Vec2 getNormal(Vec2 p) const;
};



#endif //STADIUM_H
