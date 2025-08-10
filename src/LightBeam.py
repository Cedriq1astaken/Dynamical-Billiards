from math import sqrt, pi
import pygame.draw as draw
import csv
import pandas as pd
import run_cpp as cpp

epsilon = 1e-6

def get_points(a: float, b: float, l: float, h: float, x0: float, y0: float, angle: float, count: int):
    cpp.run_cpp_with_args(a, b, l, h, x0, y0, angle, count)
    points = []
    file = pd.read_csv('data/data.csv')
    for i in range(len(file.keys())):
        current = list(file[f'p{i}'])
        for i, coords in enumerate(current):
            current[i] = tuple(map(lambda n: float(n), coords.split("|")))
        points.append(current)
    return points

def draw_billiard(a: float, b: float, l: float, h: float, cx: float, cy: float, screen) -> None:
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
    draw.arc(screen, color, (LEFT, TOP, b * 2, a * 2), pi/2, pi)
    draw.arc(screen, color, (LEFT, BOTTOM - a * 2, b * 2, a * 2), pi, 3 * pi/2)
    draw.arc(screen, color, (RIGHT - 2 * b, BOTTOM - a *2, 2 * b, 2 * a), 3* pi/2, 2 * pi)
    draw.arc(screen, color, (RIGHT - 2 * b, TOP, 2 * b, 2 * a), 0, pi/2)

def move(i: int, t: int, points: list, total_frames) -> tuple:
    p1 = points[i]
    p0 = points[i -1]

    x = p0[0] + (p1[0] - p0[0]) * t/total_frames
    y = p0[1] + (p1[1] - p0[1]) * t/total_frames

    return x, y

