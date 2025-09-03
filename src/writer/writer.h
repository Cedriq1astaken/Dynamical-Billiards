#pragma once
#include <vector>
#include <string>
#include <ostream>

using namespace std;

// Forward declarations for your custom types
struct Vec2;
struct Circle;
struct Color;
class SinaiBilliard;
class Schrodinger;

// Constants (make sure MAX_POINTS, WIDTH, HEIGHT are defined somewhere else)
extern const int MAX_POINTS;
extern const int WIDTH;
extern const int HEIGHT;

// Stream operator
ostream& operator<<(ostream& os, const Vec2& v);

// Utility functions
float maximum(vector<float> v);
Vec2 move(int i, int t, vector<Vec2> points, int total_frames);
vector<Circle> parseCircles(const string& s);
Vec2 next_reflection(SinaiBilliard b, Vec2 d, Vec2 p_i);
Color probability_to_rgb(float p);

// Simulation functions
vector<vector<Vec2>> write_classical(
    SinaiBilliard billiard, Vec2 p0, double angle, int count);

vector<vector<float>> write_quantum(
    double dh, double dt, double sigma, double x0, double y0,
    double k, double theta, const SinaiBilliard& billiard);