from math import sqrt
epsilon = 1e-6

def dot_product(a: tuple, b: tuple) -> float:
    return a[0] * b[0] + a[1] * b[1]

def normalization(n: tuple) -> tuple:
    magnitude = sqrt(n[0] ** 2 + n[1] ** 2)
    return n[0] / magnitude, n[1] / magnitude

def ray(d: tuple, n: tuple, dot) -> tuple:
    return d[0] - 2 * dot * n[0], d[1] - 2 * dot * n[1]

def reflection(a: float, b: float, radius: float, x0: float, y0: float, x_ray: float, y_ray: float):
    points = [(x0,y0)]

    D = a * x_ray ** 2 + b * y_ray ** 2
    E = 2 * (a * x0 * x_ray + b * y0 * y_ray)
    F = a * x0 ** 2 + b * y0 ** 2 - radius ** 2

    t1 = (-E + sqrt(E ** 2 - 4 * D * F)) / (2 * D)
    t2 = (-E - sqrt(E ** 2 - 4 * D * F)) / (2 * D)

    t_valid = t1 if abs(t2) < epsilon else t2

    new_x = x0 + x_ray * t_valid
    new_y = y0 + y_ray * t_valid

    points.append((new_x, new_y))

    while not len(points) > 10_000:
        x1, y1 = points[-1]
        x0, y0 = points[len(points) - 2]

        d = (x1 - x0, y1 - y0)
        n = (4 * x1, y1)
        n_normalized = normalization(n)
        dot = dot_product(d, n_normalized)
        r = ray(d, n_normalized, dot)

        D = a * r[0] ** 2 + b * r[1] ** 2
        E = 2 * (a * x1 * r[0] + b * y1 * r[1])
        F = a * x1 ** 2 + b * y1 ** 2 - radius ** 2

        t1 = (-E + sqrt(E ** 2 - 4 * D * F)) / (2 * D)
        t2 = (-E - sqrt(E ** 2 - 4 * D * F)) / (2 * D)

        t_valid = t1 if abs(t2) < epsilon else t2

        new_x = x1 + r[0] * t_valid
        new_y = y1 + r[1] * t_valid

        points.append((new_x, new_y))

    return points

def move(i: int, t: int, points: list, total_frames) -> tuple:
    p1 = points[i]
    p0 = points[i -1]

    x = p0[0] + (p1[0] - p0[0]) * t/total_frames
    y = p0[1] + (p1[1] - p0[1]) * t/total_frames

    return x, y

