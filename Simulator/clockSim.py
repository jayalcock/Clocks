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

# clock number dictionary
timeAngle = {'12': 0, '1': 30, '2': 60, '3': 90, '4': 120, '5': 150,
             '6': 180, '7': 210, '8': 240, '9': 270, '10': 300, '11': 330}

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
        self.targetAngle = 400.0
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

    def __init__(self, screen):
        self.height = screen.height
        self.width = screen.width
        self.clockDiameter = 0

        # populates matrix with empty objects
        self.clockMtx = np.empty([columns, rows], dtype=Clock)
        self.calculateDiameter()
        self.populateMatrix()

    # Calculates correct clock face diameter to fit on screen
    # whether limited by width or height
    def calculateDiameter(self):
        if(self.height / rows > self.width / columns):
            self.clockDiameter = (self.width / columns)
        else:
            self.clockDiameter = (self.height / rows) - 10

    # Populates clock matrix with clock objects on equally spaced centre points
    def populateMatrix(self):
        for j in range(rows):
            for i in range(columns):
                centre_x = self.clockDiameter * i + self.clockDiameter / 2
                centre_y = self.clockDiameter * j + self.clockDiameter / 2
                self.clockMtx[i, j] = Clock((centre_x, centre_y),
                                            self.clockDiameter)

    # Draws clock frames on surface
    def draw(self, display, angleMtx):
        for j in range(rows):
            for i in range(columns):
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

    # determine difference in angle between current and target
    def angleCheck(self, currentAngle, targetAngle):
        if(abs(targetAngle - currentAngle) <= 1):
            return True
        else:
            return False

    # testing cascade pattern across matrix
    def cascade(self, clockMatrix):
        for j in range(rows):
            for i in range(columns):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle,
                                    clockMatrix.clockMtx[i, j].
                                    minuteArm.targetAngle)):
                    self.minAngleMtx[i, j] =\
                        clockMatrix.clockMtx[i, j].minuteArm.targetAngle
                    continue
                else:
                    self.minAngleMtx[i, j] +=\
                        self.minRate * ((j + 5.0) / 100.0)

                    if self.minAngleMtx[i, j] > 360.0:
                        self.minAngleMtx[i, j] -= 360.0
                    elif self.minAngleMtx[i, j] < 0.0:
                        self.minAngleMtx[i, j] += 360.0

        for j in range(rows):
            for i in range(columns):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle,
                   clockMatrix.clockMtx[i, j].hourArm.targetAngle)):
                    self.hourAngleMtx[i, j] =\
                        clockMatrix.clockMtx[i, j].hourArm.targetAngle
                    continue
                else:
                    self.hourAngleMtx[i, j] +=\
                        self.hourRate * ((j + 5.0) / 100.0)
                    if self.hourAngleMtx[i, j] > 360.0:
                        self.hourAngleMtx[i, j] -= 360.0
                    elif self.hourAngleMtx[i, j] < 0.0:
                        self.hourAngleMtx[i, j] += 360.0

    # draws border on outermost clocks
    def border(self, clockMatrix):

        for i in range(columns):
            clockMatrix.clockMtx[i, 0].minuteArm.targetAngle = 270
            clockMatrix.clockMtx[i, 0].hourArm.targetAngle = 90
            clockMatrix.clockMtx[i, rows - 1].minuteArm.targetAngle = 270
            clockMatrix.clockMtx[i, rows - 1].hourArm.targetAngle = 90

        for j in range(rows):
            clockMatrix.clockMtx[0, j].minuteArm.targetAngle = 0
            clockMatrix.clockMtx[0, j].hourArm.targetAngle = 180
            clockMatrix.clockMtx[columns - 1, j].minuteArm.targetAngle = 0
            clockMatrix.clockMtx[columns - 1, j].hourArm.targetAngle = 180

        # coners
        clockMatrix.clockMtx[0, 0].minuteArm.targetAngle = 180
        clockMatrix.clockMtx[0, 0].hourArm.targetAngle = 90
        clockMatrix.clockMtx[columns - 1, 0].minuteArm.targetAngle = 270
        clockMatrix.clockMtx[columns - 1, 0].hourArm.targetAngle = 180
        clockMatrix.clockMtx[0, rows - 1].minuteArm.targetAngle = 0
        clockMatrix.clockMtx[0, rows - 1].hourArm.targetAngle = 90
        clockMatrix.clockMtx[columns - 1, rows - 1].minuteArm.targetAngle = 270
        clockMatrix.clockMtx[columns - 1, rows - 1].hourArm.targetAngle = 0

    # clears border from clocks
    def clearBorder(self, clockMatrix):
        free = 400
        for i in range(columns):
            clockMatrix.clockMtx[i, 0].minuteArm.targetAngle = free
            clockMatrix.clockMtx[i, 0].hourArm.targetAngle = free
            clockMatrix.clockMtx[i, rows - 1].minuteArm.targetAngle = free
            clockMatrix.clockMtx[i, rows - 1].hourArm.targetAngle = free

        for j in range(rows):
            clockMatrix.clockMtx[0, j].minuteArm.targetAngle = free
            clockMatrix.clockMtx[0, j].hourArm.targetAngle = free
            clockMatrix.clockMtx[columns - 1, j].minuteArm.targetAngle = free
            clockMatrix.clockMtx[columns - 1, j].hourArm.targetAngle = free

        # coners
        clockMatrix.clockMtx[0, 0].minuteArm.targetAngle = free
        clockMatrix.clockMtx[0, 0].hourArm.targetAngle = free
        clockMatrix.clockMtx[columns - 1, 0].minuteArm.targetAngle = free
        clockMatrix.clockMtx[columns - 1, 0].hourArm.targetAngle = free
        clockMatrix.clockMtx[0, rows - 1].minuteArm.targetAngle = free
        clockMatrix.clockMtx[0, rows - 1].hourArm.targetAngle = free
        clockMatrix.clockMtx[columns - 1, rows - 1].minuteArm.targetAngle =\
            free
        clockMatrix.clockMtx[columns - 1, rows - 1].hourArm.targetAngle = free


pygame.init()  # initiate pygame
sysClock = pygame.time.Clock()     # set up system clock
sysClock.tick(60)   # set framerate

# setup screen
display = Screen("Clock Sim")

# setup and display background
background = Background(display)
background.draw(display)

# Clock class matrix initialisation
clockMatrix = Clock_Matrix(display)

# initialised pattern engine with initial arm angles and rotation rates
patternEngine = pattern_Engine(0, 180, 5, 5)

running = True
border = False
borderTrigger = False

time.sleep(1)

while running:

    for event in pygame.event.get():  # quit pygame
        if event.type == pygame.KEYDOWN:

            # 'r' to randomise rotation rates
            if event.key == pygame.K_r:
                patternEngine.randomRate()

            # 'b' to set border
            if event.key == pygame.K_b:
                borderTrigger = True

        if event.type == QUIT:
            pygame.quit()
            sys.exit()
            running = False

    # run select pattern engine functions
    patternEngine.cascade(clockMatrix)

    # border
    while borderTrigger:
        border = not border
        if border:
            patternEngine.border(clockMatrix)
        else:
            patternEngine.clearBorder(clockMatrix)

        borderTrigger = False

    # prints background to screen
    background.draw(display)

    # draw clocks to display
    clockMatrix.draw(display, patternEngine)

    # update display
    pygame.display.update()


pygame.quit()
