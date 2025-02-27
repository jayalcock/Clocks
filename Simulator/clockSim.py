# This file was developed to run a simulator for the clocks to develop and test
# algorithms easily and off the hardware model. 
# To run this file the following is needed:
#   * python 3.10 will be required - switch statements
#   * pygame 2.0.2 - graphics
#   * numpy - matricies
#
# Developed by - Jay Alcock - 2021

import pygame
import sys
import math
import time
import random
import numpy as np
from pygame.locals import *
# from pygame import gfxdraw # Required for anti-aliased lines

columns = 15    # number of clock columns
rows = 8     # number of clock rows

# define colours
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)

# clock number/angle dictionary
timeAngle = {'12': 0, '1': 30, '2': 60, '3': 90, '4': 120, '5': 150,
             '6': 180, '7': 210, '8': 240, '9': 270, '10': 300, '11': 330, '45': 45}

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
        aspectRatio = 16/9
        self.height = vidinfo.current_h
        self.width = vidinfo.current_h * aspectRatio

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
        pygame.draw.line(display.screen, self.colour, self.centre, self.endPoint, self.lineWidth)

        # anti-aliased line. for a line with thickness, a rectangle would have to be drawn 
        # pygame.gfxdraw.line(display.screen, int(self.centre[0]), int(self.centre[1]),
                        #  int(self.endPoint[0]), int(self.endPoint[1]), BLACK) 

# Initiate clock class
class Clock:

    def __init__(self, centre, diameter):
        self.centre = centre
        self.hourArmLength = (diameter / 2) - 10
        self.minuteArmLength = (diameter / 2) - 1
        self.diameter = diameter
        self.minuteArm = Clock_Arm(self.centre, self.minuteArmLength, 0.0)
        self.hourArm = Clock_Arm(self.centre, self.hourArmLength, 0.0)
        self.colour = BLACK
        self.lineWidth = 1
        self.active = True

    # draw clock frame instance
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
                self.clockMtx[i, j].minuteArm.angle = angleMtx.minAngleMtx[i, j]
                self.clockMtx[i, j].minuteArm.draw(display)

                # update hour arm and draw
                self.clockMtx[i, j].hourArm.angle = angleMtx.hourAngleMtx[i, j]
                self.clockMtx[i, j].hourArm.draw(display)

                # draw clock frames
                self.clockMtx[i, j].draw(display)


# Engine to drive clock arms
class Pattern_Engine:

    def __init__(self, minute=0, hour=0, defRates = 0.5):
        self.pattern = 0
        self.minAngle = minute
        self.hourAngle = hour
        # Matricies for storing calcualted angles
        self.minAngleMtx = np.empty([columns, rows], dtype=float)
        self.hourAngleMtx = np.empty([columns, rows], dtype=float)
        self.defaultRotRates = defRates
        self.freeRotate = 400

        self.cyclesRan = 0
        self.desiredCycles = 0
        self.incrementX = 0
        self.incrementY = 0

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

    #rotates hour and minute arms in opposite direction
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


    # checks to see if angle has been driven < 0 or > 360 degrees
    def angleBoundCheck(self, angle):
        if angle > 360.0:
            angle -= 360.0
        elif angle < 0.0:
            angle += 360.0

        return angle

    # sets random rotation rate for each arm 
    def randomRate(self, clockMatrix):
        for j in range(rows):
            for i in range(columns):
                # initialise 
                clockMatrix.clockMtx[i, j].minuteArm.rotationRate = 0
                clockMatrix.clockMtx[i, j].hourArm.rotationRate = 0

                # write random rate that is -0.7 < n < -0.3 and 0.3 > n > 0.7 
                while(clockMatrix.clockMtx[i, j].minuteArm.rotationRate < 0.3 and clockMatrix.clockMtx[i, j].minuteArm.rotationRate > -0.3):
                    clockMatrix.clockMtx[i, j].minuteArm.rotationRate = random.uniform(-0.7, 0.7)
                while(clockMatrix.clockMtx[i, j].hourArm.rotationRate < 0.3 and clockMatrix.clockMtx[i, j].hourArm.rotationRate > -0.3):
                    clockMatrix.clockMtx[i, j].hourArm.rotationRate = random.uniform(-0.7, 0.7) 

    # releases all arms from fixed angle, to freely rotate
    def relase(self, clockMatrix):
        for i in range(columns):
            for j in range(rows):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = self.freeRotate
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = self.freeRotate

    # determine difference in angle between current and target
    def angleCheck(self, currentAngle, targetAngle):
        if(abs(targetAngle - currentAngle) <= 1.5):
            return True
        else:
            return False

    # rotates clock arms with respect to different rotation patterns
    # Rotation list:
    # Rotation 0 = Default rotate
    # Rotation 1 = Cascade right to left
    # Rotation 2 = Cascade left to right
    # Rotation 3 = Cascade centre out
    def rotateClocks(self, clockMatrix, firstCall, rotationNum):
        complete = True 

        # all clocks rotate
        if(rotationNum == 0):
            if(firstCall):
                for i in range(columns):
                    for j in range(rows):
                        clockMatrix.clockMtx[i, j].active = True

        # cascade right to left
        if(rotationNum == 1):
            # turn all clocks off
            if(firstCall):
                for i in range(columns):
                    for j in range(rows):
                        clockMatrix.clockMtx[i, j].active = False
                        
                self.cyclesRan = 0
                self.desiredCycles = 20
                self.incrementX = columns - 1
 
            # iterate through columns until all set to active and after specified delay
            if(self.incrementX >= 0):
                if(self.cyclesRan == self.desiredCycles):
                    self.cyclesRan = 0
                    
                    # for i in range (self.incrementX):
                    for j in range (rows):
                        clockMatrix.clockMtx[self.incrementX, j].active = True

                    self.incrementX -= 1
        
        # cascade left to right
        if(rotationNum == 2):
            # turn all clocks off
            if(firstCall):
                for i in range(columns):
                    for j in range(rows):
                        clockMatrix.clockMtx[i, j].active = False

                self.cyclesRan = 0
                self.desiredCycles = 20
                self.incrementX = 0
 
            # iterate through columns until all set to active and after specified delay
            if(self.incrementX < columns):
                if(self.cyclesRan == self.desiredCycles):
                    self.cyclesRan = 0
        
                    # for i in range (self.incrementX):
                    for j in range (rows):
                        clockMatrix.clockMtx[self.incrementX, j].active = True

                    self.incrementX += 1

        # centre outward cascade
        if(rotationNum == 3):

            # find centre of clock matrix
            centreX = round(columns / 2) - 1
            centreY = round(rows / 2) - 1 

            # turn all clocks off except centre two
            if(firstCall):
                for i in range(columns):
                    for j in range(rows):
                        clockMatrix.clockMtx[i, j].active = False

                clockMatrix.clockMtx[centreX, centreY].active = True
                clockMatrix.clockMtx[centreX, centreY + 1].active = True
                self.cyclesRan = 0
                self.desiredCycles = 20
 
            # reset cycle counter
            if(self.cyclesRan == self.desiredCycles):
                self.cyclesRan = 0
                
                if(centreX + self.incrementX < columns):
                    self.incrementX += 1
                if(centreY + self.incrementY < rows - 1):
                    self.incrementY += 1

                for i in range (self.incrementX):
                    for j in range (self.incrementY):
                        clockMatrix.clockMtx[centreX + i, centreY - j].active = True
                        clockMatrix.clockMtx[centreX - i, centreY - j].active = True
                        clockMatrix.clockMtx[centreX + i, centreY + j + 1].active = True                    
                        clockMatrix.clockMtx[centreX - i, centreY + j + 1].active = True
            
        # minute arm control
        for i in range(columns):
            for j in range(rows):
                if (clockMatrix.clockMtx[i, j].active == False):
                    pass

                elif (self.angleCheck(clockMatrix.clockMtx[i, j].minuteArm.angle, clockMatrix.clockMtx[i, j].minuteArm.targetAngle)):
                    self.minAngleMtx[i, j] = clockMatrix.clockMtx[i, j].minuteArm.targetAngle

                else:

                    self.minAngleMtx[i, j] += clockMatrix.clockMtx[i, j].minuteArm.rotationRate
                    self.minAngleMtx[i, j] = self.angleBoundCheck(self.minAngleMtx[i, j])

                    complete = False

        # hour arm control
        for i in range(columns):
            for j in range(rows):
                if (clockMatrix.clockMtx[i, j].active == False):
                    pass

                elif (self.angleCheck(clockMatrix.clockMtx[i, j].hourArm.angle, clockMatrix.clockMtx[i, j].hourArm.targetAngle)):
                    self.hourAngleMtx[i, j] = clockMatrix.clockMtx[i, j].hourArm.targetAngle
                
                else:
                    self.hourAngleMtx[i, j] += clockMatrix.clockMtx[i, j].hourArm.rotationRate
                    self.hourAngleMtx[i, j] = self.angleBoundCheck(self.hourAngleMtx[i, j])

                    complete = False

        self.cyclesRan += 1

        return complete

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

    # aligns the clocks to show a digital representation of the time
    def showTime(self, clockMatrix):

        y_offset = 1
        unusedAngle = 45

        # offset for digital representation position number
        numberPosition = {1: 1, 2: 4, 3: 8, 4: 11}

        # get current time from system
        def getTime():
            t = time.localtime()
            currentTime = time.strftime("%H:%M", t)
            return currentTime

        #sets clocks not used in displaying time to fixed angle
        def border(clockMatrix):
            for i in range(columns):
                clockMatrix.clockMtx[i, 0].minuteArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[i, 0].hourArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[i, rows - 1].minuteArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[i, rows - 1].hourArm.targetAngle = unusedAngle

            for j in range(rows):
                clockMatrix.clockMtx[0, j].minuteArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[0, j].hourArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[columns - 1, j].minuteArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[columns - 1, j].hourArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[7, j].minuteArm.targetAngle = unusedAngle
                clockMatrix.clockMtx[7, j].hourArm.targetAngle = unusedAngle

        # number 0 representation 
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

        # number 1 representation 
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

        # number 2 representation 
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

        # number 3 representation
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

        # number 4 representation
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

        # number 5 representation
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

        # number 6 representation
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

        # number 7 representation
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

        # number 8 representation
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

        # number 9 representation
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

        # set each position in display to respective digit
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

    # moves clocks to "reset" configuration 
    def reset(self, clockMatrix):
        for i in range(columns):
            for j in range(rows):
                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = 45
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = 225

    # calculates angle to centre point of display and updates clock arms to point at it
    def pointToCentre(self, clockMatrix):
        centrePointX = clockMatrix.width / 2
        centrePointY = clockMatrix.height / 2
       
        for i in range(columns):
            for j in range(rows):
                angle = math.pi / 2 + math.atan2((centrePointY - clockMatrix.clockMtx[i, j].centre[1]), (centrePointX - clockMatrix.clockMtx[i, j].centre[0]))
                angle = math.degrees(angle)
                if(angle < 0):
                    angle += 360 

                clockMatrix.clockMtx[i, j].minuteArm.targetAngle = angle
                clockMatrix.clockMtx[i, j].hourArm.targetAngle = angle

# edge trigger for debouncing keystroke or show time trigger
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

# callback for 0 to 1 transition
def binaryEdgeCallback(oldVal, newVal):
    if(oldVal == 0 and newVal == 1):
        return True
    else:
        return False 
           

def main():

    frameRate = 100

    pygame.init()  # initiate pygame
    sysClock = pygame.time.Clock()     # set up system clock

    # setup screen
    display = Screen("Clock Sim")

    # setup and display background
    background = Background(display)
    background.draw(display)

    # Clock class matrix initialisation
    clockMatrix = Clock_Matrix(display)

    # initialised pattern engine with initial arm angles and rotation rates
    patternEngine = Pattern_Engine(0, 180, 0.5)

    # initialise trigger booleans
    running = True
    atAngle = False
    showTime = False
    squares = False
    reset = False
    pointToCentre = False

    # demo = 2
    rotationNum = 1
    patternNum = None
    firstCall = True

    # Callback trigger initialisation 
    showTimeTrigger = edgeTrigger(binaryEdgeCallback)
    showTimeTrigger2 = edgeTrigger(binaryEdgeCallback)
    squaresTrigger = edgeTrigger(binaryEdgeCallback)
    pointToCentreTrigger = edgeTrigger(binaryEdgeCallback)
    resetTrigger = edgeTrigger(binaryEdgeCallback)

    while running:

        sysClock.tick(frameRate) # fix framerate 
        # print(sysClock.get_fps()) # print framerate to terminal

        currentTime = time.localtime() # get current time from  system

        if(showTimeTrigger(currentTime.tm_sec)):
            patternNum = 0

        if(showTimeTrigger2(showTime)):
            patternEngine.showTime(clockMatrix)

        if squaresTrigger(squares):
            patternEngine.squares(clockMatrix)

        if pointToCentreTrigger(pointToCentre):
            patternEngine.pointToCentre(clockMatrix)

        if resetTrigger(reset):
            patternEngine.reset(clockMatrix)
    
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
                    patternNum = 0

                # 's' for squares
                if event.key == pygame.K_s:
                    patternNum = 2

                # 'q' for reset
                if event.key == pygame.K_q:
                    patternNum = 1

                # '1' for centre rotate cascade
                if event.key == pygame.K_1:
                    patternNum = 3

                # '2' for centre rotate swirl
                if event.key == pygame.K_2:
                    patternNum = 4

            if event.type == QUIT:
                running = False
                pygame.quit()
                sys.exit()
    
        # Pattern list:
        # Pattern 0 = Shows the current time in digital form
        # Pattern 1 = Moves all clocks to reset angle
        # Pattern 2 = Squares
        # Pattern 3 = All clocks point to centre - centre out cascade rotation
        # Pattern 4 = All clocks point to centre - swirl rotation

        # Rotation list:
        # Rotation 0 = Default rotate
        # Rotation 1 = Cascade right to left
        # Rotation 2 = Cascade left to right
        # Rotation 3 = Cascade centre out

        if(patternNum == 0): # Show time
            if(atAngle):
                time.sleep(2)
                atAngle = False
                showTime = False
                patternNum = random.randint(1, 4)
                # patternNum = demo
                # demo += 1
                patternEngine.relase(clockMatrix)
                patternEngine.defaultRate(clockMatrix)

            else:
                showTime = True

        elif(patternNum == 1): # Reset angle
            if(atAngle):
                atAngle = False
                reset = False
                patternNum = None
                rotationNum = random.randint(1, 2)
                firstCall = True
                patternEngine.relase(clockMatrix)
                patternEngine.defaultRate(clockMatrix)
                
            else:
                reset = True

        elif(patternNum == 2): # Squares
            if(atAngle):
                time.sleep(2)
                atAngle = False
                squares = False
                patternNum = None
                rotationNum = 0
                patternEngine.relase(clockMatrix)
                patternEngine.oppositeRate(clockMatrix, 0.5)

            else:
                squares = True

        elif(patternNum == 3): # Point to centre - cascade
            if(atAngle):
                time.sleep(2)
                atAngle = False
                pointToCentre = False
                patternNum = None
                firstCall = True
                rotationNum = 3
                patternEngine.relase(clockMatrix)
                patternEngine.oppositeRate(clockMatrix, 0.5)
                
            else:
                pointToCentre = True

        elif(patternNum == 4): # Point to centre - swirl
            if(atAngle):
                time.sleep(2)
                atAngle = False
                pointToCentre = False
                patternNum = None
                rotationNum = 1
                patternEngine.relase(clockMatrix)
                patternEngine.defaultRate(clockMatrix)
            else:
                pointToCentre = True

        # update clock angles
        atAngle = patternEngine.rotateClocks(clockMatrix, firstCall, rotationNum)

        firstCall = False

        # prints background to screen
        background.draw(display)

        # draw clocks to display
        clockMatrix.draw(display, patternEngine)

        # update display
        pygame.display.update()

    pygame.quit()

if __name__ == "__main__":

    main()
