#include <iostream>
#include <cmath>
#include "Vec2.h"
#include <fstream>
#include "SinaiBilliard.h"
#include "Utils.h"
#include <algorithm>
#include "raylib.h"
#include "writer/writer.h"
using namespace std;

const double epsilon = 1e-8;
const int MAX_POINTS = 2000;
const int WIDTH = 1200;
const int HEIGHT = 1200;
bool start = false;
bool quantum = false;

double dh = 8;


void draw_classical(vector<vector<Vec2>>& ALL_POINTS,
                     vector<int>& indices,
                     vector<int>& t,
                     vector<vector<Vec2>>& all_trail,
                     vector<double>& speed) {
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;
    for (size_t k = 0; k < ALL_POINTS.size(); ++k) {
        int i = indices[k];
        vector<Vec2>& POINTS = ALL_POINTS[k];
        vector<Vec2>& trail = all_trail[k];

        Vec2 p1 = POINTS[i];
        Vec2 p0 = POINTS[i - 1];

        double distance = sqrt((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y));
        int total_frames = max(1, static_cast<int>(distance / speed[k]));

        Vec2 pos = move(i, t[k], POINTS, total_frames);

        trail.push_back(pos);

        DrawCircle(cx + static_cast<int>(pos.x), cy - static_cast<int>(pos.y), 5, WHITE);

        for (size_t j = 0; j + 1 < trail.size(); ++j) {
            int start_x = static_cast<int>(round(cx + trail[j].x));
            int start_y = static_cast<int>(round(cy - trail[j].y));

            int end_x = static_cast<int>(round(cx + trail[j+1].x));
            int end_y = static_cast<int>(round(cy - trail[j+1].y));
            DrawLine(start_x, start_y, end_x, end_y, WHITE);
        }
        // Optional: limit trail length
        if (trail.size() > static_cast<size_t>(speed[k] * 500))
            trail.erase(trail.begin());

        if (start) t[k] += 1;

        if (t[k] > total_frames) {
            t[k] = 0;
            indices[k] += 1;
            speed[k] = min(speed[k] + 0.01, 200.0);
        }
    }
}

void draw_quantum(vector<vector<float>>& QUANTUM_POINTS, int& q, double dh) {
    int nx = WIDTH/dh;
    int ny = HEIGHT/dh;
    vector<float> points = QUANTUM_POINTS[q];

    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            float p = points[idx(j, i, ny)];
            Color color = probability_to_rgb(p);
            DrawRectangle(j * dh, i * dh, dh, dh, color);
        }
    }
    if (start) q += 1;
}


void windowVis(SinaiBilliard billiard, vector<vector<Vec2>>& ALL_POINTS, vector<vector<float>> QUANTUM_POINTS) {
    // For anti-aliasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
     // Initialize the window
    InitWindow(WIDTH, HEIGHT, "Dynamical Billiard");

    // Set the target FPS (so the loop runs smoothly)
    SetTargetFPS(60);

    // Classical variables
    vector<int> indices = vector<int>(ALL_POINTS.size(), 1);
    vector<int> t = vector<int>(ALL_POINTS.size(), 0);
    vector<vector<Vec2>> all_trail = vector<vector<Vec2>>(ALL_POINTS.size(), vector<Vec2>());
    vector<double> speed = vector<double>(ALL_POINTS.size(), 100.0);

    // Quantum variables
    int q = 0;

    // Main loop
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            quantum = !quantum;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            start = !start;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (!quantum)
            draw_classical(ALL_POINTS, indices, t, all_trail, speed);
        else
            draw_quantum(QUANTUM_POINTS, q, dh);
        billiard.draw(WIDTH/2, HEIGHT/2);

        EndDrawing();
    }
    // Close the window and clean up
    CloseWindow();
}

int main() {
    double a     = 400;
    double b     = 500;
    double l     = 0;
    double h     = 0;
    double x0    = 0;
    double y0    = 0;
    double angle = 85 * M_PI / 180.0;
    int count    = 1;

    SinaiBilliard bill(a, b, l, h);
    vector<vector<Vec2>> data_classical = write_classical(bill, {x0, y0}, angle, count);
    vector<vector<float>> data_quantum = write_quantum(dh, 3, 10, x0, y0, 100, angle, bill);

    windowVis(bill, data_classical, data_quantum);
    return 0;
}