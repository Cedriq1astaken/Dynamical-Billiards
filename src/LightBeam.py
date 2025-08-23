from math import sqrt, pi, cos, sin
import pygame.draw as draw
import pygame.gfxdraw as gfx
import numpy as np
import pandas as pd
from pandas.core.internals.base import ensure_np_dtype
import struct

import run_cpp as cpp

epsilon = 1e-6


def get_points(shape: tuple, x0: float, y0: float, angle: float, count: int, scatterer: str, width, height, dh, dt,
               sigma, k):
    a, b, l, h = shape

    cpp.run_cpp_with_args(a + epsilon, b + epsilon, l, h, x0, y0, angle, count, scatterer, width, height, dh, dt, sigma, k)
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
    color2 = (128, 128, 128)
    TOP = cy - a - h
    BOTTOM = cy + a + h
    LEFT = cx - b - l
    RIGHT = cx + b + l
    width = 2

    # Rectangular parts
    draw.aaline(screen, color, (LEFT + b, TOP), (RIGHT - b, TOP), width=width)
    draw.aaline(screen, color, (LEFT + b, BOTTOM), (RIGHT - b, BOTTOM), width=width)
    draw.aaline(screen, color, (LEFT, TOP + a), (LEFT, BOTTOM - a), width=width)
    draw.aaline(screen, color, (RIGHT, TOP + a), (RIGHT, BOTTOM - a), width=width)

    # Quarter circles
    draw.arc(screen, color, (LEFT, TOP, b * 2, a * 2), pi / 2, pi, width=width)
    draw.arc(screen, color, (LEFT, BOTTOM - a * 2, b * 2, a * 2), pi, 3 * pi / 2, width=width)
    draw.arc(screen, color, (RIGHT - 2 * b, BOTTOM - a * 2, 2 * b, 2 * a), 3 * pi / 2, 2 * pi, width=width)
    draw.arc(screen, color, (RIGHT - 2 * b, TOP, 2 * b, 2 * a), 0, pi / 2, width=width)

    for scatter in scatterer:
        gfx.filled_circle(screen, cx + scatter[0], cy - scatter[1], scatter[2] -1, color2)
        draw.aacircle(screen, color, [cx + scatter[0], cy - scatter[1]], scatter[2], 4)




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
    p = p ** gamma

    if p < 0.5:
        # Black → Dark blue
        t = p / 0.5
        r = 0
        g = 0
        b = int(128 * t)   # 0 → 128
    else:
        # Dark blue → Light blue
        t = (p - 0.5) / 0.5
        r = int(0 + (173 - 0) * t)     # 0 → 173
        g = int(0 + (216 - 0) * t)     # 0 → 216
        b = int(128 + (255 - 128) * t) # 128 → 255

    return r, g, b

def idx(i, j, nx) :
    return i * nx + j


