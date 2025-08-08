import pygame, sys, subprocess, random, os
from math import pi, sqrt
from pygame.locals import *


pygame.init()

# Colours
BACKGROUND = (0, 0, 0)

# Game Setup
FPS = 60
fpsClock = pygame.time.Clock()
WINDOW_WIDTH = 800
WINDOW_HEIGHT = 800
cx, cy = WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2

WINDOW = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption('Dynamical Billiards')

# The main function that controls the game
def main():
    looping = True
    get_points()
    # The main game loop
    while looping:
        # Get inputs
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()


        WINDOW.fill(BACKGROUND)

        pygame.display.update()
        fpsClock.tick(FPS)

# def get_points():
#     source_dir = "."  # Directory with CMakeLists.txt
#     build_dir = "logic/cmake-build-debug"
#
#     result_config = subprocess.run(["cmake", source_dir], cwd=build_dir, capture_output=True, text=True)
#
#     result_build = subprocess.run(["cmake", "--build", "."], cwd=build_dir, capture_output=True, text=True)
#
#     executable = build_dir + "/logic.exe"
#     args = ["arg1", "arg2"]
#     result = subprocess.run([executable] + args, capture_output=True, text=True)
#
#     print("Program output:")
#     print(result.stdout)
#     print("Program errors (if any):")
#     print(result.stderr)





if __name__=="__main__":
    main()