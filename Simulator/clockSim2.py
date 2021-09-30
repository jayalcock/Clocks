import pygame
import sys
import math
import time
import random
import numpy as np
from pygame.locals import *

columns = 15    # number of clock columns
rows = 8     # number of clock rows

# define colours
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)

# screen class to set up window and background
class Screen:

    def __init__(self, screen_title):
        self.screen_title = screen_title
        self.height = 0
        self.width = 0
        self.screen = 0

        self.getDisplayInfo()
        self.setupScreen()

    # gets video info (resolution) from current active display
    def getDisplayInfo(self):
        vidinfo = pygame.display.Info()
        self.height = vidinfo.current_h
        self.width = vidinfo.current_w

    # sets up screen size and parameters that will be drawn on
    def setupScreen(self):
        self.screen = pygame.display.set_mode((self.width, self.height), 0, 32)
        pygame.display.set_caption(self.screen_title)
        pygame.mouse.set_visible(1)

# Background class to draw blank background
class Background:

    def __init__(self, display):
        self.background = 0
        self.width = display.width
        self.height = display.height
        self.backgroundColour = WHITE

        self.setupBackground()

    # sets up background to be printed on
    def setupBackground(self):
        self.background = pygame.Surface([self.width, self.height])
        self.background = self.background.convert()
        self.background.fill(self.backgroundColour)

    # draws background to screen
    def draw(self, display):
        display.screen.blit(self.background, (0, 0))

# draw clock arms
class clock_Arm:

    def __init__(self, centre, length, angle):
        self.centre = centre
        self.length = length
        self.angle = angle
        self.colour = BLACK
        self.lineWidth = 1
        self.endPoint = [0, 0]

    # calculates arm end point given angle and length
    # from centre of rect object
    def endPtCalc(self):
        ang = math.pi - math.radians(self.angle)
        self.endPoint = [(math.sin(ang) * self.length) + self.centre[0],
                         (math.cos(ang) * self.length) + self.centre[1]]

    # draws arm objects on surface and centres rect
    # object on centre of clock face object
    def draw(self, display):
        self.endPtCalc()
        pygame.draw.line(display.screen, self.colour, self.centre,
                         self.endPoint, self.lineWidth)

# Initiate clock class
class Clock:

    def __init__(self, centre, diameter):
        self.centre = centre
        self.hourArmLength = (diameter / 2) - 10
        self.minuteArmLength = (diameter / 2) - 1
        self.diameter = diameter
        self.minuteArm = clock_Arm(self.centre, self.minuteArmLength, 0)
        self.hourArm = clock_Arm(self.centre, self.hourArmLength, 0)
        self.colour = BLACK
        self.lineWidth = 2

    def draw(self, display):
        pygame.draw.circle(display.screen, self.colour, self.centre,
                           self.diameter / 2, self.lineWidth)

# Clock matrix to setup and control clock classes
class Clock_Matrix:

    def __init__(self, columns, rows, screen):
        self.columns = columns
        self.rows = rows
        self.height = screen.height
        self.width = screen.width
        self.clockDiameter = 0

        # populates matrix with empty objects
        self.clockMtx = np.empty([self.columns, self.rows], dtype=Clock)

        self.calculateDiameter()
        self.populateMatrix()

    # Calculates correct clock face diameter to fit on screen
    # whether limited by width or height
    def calculateDiameter(self):
        if(self.height / self.rows > self.width / self.columns):
            self.clockDiameter = (self.width / self.columns)
        else:
            self.clockDiameter = (self.heigh / self.rows) - 10

    # Populates clock matrix with clock objects on equally spaced centre points
    def populateMatrix(self):
        for j in range(self.rows):
            for i in range(self.columns):
                centre_x = self.clockDiameter * i + self.clockDiameter / 2
                centre_y = self.clockDiameter * j + self.clockDiameter / 2
                self.clockMtx[i, j] = Clock((centre_x, centre_y),
                                            self.clockDiameter)

    # Draws clock frames on surface
    def draw(self, display, angleMtx):
        for j in range(self.rows):
            for i in range(self.columns):
                # update minute arm and draw
                self.clockMtx[i, j].minuteArm.angle =\
                    angleMtx.minAngleMtx[i, j]
                self.clockMtx[i, j].minuteArm.draw(display)

                # update hour arm and draw
                self.clockMtx[i, j].hourArm.angle = angleMtx.hourAngleMtx[i, j]
                self.clockMtx[i, j].hourArm.draw(display)

                # draw clock frames
                self.clockMtx[i, j].draw(display)


# Engine to drive clock arms
class pattern_Engine:

    def __init__(self, minute=0, hour=0, minRate=10, hourRate=10):
        self.pattern = 0
        self.minRate = minRate
        self.hourRate = hourRate
        self.minAngle = minute
        self.hourAngle = hour
        # Matricies for storing calcualted angles
        self.minAngleMtx = np.empty([columns, rows], dtype=float)
        self.hourAngleMtx = np.empty([columns, rows], dtype=float)

        self.initialiseMatrix()

    # Initialises angle matricies in which angle data is stored for each arm
    def initialiseMatrix(self):
        for j in range(rows):
            for i in range(columns):
                self.minAngleMtx[i, j] = self.minAngle

        for j in range(rows):
            for i in range(columns):
                self.hourAngleMtx[i, j] = self.hourAngle

    def rotateAtRate(self, arm, rate, direction):

        if direction == "cw":
            if arm == "min":
                self.minAngle += rate
            else:
                self.hourAngle += rate

        else:
            if arm == "min":
                self.minAngle += rate
            else:
                self.hourAngle += rate

    def rotateToAngle(self, arm, angle):
        if arm == "min":
            self.minAngle = angle
        else:
            self.hourAngle = angle

    # Randomises rotation rate - hour and minute independent
    def randomRate(self):
        self.minRate = random.randint(-10, 10)
        self.hourRate = random.randint(-10, 10)

    def cascade(self):
        for j in range(rows):
            for i in range(columns):
                self.minAngleMtx[i, j] += self.minRate * ((j + 10) / 100)

        for j in range(rows):
            for i in range(columns):
                self.hourAngleMtx[i, j] += self.hourRate * ((j + 10) / 100)

    # draws border on outermost clocks
    def border(self):
        for i in range(columns):
            self.minAngleMtx[i, 0] = 270
            self.hourAngleMtx[i, 0] = 90
            self.minAngleMtx[i, rows - 1] = 270
            self.hourAngleMtx[i, rows - 1] = 90

        for j in range(rows):
            self.minAngleMtx[0, j] = 0
            self.hourAngleMtx[0, j] = 180
            self.minAngleMtx[columns - 1, j] = 0
            self.hourAngleMtx[columns - 1, j] = 180

        # coners
        self.minAngleMtx[0, 0] = 180
        self.hourAngleMtx[0, 0] = 90
        self.minAngleMtx[columns - 1, 0] = 270
        self.hourAngleMtx[columns - 1, 0] = 180
        self.minAngleMtx[0, rows - 1] = 0
        self.hourAngleMtx[0, rows - 1] = 90
        self.minAngleMtx[columns - 1, rows - 1] = 270
        self.hourAngleMtx[columns - 1, rows - 1] = 0


pygame.init()  # initiate pygame
sysClock = pygame.time.Clock()     # set up system clock
sysClock.tick(60)   # set framerate

# setup screen
display = Screen("Clock Sim")

# setup and display background
background = Background(display)
background.draw(display)

# Clock class matrix initialisation
clockMatrix = Clock_Matrix(columns, rows, display)

# initialised pattern engine with initial arm angles and rotation rates
patternEngine = pattern_Engine(0, 180, 5, 5)

running = True

time.sleep(1)

while running:

    start_time = time.time()

    for event in pygame.event.get():  # quit pygame
        if event.type == pygame.KEYDOWN:

            if event.key == pygame.K_r:
                patternEngine.randomRate()

        if event.type == QUIT:
            pygame.quit()
            sys.exit()
            running = False

    # run select pattern engine functions
    patternEngine.cascade()

    # border test
    patternEngine.border()

    # prints background to screen
    background.draw(display)

    # draw clocks to display
    clockMatrix.draw(display, patternEngine)

    # update display
    pygame.display.update()


pygame.quit()
