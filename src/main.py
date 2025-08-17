import pygame, sys, random, os
from math import sqrt, pi
from pygame.locals import *
import LightBeam
import logic.Schrodinger
import run_cpp as cpp
import numpy as np

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
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 800
cx, cy = WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2

# Setup properties
shape = (0, 0, 300, 300)
x0 = 0
y0 = 0
angle = 0
count = 2
mass = 1
dh = 8
scatterer = [] # Takes tuples with a center and a radius
scatterer_cpp = "[" + ",".join(f"({x}, {y}, {r})" for x,y,r in scatterer) + "]"

ALL_POINTS = LightBeam.get_points(shape, x0, y0, angle, count, scatterer_cpp, WINDOW_WIDTH, WINDOW_HEIGHT)

WINDOW = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption('Dynamical Billiards')


schrodinger = Schrodinger(dh, 5, 15)

# The main function that controls the game
def main():
    global quantum
    looping = True
    #Variables
    indices = [1] * len(ALL_POINTS)
    t = [0] * len(ALL_POINTS)
    all_trail =  [[] for _ in range(len(ALL_POINTS))]
    speed = [5]* len(ALL_POINTS)
    np.set_printoptions(threshold=np.inf)
    boundary = LightBeam.get_boundary(shape, scatterer, WINDOW_WIDTH, WINDOW_HEIGHT, dh)
    psi = schrodinger.gaussian_packet(boundary.shape[1], boundary.shape[0], x0, y0, mass, angle * pi/180)
    print(psi.shape)
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

                pygame.draw.line(WINDOW, (red, green, blue), start_pos, end_pos, width=1)
            if len(trail) > speed[k] * 500: trail.pop(0)

            t[k] += 1

            if t[k] > total_frames:
                t[k] = 0
                indices[k] += 1
                speed[k] = min(speed[k] + 0.01, 200)

    def quantum_display(psi):
        prob_density = np.abs(psi) ** 2
        prob_density /= np.max(prob_density)
        height, width = prob_density.shape

        for i in range(height):
            for j in range(width):
                p = prob_density[i][j]
                pygame.draw.rect(WINDOW, LightBeam.probability_to_rgb(p), (j * dh, (WINDOW_HEIGHT - i * dh), dh, dh))
        psi = schrodinger.RK4_Schrodinger(psi, boundary)

        return psi

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
        WINDOW.fill(BACKGROUND)
        if quantum:
            psi = quantum_display(psi)
            FPS = 30
        else:
            classical_display()
            FPS = 60
        LightBeam.draw_billiard(shape, cx, cy, scatterer, WINDOW)


        pygame.display.update()
        fpsClock.tick(FPS)



if __name__=="__main__":
    main()