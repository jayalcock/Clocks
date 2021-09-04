import pygame, sys
import math
import time
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
        self.background = 0
        self.backgroundColour = WHITE

        self.getDisplayInfo()
        self.setupScreen()
        self.setupBackground()
        self.draw()

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

    #sets up background to be printed on
    def setupBackground(self):
        self.background = pygame.Surface([self.width, self.height]) 
        self.background = self.background.convert()
        self.background.fill(self.backgroundColour)

    #draws background to screen
    def draw(self):
        self.screen.blit(self.background, (0, 0))
        pygame.display.flip()



#Initiate clock class
class Clock:

    #Clock class init
    def __init__(self, centre, hour, minute, diameter):
        self.centre = centre
        self.hour = hour
        self.minute = minute
        self.diameter = diameter
        self.hourArmLength = (diameter / 2) - 10
        self.minuteArmLength = (diameter / 2) - 1
        self.colour = BLACK

#draw clock arms
class clockArm:
    def __init__(self, centre, length, angle, diameter):
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
        self.diameter = diameter

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


#clock matrix to setup and control clock classes
class Clock_Matrix:
    def __init__(self, columns, rows, screen):
        self.columns = columns
        self.rows = rows
        self.height = screen.height
        self.width = screen.width
        self.diameter = 1
        self.clockMtx = np.empty([self.columns, self.rows], dtype= Clock) #populates with empty matrix
        self.minArmMtx = np.empty([self.columns, self.rows], dtype= clockArm)
        self.hourArmMtx = np.empty([self.columns, self.rows], dtype= clockArm)

        self.calculateDiameter()
        self.populateMatrix()

    #calculates correct clock face diameter to fit on screen whether limited by width or height
    def calculateDiameter(self):
        #calculate clock diameter and make matrix fit into screen
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
                self.clockMtx[i,j] = Clock((centre_x, centre_y), 0, 0, self.diameter)

    #updates and draws minute arm
    def updateMin(self, angle, delay):
        for j in range (rows):
            for i in range(columns):
                self.minArmMtx[i,j] = clockArm((self.clockMtx[i,j].centre[0], self.clockMtx[i,j].centre[1]), self.diameter/2, angle * (i + 40) * delay, self.diameter)
                # self.minArmMtx[i,j] = clockArm((self.clockMtx[i,j].centre[0], self.clockMtx[i,j].centre[1]), self.diameter/2, angle, self.diameter)

                self.minArmMtx[i,j].surface.fill(WHITE)
                self.minArmMtx[i,j].draw()

    #updates and draws hour arm
    def updateHour(self, angle, delay):
        for j in range (rows):
            for i in range(columns):
                self.hourArmMtx[i,j] = clockArm((self.clockMtx[i,j].centre[0], self.clockMtx[i,j].centre[1]), self.diameter/2-20, angle * (i + 40) * delay, self.diameter)
                self.hourArmMtx[i,j].draw()

    #draws clock frames on surface
    def draw(self, display, angle1, angle2, delay):
        self.updateMin(angle1, delay)
        self.updateHour(angle2, delay)
        for j in range (self.rows):
            for i in range(self.columns):
                pygame.draw.circle(display.screen, self.clockMtx[i,j].colour, self.clockMtx[i,j].centre, self.diameter/2, 2)


#engine to drive clock arms
class patternEngine:
    def __init__(self, min = 0, hour = 0):
        self.pattern = 0
        self.minRate = 0
        self.hourRate = 90
        self.minAngle = min
        self.hourAngle = hour

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

    # def randomRate(self):



pygame.init()  #initiate pygame
sysClock = pygame.time.Clock()     #set up system clock
sysClock.tick(60)   #set framerate 

#setup screen
display = Screen("Clock Sim")

#Clock class matrix initialisation
clockMatrix = Clock_Matrix(columns, rows, display)

clockDriver = patternEngine(90, 90)

running = True

clockDriver.minRate = 10
clockDriver.hourRate = 10/12

tempDel = 0.001
# clockDriver.rotateToAngle("min", 0)
# clockDriver.rotateToAngle("hour", 90)

#draw clocks to display
# clockMatrix.draw(display, clockDriver.minAngle, clockDriver.hourAngle, 0)
pygame.display.update() #update display

time.sleep(1)

while running:
    for event in pygame.event.get(): #quite pygame
        if event.type==QUIT: 
            pygame.quit()
            sys.exit()
            running = False


    clockDriver.rotateAtRate("min", clockDriver.minRate, "cw")
    clockDriver.rotateAtRate("hour", clockDriver.hourRate, "cw")
  
    print(clockMatrix.minArmMtx[0,0].angle)

    tempDel = 0.001


    #draw clocks to display
    clockMatrix.draw(display, clockDriver.minAngle, clockDriver.hourAngle, tempDel)
    pygame.display.update() #update display

 

pygame.quit()