import pygame, sys, subprocess, random, os
from math import pi, sqrt
from pygame.locals import *
import LightBeam

pygame.init()

# Colours
BACKGROUND = (0, 0, 0)

# Game Setup
FPS = 60
fpsClock = pygame.time.Clock()
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 800
cx, cy = WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2
ALL_POINTS = LightBeam.get_points()

WINDOW = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption('Dynamical Billiards')

# The main function that controls the game
def main():
    looping = True
    #Variables
    indices = [1] * len(ALL_POINTS)
    t = [0] * len(ALL_POINTS)
    all_trail =  [[] for _ in range(len(ALL_POINTS))]
    speed = [5]* len(ALL_POINTS)
    color_offsets = []
    for _ in range(len(ALL_POINTS)):
        # small random offset between -20 and +20 for each color channel
        offset = (
            random.randint(-20, 20),
            random.randint(-20, 20),
            random.randint(-20, 20)
        )
        color_offsets.append(offset)

    # The main game loop
    while looping:
        # Get inputs
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()

        WINDOW.fill(BACKGROUND)
        LightBeam.draw_billiard(200, 200, 200, 0, cx, cy, WINDOW)
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

                r_offset, g_offset, b_offset = color_offsets[k]

                red = int(min(max(64 * j / (4 * len(trail)) + r_offset, 0), 255))
                green = int(min(max(224 * j / (2 * len(trail)) + g_offset, 0), 255))
                blue = int(min(max(208 * j / (4 * len(trail)) + b_offset, 0), 255))

                pygame.draw.line(WINDOW, (red, green, blue), start_pos, end_pos, width=1)
            if len(trail) > speed[k] * 8: trail.pop(0)

            t[k] += 1

            if t[k] > total_frames:
                t[k] = 0
                indices[k] += 1
                speed[k] = min(speed[k] + 0.01, 200)

        pygame.display.update()
        fpsClock.tick(FPS)

if __name__=="__main__":
    main()