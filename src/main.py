import pygame, sys, random, os
from math import sqrt, pi
from pygame.locals import *
import LightBeam
import logic.Schrodinger
import run_cpp as cpp
import pygame

from src.logic.Schrodinger import Schrodinger

pygame.init()
cpp.compile_cpp()

# Mode
quantum = False

# Colours
BACKGROUND = (0, 0, 0)

# Game Setup
FPS = 60
fpsClock = pygame.time.Clock()
WINDOW_WIDTH = 800
WINDOW_HEIGHT = 900
cx, cy = WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2

# Setup properties
shape = (200, 200, 200, 0)
x0 = 0
y0 = 150
angle = 40
count = 1
mass = 0.025
dh = 50
dt = 0.1
sigma = dh * 25
scatterer = [] # Takes tuples with a center and a radius
scatterer_cpp = "[" + ",".join(f"({x}, {y}, {r})" for x,y,r in scatterer) + "]"

ALL_POINTS, QUANTUM_POINTS = LightBeam.get_points(shape, x0, y0, angle, count, scatterer_cpp, WINDOW_WIDTH, WINDOW_HEIGHT, dh, dt, sigma * dt, 0.3/dh)

WINDOW = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption('Dynamical Billiards')


# The main function that controls the game
def main():
    global quantum
    looping = True
    #Variables
    indices = [1] * len(ALL_POINTS)
    t = [0] * len(ALL_POINTS)
    all_trail =  [[] for _ in range(len(ALL_POINTS))]
    speed = [5]* len(ALL_POINTS)

    # Logic for visualizing classical paths
    def classical_display():
        for k, POINTS in enumerate(ALL_POINTS):
            i = indices[k]
            trail = all_trail[k]
            p1 = POINTS[i]
            p0 = POINTS[i - 1]
            distance = sqrt((p1[0] - p0[0]) ** 2 + (p1[1] - p0[1]) ** 2)
            total_frames = max(1, int(distance / speed[k]))

            x, y = LightBeam.move(i, t[k], POINTS, total_frames)
            trail.append((x, y))

            pygame.draw.circle(WINDOW, (64, 224, 208), (cx + x, cy - y), 2)
            for j in range(len(trail) - 1):
                start_pos = (round(cx + trail[j][0]), round(cy - trail[j][1]))
                end_pos = (round(cx + trail[j + 1][0]), round(cy - trail[j + 1][1]))

                red = 64
                green = 224
                blue = 208

                pygame.draw.aalines(WINDOW, (red, green, blue), False,[start_pos, end_pos])
            if len(trail) > speed[k] * 500: trail.pop(0)

            t[k] += 1

            if t[k] > total_frames:
                t[k] = 0
                indices[k] += 1
                speed[k] = min(speed[k] + 0.01, 200)

    def quantum_display():
        nx = int(WINDOW_WIDTH/dh)
        ny = int(WINDOW_HEIGHT/dh)
        points = QUANTUM_POINTS[t[0]]

        for i in range(ny):
            for j in range(nx):
                color = LightBeam.probability_to_rgb(points[j, i])
                pygame.draw.rect(WINDOW, color, (j * dh, i * dh, dh, dh))
        t[0] += 1


    # The main game loop
    while looping:
        # Get inputs
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_q:
                    quantum = not quantum
                if event.key == pygame.K_p:
                    t[0] += 1
        WINDOW.fill(BACKGROUND)
        if quantum:
            FPS = 30
            quantum_display()
        else:
            FPS = 60
            classical_display()

        LightBeam.draw_billiard(shape, cx, cy, scatterer, WINDOW)


        pygame.display.update()
        fpsClock.tick(FPS)



if __name__=="__main__":
    main()