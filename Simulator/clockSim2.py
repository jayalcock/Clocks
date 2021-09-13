import pygame, sys
import math
import time
import random
import numpy as np
from pygame.locals import *

columns = 15    #number of clock columns
rows =  8     #number of clock rows


#define colours
WHITE=(255,255,255) 
BLUE=(0,0,255)
BLACK=(0,0,0)

#screen class to set up window and background
class Screen:
    def __init__ (self, screen_title):
        self.screen_title = screen_title
        self.height = 0
        self.width = 0
        self.screen = 0

        self.getDisplayInfo()
        self.setupScreen()

    #gets video info (resolution) from current active display 
    def getDisplayInfo(self):
        vidinfo = pygame.display.Info()
        self.height = vidinfo.current_h
        self.width = vidinfo.current_w     

    #sets up screen size and parameters that will be drawn on
    def setupScreen(self):
        self.screen = pygame.display.set_mode((self.width, self.height), 0, 32)
        pygame.display.set_caption(self.screen_title)
        pygame.mouse.set_visible(1)    

#Background class to draw blank background
class Background:
    def __init__(self, display):
        self.background = 0
        self.width = display.width
        self.height = display.height
        self.backgroundColour = WHITE

        self.setupBackground()

    #sets up background to be printed on
    def setupBackground(self):
        self.background = pygame.Surface([self.width, self.height]) 
        self.background = self.background.convert()
        self.background.fill(self.backgroundColour)

    #draws background to screen
    def draw(self, display):
        display.screen.blit(self.background, (0, 0))

#draw clock arms
class clockArm:
    def __init__(self, centre = (0, 0), length = 0, angle = 0, diameter = 0):
    # def __init__(self, centre, length, diameter):   
        self.centre = centre
        self.length = length
        self.angle = angle
        self.colour = BLACK
        self.endPoint = [0, 0]
        self.surfSize  = (width, height) = (int(diameter), int(diameter))
        self.surface = pygame.Surface(self.surfSize)
        self.surface = self.surface.convert_alpha()
        self.surface.fill((0,0,0,0))
        self.rect = self.surface.get_rect()
        self.endPtCalc()

    #calculates arm end point given angle and length, from centre of rect object
    def endPtCalc(self):
        ang = math.pi - math.radians(self.angle)
        self.endPoint = [(math.sin(ang) * self.length) + self.rect.centerx, (math.cos(ang) * self.length) + self.rect.centery]

    #draws arm objects on surface and centres rect object on centre of clock face object
    def draw(self):
        pygame.draw.line(self.surface, self.colour, self.rect.center, self.endPoint) #from centre point of surface
        self.rect.centerx = self.centre[0]
        self.rect.centery = self.centre[1]  
        display.screen.blit(self.surface, self.rect)

#Initiate clock class
class Clock:
    def __init__(self, centre, minuteArm, diameter):
        self.centre = centre
        # self.hourAngle = hour
        # self.minutAngle = minute
        self.hourArmLength = (diameter / 2) - 10
        self.minuteArmLength = (diameter / 2) - 1
        self.diameter = diameter
        self.minuteArm = clockArm(self.centre, self.minuteArmLength, self.diameter)
        self.colour = BLACK


#clock matrix to setup and control clock classes
class Clock_Matrix:
    def __init__(self, columns, rows, screen):
        self.columns = columns
        self.rows = rows
        self.height = screen.height
        self.width = screen.width
        self.diameter = 1
        self.clockMtx = np.empty([self.columns, self.rows], dtype= Clock) #populates matrix with empty objects
        self.minArmMtx = np.empty([self.columns, self.rows], dtype= clockArm)
        self.hourArmMtx = np.empty([self.columns, self.rows], dtype= clockArm)

        self.calculateDiameter()
        self.populateMatrix()

    #calculates correct clock face diameter to fit on screen whether limited by width or height
    def calculateDiameter(self):
        if(self.height/self.rows > self.width/self.columns):
            self.diameter = (self.width/self.columns)
        else:
            self.diameter = (self.height/self.rows) - 10

    #populates clock matrix with clock objects on equally spaced centre points
    def populateMatrix(self):
        for j in range (self.rows):
            for i in range(self.columns):
                centre_x = self.diameter * i + self.diameter/2
                centre_y = self.diameter * j + self.diameter/2
                self.clockMtx[i,j] = Clock((centre_x, centre_y), 0, self.diameter)

    #updates and draws minute arm
    def updateMin(self, angle):
        for j in range (rows):
            for i in range(columns):
                self.minArmMtx[i,j] = clockArm((self.clockMtx[i,j].centre[0], self.clockMtx[i,j].centre[1]), self.diameter/2, patternEngine.minAngleMtx[i,j], self.diameter)
                self.minArmMtx[i,j].surface.fill(WHITE)
                self.minArmMtx[i,j].draw()

    #updates and draws hour arm
    def updateHour(self, angle):
        for j in range (rows):
            for i in range(columns):
                self.hourArmMtx[i,j] = clockArm((self.clockMtx[i,j].centre[0], self.clockMtx[i,j].centre[1]), self.diameter/2-20, patternEngine.hourAngleMtx[i,j], self.diameter)
                self.hourArmMtx[i,j].draw()

    #draws clock frames on surface
    def draw(self, display, angle1, angle2):
        self.updateMin(angle1)
        self.updateHour(angle2)
        for j in range (self.rows):
            for i in range(self.columns):
                pygame.draw.circle(display.screen, self.clockMtx[i,j].colour, self.clockMtx[i,j].centre, self.diameter/2, 2)


#engine to drive clock arms
class pattern_Engine:
    def __init__(self, minute = 0, hour = 0, minRate = 10, hourRate = 10):
        self.pattern = 0
        self.minRate = minRate
        self.hourRate = hourRate
        self.minAngle = minute
        self.hourAngle = hour
        self.minAngleMtx = np.empty([columns, rows], dtype= float) #matrix for storing calcualted angles
        self.hourAngleMtx = np.empty([columns, rows], dtype= float) #matrix for storing calcualted angles

        self.initialiseMatrix()


    def initialiseMatrix(self):
        for j in range (rows):
            for i in range(columns):
                self.minAngleMtx[i,j] = self.minAngle   #initialise minute angle matrix

        for j in range (rows):
            for i in range(columns):
                self.hourAngleMtx[i,j] = self.hourAngle   #initialise hour angle matrix

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

    def randomRate(self):
        # for j in range (rows):
        #     for i in range(columns):
        #         self.angleMtx[i,j] = random.randint(1,10)
        self.minRate = random.randint(-10,10)
        self.hourRate = random.randint(-10,10)

    def test(self):
        for j in range (rows):
            for i in range(columns):
                self.minAngleMtx[i,j] += self.minRate

        for j in range (rows):
            for i in range(columns):
                self.hourAngleMtx[i,j] += self.hourRate

    def cascade(self):
        for j in range (rows):
            for i in range(columns):
                self.minAngleMtx[i,j] += self.minRate * ((j + 10) / 100)

        for j in range (rows):
            for i in range(columns):
                self.hourAngleMtx[i,j] += self.hourRate * ((j + 5) / 100)

pygame.init()  #initiate pygame
sysClock = pygame.time.Clock()     #set up system clock
sysClock.tick(60)   #set framerate 

#setup screen
display = Screen("Clock Sim")

#setup and display background
background = Background(display)
background.draw(display)

#Clock class matrix initialisation
clockMatrix = Clock_Matrix(columns, rows, display)

patternEngine = pattern_Engine(0, 180, 10, 10)

running = True

#draw clocks to display
# clockMatrix.draw(display, patternEngine.minAngle, patternEngine.hourAngle, 0)
# pygame.display.update() #update display

time.sleep(1)

while running:
    for event in pygame.event.get(): #quit pygame
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                patternEngine.minRate = random.randint(-10,10)
                patternEngine.hourRate = random.randint(-10,10)

            if event.key == pygame.K_r:
                patternEngine.randomRate()

        if event.type==QUIT: 
            pygame.quit()
            sys.exit()
            running = False


    # patternEngine.rotateAtRate("min", patternEngine.minRate, "cw")
    # patternEngine.rotateAtRate("hour", patternEngine.hourRate, "cw")
    

    patternEngine.cascade()
  
    # print(clockMatrix.minArmMtx[0,0].angle)

    #draw clocks to display
    clockMatrix.draw(display, patternEngine.minAngle, patternEngine.hourAngle)
    pygame.display.update() #update display

 

pygame.quit()