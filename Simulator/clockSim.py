from typing import Tuple
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
             '6': 180, '7': 210, '8': 240, '9': 270, '10': 300, '11': 330, '45': 45}

# digit position dictionary
pos = {'1': columns / 4, '2': columns / 2}

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
class Clock_Arm:

    def __init__(self, centre, length, angle):
        self.centre = centre
        self.length = length
        self.colour = BLACK
        self.lineWidth = 4
        self.endPoint = [0, 0]
        self.angle = angle
        self.targetAngle = 400.0
        self.rotationRate = 0.5

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
        self.minuteArm = Clock_Arm(self.centre, self.minuteArmLength, 0)
        self.hourArm = Clock_Arm(self.centre, self.hourArmLength, 0)
        self.colour = BLACK
        self.lineWidth = 1

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
class Pattern_Engine:

    def __init__(self, minute=0, hour=0, minRate=0.5, hourRate=0.5, defRates = 0.5):
        self.pattern = 0
        # self.minRate = minRate
        # self.hourRate = hourRate
        self.minAngle = minute
        self.hourAngle = hour
        # Matricies for storing calcualted angles
        self.minAngleMtx = np.empty([columns, rows], dtype=float)
        self.hourAngleMtx = np.empty([columns, rows], dtype=float)
        self.delay = 0
        self.defaultRotRates = defRates

        self.initialiseMatrix()

    # Initialises angle matricies in which angle data is stored for each arm
    def initialiseMatrix(self):
        for j in range(rows):
            for i in range(columns):
                self.minAngleMtx[i, j] = self.minAngle

        for j in range(rows):
            for i in range(columns):
                self.hourAngleMtx[i, j] = self.hourAngle

    # Set rotation rate for all clocks
    def setRate(self, clockMatrix, min, hour):
        for j in range(rows):
            for i in range(columns):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = min
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = hour

    # Set rotation rate to all clocks to default value
    def defaultRate(self, clockMatrix):
        for j in range(rows):
            for i in range(columns):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = self.defaultRotRates
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = self.defaultRotRates

    #rotates hour and minute in opposite direction - used as a squares effect
    def oppositeRate(self, clockMatrix, rate):
        for j in range(0, rows, 2):
            for i in range(0, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = rate
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = -rate
        for j in range(0, rows, 2):
            for i in range(1, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = -rate
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = rate

        for j in range(1, rows, 2):
            for i in range(0, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = rate
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = -rate
        for j in range(1, rows, 2):
            for i in range(1, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = -rate
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = rate

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

    def rotate(self, clockMatrix):
        complete = True

        # delay = 0.02
        # for j in range(rows):
            # if (self.delay > 0):
            #     for i in range(self.delay, 0, -1):
            #         if (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle,
            #                             clockMatrix.clockMtx[i, j].minuteArm.targetAngle)):

            #             self.minAngleMtx[i, j] = clockMatrix.clockMtx[i, j].minuteArm.targetAngle
            #             # continue
            #         else:
            #             if (self.delay > 0):
            #                 self.minAngleMtx[i, j] += self.minRate
            #             else:
            #                 self.minAngleMtx[i, j] += self.minRate

            #             self.minAngleMtx[i, j] = self.angleBoundCheck(self.minAngleMtx[i, j])

            #             complete = False

            #         if (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle,
            #            clockMatrix.clockMtx[i, j].hourArm.targetAngle)):

            #             self.hourAngleMtx[i, j] = clockMatrix.clockMtx[i, j].hourArm.targetAngle
            #             # continue
            #         else:
            #             if (self.delay > 0):
            #                 self.hourAngleMtx[i, j] += self.hourRate
            #             else:
            #                 self.hourAngleMtx[i, j] += self.hourRate

            #             self.hourAngleMtx[i, j] = self.angleBoundCheck(self.hourAngleMtx[i, j])

            #             complete = False
            # else:
        for j in range(rows):
            for i in range(columns):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle,
                                    clockMatrix.clockMtx[i, j].minuteArm.targetAngle)):

                    self.minAngleMtx[i, j] = clockMatrix.clockMtx[i, j].minuteArm.targetAngle
                    # continue
                else:
                    if (self.delay > 0):
                        # self.minAngleMtx[i, j] += self.minRate
                        self.minAngleMtx[i, j] += clockMatrix.clockMtx[i, j].minuteArm.rotationRate
                    else:
                        # self.minAngleMtx[i, j] += self.minRate
                        self.minAngleMtx[i, j] += clockMatrix.clockMtx[i, j].minuteArm.rotationRate

                    self.minAngleMtx[i, j] = self.angleBoundCheck(self.minAngleMtx[i, j])

                    complete = False

                if (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle,
                   clockMatrix.clockMtx[i, j].hourArm.targetAngle)):

                    self.hourAngleMtx[i, j] = clockMatrix.clockMtx[i, j].hourArm.targetAngle
                    # continue
                else:
                    if (self.delay > 0):
                        # self.hourAngleMtx[i, j] += self.hourRate
                        self.hourAngleMtx[i, j] += clockMatrix.clockMtx[i, j].hourArm.rotationRate
                    else:
                        self.hourAngleMtx[i, j] += self.hourRate
                        self.hourAngleMtx[i, j] += clockMatrix.clockMtx[i, j].hourArm.rotationRate

                    self.hourAngleMtx[i, j] = self.angleBoundCheck(self.hourAngleMtx[i, j])

                    complete = False

        return complete

    def rotateToAngle(self, arm, angle):
        if arm == "min":
            self.minAngle = angle
        else:
            self.hourAngle = angle

    def angleBoundCheck(self, angle):
        if angle > 360.0:
            angle -= 360.0
        elif angle < 0.0:
            angle += 360.0

        return angle

    # Randomises rotation rate - hour and minute independent
    def randomRate(self):
        self.minRate = random.randint(-10, 10) / 10.0
        self.hourRate = random.randint(-10, 10) / 10.0

    def relase(self, clockMatrix):
        free = 400
        for i in range(columns):
            for j in range(rows):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = free
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = free

    # determine difference in angle between current and target
    def angleCheck(self, currentAngle, targetAngle):
        if(abs(targetAngle - currentAngle) <= 1.5):
            return True
        else:
            return False

    # testing cascade pattern across matrix
    def cascade(self, clockMatrix, isolatedColumns):
        complete = True
        
        # minute arm control
        for i in range(isolatedColumns, columns):
            for j in range(rows):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle,
                                    clockMatrix.clockMtx[i, j].
                                    minuteArm.targetAngle)):
                    self.minAngleMtx[i, j] = clockMatrix.clockMtx[i, j].minuteArm.targetAngle
                    # clockMatrix.clockMtx[i, j].minuteArm.angle = clockMatrix.clockMtx[i, j].minuteArm.targetAngle
                    continue
                else:
                    # self.minAngleMtx[i, j] += self.minRate
                    self.minAngleMtx[i, j] += clockMatrix.clockMtx[i, j].minuteArm.rotationRate
                    self.minAngleMtx[i, j] = self.angleBoundCheck(self.minAngleMtx[i, j])

                    complete = False

            # hour arm control
        for i in range(isolatedColumns, columns):
            for j in range(rows):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle,
                   clockMatrix.clockMtx[i, j].hourArm.targetAngle)):
                    self.hourAngleMtx[i, j] = clockMatrix.clockMtx[i, j].hourArm.targetAngle
                    # clockMatrix.clockMtx[i, j].hourArm.angle = clockMatrix.clockMtx[i, j].hourArm.targetAngle
                    continue
                else:
                    # self.hourAngleMtx[i, j] += self.hourRate
                    self.hourAngleMtx[i, j] += clockMatrix.clockMtx[i, j].hourArm.rotationRate
                    self.hourAngleMtx[i, j] = self.angleBoundCheck(self.hourAngleMtx[i, j])

                    complete = False

        return complete

    # offset clock arms by a value
    def offset(self, clockMatrix, angle):

        # minute arm control
        for j in range(rows):
            for i in range(columns):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle,
                                    clockMatrix.clockMtx[i, j].
                                    minuteArm.targetAngle)):
                    self.minAngleMtx[i, j] = clockMatrix.clockMtx[i, j].minuteArm.targetAngle
                    continue
                else:
                    self.minAngleMtx[i, j] += self.minRate * (j * angle)

                    self.minAngleMtx[i, j] = self.angleBoundCheck(self.minAngleMtx[i, j])
                    # if self.minAngleMtx[i, j] > 360.0:
                    #     self.minAngleMtx[i, j] -= 360.0
                    # elif self.minAngleMtx[i, j] < 0.0:
                    #     self.minAngleMtx[i, j] += 360.0

        # hour arm control
        for j in range(rows):
            for i in range(columns):
                if (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle,
                   clockMatrix.clockMtx[i, j].hourArm.targetAngle)):
                    self.hourAngleMtx[i, j] =\
                        clockMatrix.clockMtx[i, j].hourArm.targetAngle
                    continue
                else:
                    self.hourAngleMtx[i, j] += self.hourRate * (j * angle)

                    self.hourAngleMtx[i, j] = self.angleBoundCheck(self.hourAngleMtx[i, j])
                    # if self.hourAngleMtx[i, j] > 360.0:
                    #     self.hourAngleMtx[i, j] -= 360.0
                    # elif self.hourAngleMtx[i, j] < 0.0:
                    #     self.hourAngleMtx[i, j] += 360.0

    # draws border on outermost clocks
    def border(self, clockMatrix):

        for i in range(columns):
            clockMatrix.clockMtx[i, 0].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[i, 0].hourArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[i, rows - 1].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[i, rows - 1].hourArm.targetAngle = timeAngle['3']

        for j in range(rows):
            clockMatrix.clockMtx[0, j].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0, j].hourArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[columns - 1, j].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[columns - 1, j].hourArm.targetAngle = timeAngle['6']

        # coners
        clockMatrix.clockMtx[0, 0].minuteArm.targetAngle = timeAngle['6']
        clockMatrix.clockMtx[0, 0].hourArm.targetAngle = timeAngle['3']
        clockMatrix.clockMtx[columns - 1, 0].minuteArm.targetAngle = timeAngle['9']
        clockMatrix.clockMtx[columns - 1, 0].hourArm.targetAngle = timeAngle['6']
        clockMatrix.clockMtx[0, rows - 1].minuteArm.targetAngle = timeAngle['12']
        clockMatrix.clockMtx[0, rows - 1].hourArm.targetAngle = timeAngle['3']
        clockMatrix.clockMtx[columns - 1, rows - 1].minuteArm.targetAngle = timeAngle['9']
        clockMatrix.clockMtx[columns - 1, rows - 1].hourArm.targetAngle = timeAngle['12']

    def showTime(self, clockMatrix):

        y_offset = 1

        # offset for digital representation position number
        numberPosition = {1: 1, 2: 4, 3: 8, 4: 11}

        def getTime():
            t = time.localtime()
            currentTime = time.strftime("%H:%M", t)
            return currentTime

        def border(clockMatrix):
            for i in range(columns):
                clockMatrix.clockMtx[i, 0].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[i, 0].hourArm.targetAngle = 45
                clockMatrix.clockMtx[i, rows - 1].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[i, rows - 1].hourArm.targetAngle = 45

            for j in range(rows):
                clockMatrix.clockMtx[0, j].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[0, j].hourArm.targetAngle = 45
                clockMatrix.clockMtx[columns - 1, j].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[columns - 1, j].hourArm.targetAngle = 45
                clockMatrix.clockMtx[7, j].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[7, j].hourArm.targetAngle = 45

        def zero(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def one(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['45']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def two(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def three(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def four(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['45']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['9']

        def five(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['9']

        def six(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['9']

        def seven(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['45']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['45']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['45']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def eight(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def nine(clockMatrix, x_offset):
            # left
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[0 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['3']
            clockMatrix.clockMtx[0 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[0 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # middle
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['3']

            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[1 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['12']
            clockMatrix.clockMtx[1 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['9']

            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[1 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['3']

            # right
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 0 + y_offset].hourArm.targetAngle = timeAngle['6']

            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 1 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 2 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 3 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].minuteArm.targetAngle = timeAngle['6']
            clockMatrix.clockMtx[2 + x_offset, 4 + y_offset].hourArm.targetAngle = timeAngle['12']

            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].minuteArm.targetAngle = timeAngle['9']
            clockMatrix.clockMtx[2 + x_offset, 5 + y_offset].hourArm.targetAngle = timeAngle['12']

        def setClocks(currentTime):
            match currentTime[0]:
                case '0':
                    zero(clockMatrix, numberPosition[1])
                case '1':
                    one(clockMatrix, numberPosition[1])
                case '2':
                    two(clockMatrix, numberPosition[1])
                case '3':
                    three(clockMatrix, numberPosition[1])
                case '4':
                    four(clockMatrix, numberPosition[1])
                case '5':
                    five(clockMatrix, numberPosition[1])
                case '6':
                    six(clockMatrix, numberPosition[1])
                case '7':
                    seven(clockMatrix, numberPosition[1])
                case '8':
                    eight(clockMatrix, numberPosition[1])
                case '9':
                    nine(clockMatrix, numberPosition[1])

            match currentTime[1]:
                case '0':
                    zero(clockMatrix, numberPosition[2])
                case '1':
                    one(clockMatrix, numberPosition[2])
                case '2':
                    two(clockMatrix, numberPosition[2])
                case '3':
                    three(clockMatrix, numberPosition[2])
                case '4':
                    four(clockMatrix, numberPosition[2])
                case '5':
                    five(clockMatrix, numberPosition[2])
                case '6':
                    six(clockMatrix, numberPosition[2])
                case '7':
                    seven(clockMatrix, numberPosition[2])
                case '8':
                    eight(clockMatrix, numberPosition[2])
                case '9':
                    nine(clockMatrix, numberPosition[2])

            match currentTime[3]:
                case '0':
                    zero(clockMatrix, numberPosition[3])
                case '1':
                    one(clockMatrix, numberPosition[3])
                case '2':
                    two(clockMatrix, numberPosition[3])
                case '3':
                    three(clockMatrix, numberPosition[3])
                case '4':
                    four(clockMatrix, numberPosition[3])
                case '5':
                    five(clockMatrix, numberPosition[3])
                case '6':
                    six(clockMatrix, numberPosition[3])
                case '7':
                    seven(clockMatrix, numberPosition[3])
                case '8':
                    eight(clockMatrix, numberPosition[3])
                case '9':
                    nine(clockMatrix, numberPosition[3])

            match currentTime[4]:
                case '0':
                    zero(clockMatrix, numberPosition[4])
                case '1':
                    one(clockMatrix, numberPosition[4])
                case '2':
                    two(clockMatrix, numberPosition[4])
                case '3':
                    three(clockMatrix, numberPosition[4])
                case '4':
                    four(clockMatrix, numberPosition[4])
                case '5':
                    five(clockMatrix, numberPosition[4])
                case '6':
                    six(clockMatrix, numberPosition[4])
                case '7':
                    seven(clockMatrix, numberPosition[4])
                case '8':
                    eight(clockMatrix, numberPosition[4])
                case '9':
                    nine(clockMatrix, numberPosition[4])

        currentTime = getTime()
        setClocks(currentTime)
        border(clockMatrix)

    # square pattern across clock matrix
    def squares(self, clockMatrix):
        for j in range(0, rows, 2):
            for i in range(0, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = timeAngle['3']
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = timeAngle['6']
        for j in range(0, rows, 2):
            for i in range(1, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = timeAngle['9']
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = timeAngle['6']

        for j in range(1, rows, 2):
            for i in range(0, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = timeAngle['12']
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = timeAngle['3']
        for j in range(1, rows, 2):
            for i in range(1, columns, 2):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = timeAngle['12']
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = timeAngle['9']

    def reset(self, clockMatrix):
        for i in range(columns):
            for j in range(rows):
                # clockMatrix.clockMtx[i, j].minuteArm.targetAngle = timeAngle['12']
                # clockMatrix.clockMtx[i, j].hourArm.targetAngle = timeAngle['6']
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = 225

class edgeTrigger(object):
    def __init__(self, callback):
        self.value = None
        self.callback = callback
        self.atVal = None

    def __call__(self, value):
        if value != self.value:
            self.atVal = self.callback(self.value, value)
        self.value = value

        return self.atVal

def callBack(oldVal, newVal):
    if(oldVal == 0 and newVal == 1):
        return True
    else:
        return False    

def squaresCallback(oldVal, newVal):
    if(oldVal == 0 and newVal == 1):
        return True
    else:
        return False    

def main():

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
    patternEngine = Pattern_Engine(0, 180, 0.5, 0.5)

    running = True
    atAngle = False

    border = False
    borderTrigger = False

    showTime = False
    clockTrigger = False

    squares = False
    squaresTrigger = False

    reset = False
    resetTrigger = False

    runOnce = False

    colno = 15
    delay = 0.1

    patternNum = 0
    showTimeTrigger = edgeTrigger(callBack)
    squaresTriggerF = edgeTrigger(squaresCallback)
    # patternEngine.offset(clockMatrix, 15)

    clock = pygame.time.Clock()

    while running:

        # print FPS to terminal
        # clock.tick()
        # print(clock.get_fps())

        currentTime = time.localtime()

        if(showTimeTrigger(currentTime.tm_sec)):
            showTime = True
            patternEngine.showTime(clockMatrix)

        if squaresTriggerF(squares):
            # squares = True
            patternEngine.squares(clockMatrix)
    

        # if(currentTime.tm_sec == 55):
        #     clockTrigger = True
        #     showTime = True
        # if(currentTime.tm_min % 2 != 0) and currentTime.tm_sec == 40:
        #     squares = True

        for event in pygame.event.get():  # quit pygame
            if event.type == pygame.KEYDOWN:

                # 'r' to randomise rotation rates
                if event.key == pygame.K_r:
                    patternEngine.randomRate()

                # 'b' to set border
                if event.key == pygame.K_b:
                    borderTrigger = True

                # 't' to show time
                if event.key == pygame.K_t:
                    clockTrigger = True

                # 's' for squares
                if event.key == pygame.K_s:
                    squaresTrigger = True

                # 'q' for reset
                if event.key == pygame.K_q:
                    resetTrigger = True

            if event.type == QUIT:
                pygame.quit()
                sys.exit()
                running = False

        # enables delay function
        # to be cleaned up  
        if (colno > 0):
            delay -= 1
            if (delay <= 0):
                colno -= 1
                delay = 0.35 * 60

        # run select pattern engine functions
        if(patternNum == 0):
            atAngle = patternEngine.cascade(clockMatrix, colno)
        elif(patternNum == 1):
            # atAngle = patternEngine.rotate(clockMatrix) <-- fix this up
            atAngle = patternEngine.cascade(clockMatrix, colno) # <- temp placeholder
        elif(patternNum == 2):
            squares = True
            patternNum = 0
            # atAngle = patternEngine.rotate(clockMatrix) <-- fix this up
            # atAngle = patternEngine.cascade(clockMatrix, colno) # <- temp placeholder
            

        if(atAngle):
            time.sleep(2)
            if(reset):
                reset = False
                patternEngine.relase(clockMatrix)
                colno = 15
                delay = 0.1
                patternEngine.defaultRate(clockMatrix)
            elif(showTime):
                showTime = False
                patternNum = random.randint(0, 2)
                patternEngine.reset(clockMatrix)
                reset = True
                patternEngine.defaultRate(clockMatrix)
            elif(squares):
                squares = False
                patternEngine.relase(clockMatrix)
                patternEngine.oppositeRate(clockMatrix, 0.5)

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

        # atAngle = False

    pygame.quit()


if __name__ == "__main__":

    main()
