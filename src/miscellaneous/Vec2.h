#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <iostream>

using namespace std;

struct Vec2{
    double x, y;
    // Constructor
    Vec2(double x = 0, double y = 0) : x(x), y(y) {}

    // Operator overloads
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(double scalar) const { return Vec2(x * scalar, y * scalar); }
    double operator*(const Vec2& other) const { return x * other.y - y * other.x; }
    Vec2 operator/(double scalar) const { return Vec2(x / scalar, y / scalar); }
    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return x != other.x || y != other.y; }

    // Methods
    double mag() const { return sqrt(x * x + y * y); }
    double dot(const Vec2& other) const { return x * other.x + y * other.y; }
    Vec2 normalize() {
      double magnitude = mag();
      return {x / magnitude, y / magnitude};
    }

};


#endif //VEC2_H
