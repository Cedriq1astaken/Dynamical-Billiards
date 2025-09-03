#ifndef UTILS_H
#define UTILS_H

#include <complex>
#include <vector>
#include <cmath>
#include "raylib.h"
#include "Vec2.h"

inline int idx(int i, int j, int Ny) {
    return i * Ny + j;
}

// Draw an elliptical arc (outline) using doubles
inline void DrawEllipseArc(Vector2 center, double a, double b,
                    double startAngle, double endAngle,
                    int segments, Color color, double thickness = 1.0)
{
    if (segments < 1) segments = 1;

    float cx = static_cast<float>(center.x);
    float cy = static_cast<float>(center.y);
    float radiusX = static_cast<float>(a);
    float radiusY = static_cast<float>(b);
    float thick = static_cast<float>(thickness);

    Vector2 prev = { cx + radiusX * cosf(static_cast<float>(startAngle * M_PI / 180.0)),
                     cy + radiusY * sinf(static_cast<float>(startAngle * M_PI / 180.0)) };

    for (int i = 1; i <= segments; ++i) {
        float theta = static_cast<float>((startAngle + (endAngle - startAngle) * i / segments) * M_PI / 180.0);
        Vector2 curr = { cx + radiusX * cosf(theta),
                         cy + radiusY * sinf(theta) };
        DrawLineEx(prev, curr, thick, color);
        prev = curr;
    }
}

// Filled elliptical “pie slice” using doubles
inline void DrawFilledEllipseArc(Vector2 center, double a, double b,
                          double startAngle, double endAngle,
                          int segments, Color color)
{
    if (segments < 1) segments = 1;

    float cx = static_cast<float>(center.x);
    float cy = static_cast<float>(center.y);
    float radiusX = static_cast<float>(a);
    float radiusY = static_cast<float>(b);

    Vector2 prev = { cx + radiusX * cosf(static_cast<float>(startAngle * M_PI / 180.0)),
                     cy + radiusY * sinf(static_cast<float>(startAngle * M_PI / 180.0)) };

    for (int i = 1; i <= segments; ++i) {
        float theta = static_cast<float>((startAngle + (endAngle - startAngle) * i / segments) * M_PI / 180.0);
        Vector2 curr = { cx + radiusX * cosf(theta),
                         cy + radiusY * sinf(theta) };
        DrawTriangle({cx, cy}, prev, curr, color);
        prev = curr;
    }
}
#endif
