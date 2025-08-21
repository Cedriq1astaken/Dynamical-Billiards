from math import sqrt, pi, cos, sin
import pygame.draw as draw
import numpy as np
import pandas as pd
from pandas.core.internals.base import ensure_np_dtype
import struct

import run_cpp as cpp

epsilon = 1e-6


def get_points(shape: tuple, x0: float, y0: float, angle: float, count: int, scatterer: str, width, height, dh, dt,
               sigma, k):
    a, b, l, h = shape

    cpp.run_cpp_with_args(a, b, l, h, x0, y0, angle, count, scatterer, width, height, dh, dt, sigma, k)
    points = []
    with open("data/classical_data.bin", "rb") as f:
        count = struct.unpack("i", f.read(4))[0]
        max_points = struct.unpack("i", f.read(4))[0]

        for j in range(count):
            particle_points = []
            for t in range(max_points):
                x, y = struct.unpack("dd", f.read(16))
                particle_points.append((x, y))
            points.append(particle_points)

    data_quantum = []
    with open("data/quantum_data.bin", "rb") as f:
        nx = np.frombuffer(f.read(4), dtype=np.int32)[0]
        ny = np.frombuffer(f.read(4), dtype=np.int32)[0]
        max_points = np.frombuffer(f.read(4), dtype=np.int32)[0]

        # Read all probability densities
        data_quantum = np.frombuffer(f.read(), dtype=np.float32)
        data_quantum = data_quantum.reshape((max_points + 1, nx, ny))  # +1 for first timestep


    return points, data_quantum


def draw_billiard(shape: tuple, cx: float, cy: float, scatterer: list, screen) -> None:
    a, b, l, h = shape
    a, b = b, a
    color = (255, 255, 255)
    TOP = cy - a - h
    BOTTOM = cy + a + h
    LEFT = cx - b - l
    RIGHT = cx + b + l

    # Rectangular parts
    draw.line(screen, color, (LEFT + b, TOP), (RIGHT - b, TOP), 1)
    draw.line(screen, color, (LEFT + b, BOTTOM), (RIGHT - b, BOTTOM), 1)
    draw.line(screen, color, (LEFT, TOP + a), (LEFT, BOTTOM - a), 1)
    draw.line(screen, color, (RIGHT, TOP + a), (RIGHT, BOTTOM - a), 1)

    # Quarter circles
    draw.arc(screen, color, (LEFT, TOP, b * 2, a * 2), pi / 2, pi)
    draw.arc(screen, color, (LEFT, BOTTOM - a * 2, b * 2, a * 2), pi, 3 * pi / 2)
    draw.arc(screen, color, (RIGHT - 2 * b, BOTTOM - a * 2, 2 * b, 2 * a), 3 * pi / 2, 2 * pi)
    draw.arc(screen, color, (RIGHT - 2 * b, TOP, 2 * b, 2 * a), 0, pi / 2)

    for scatter in scatterer:
        draw.circle(screen, color, (cx + scatter[0], cy - scatter[1]), scatter[2], width=1)


def move(i: int, t: int, points: list, total_frames) -> tuple:
    p1 = points[i]
    p0 = points[i - 1]

    x = p0[0] + (p1[0] - p0[0]) * t / total_frames
    y = p0[1] + (p1[1] - p0[1]) * t / total_frames

    return x, y


def get_boundary(shape: tuple, scatterer, width, height, dh):
    np.set_printoptions(threshold=np.inf)

    m, n = int(width / dh), int(height / dh)

    # Center of boundary
    cx, cy = m // 2, n // 2

    a, b, l, h = shape

    boundary = np.zeros((n, m), dtype=np.uint8)
    lx, hy = int(l / dh), int(h / dh)
    ax, by = int(a / dh), int(b / dh)

    # # --- Rectangle walls (note: rows = y, cols = x) ---
    # # top and bottom
    if lx > 0:
        boundary[cy + hy + by, cx - lx: cx + lx + 1] = 1
        boundary[cy - hy - by, cx - lx: cx + lx + 1] = 1
    # left and right
    if (hy > 0):
        boundary[cy - hy: cy + hy + 1, cx - lx - ax] = 1
        boundary[cy - hy: cy + hy + 1, cx + lx + ax] = 1
    boundary[cy, cx] = 0

    def set_pixel(x, y):
        x, y = int(round(x)), int(round(y))
        if 0 <= x < m and 0 <= y < n:
            boundary[y][x] = 1

    if a != 0 or b != 0:
        step = pi / (8 * max(ax, by))
        centers = [
            (cx + lx, cy - hy),  # Q1: top-right
            (cx - lx, cy - hy),  # Q2: top-left
            (cx - lx, cy + hy),  # Q3: bottom-left
            (cx + lx, cy + hy)  # Q4: bottom-right
        ]

        quadrant_ranges = [
            (0, pi / 2),  # Q1: 0° to 90°
            (pi / 2, pi),  # Q2: 90° to 180°
            (pi, 3 * pi / 2),  # Q3: 180° to 270°
            (3 * pi / 2, 2 * pi)  # Q4: 270° to 360°
        ]
        for i, c in enumerate(centers):
            start, end = quadrant_ranges[i]
            t = start
            while t < end:
                x = c[0] + ax * cos(t)
                y = c[1] - by * sin(t)
                set_pixel(x, y)
                t += step

    for scatter in scatterer:
        c_x, c_y, r = scatter
        step = pi / (8 * r)
        t = 0
        while t <= 2 * pi:
            x = cx + (c_x + r * cos(t)) / dh
            y = cy + (c_y - r * sin(t)) / dh
            set_pixel(x, y)
            t += step

    return boundary


def probability_to_rgb(p, gamma=0.5):
    # Clamp
    p = max(0.0, min(1.0, p))
    # Boost contrast: gamma < 1 brightens high p
    p = p ** gamma

    if p < 0.5:
        # Black → Cyan
        t = p / 0.5
        r = 0
        g = int(255 * t)
        b = int(255 * t)
    else:
        # Cyan → Magenta
        t = (p - 0.5) / 0.5
        r = int(255 * t)
        g = int(255 * (1 - t))
        b = 255

    return r, g, b

def idx(i, j, nx) :
    return i * nx + j


