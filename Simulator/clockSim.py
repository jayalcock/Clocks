from graphics import *
from screeninfo import get_monitors
import time
import math
import numpy as np




class Clock:
	def __init__(self, centre, hour, minute):
		self.centre = centre
		self.hour = hour
		self.minute = minute


def makeMatrix(columns, rows):

	P1 = Point(10, 10)

	#create empty matrix
	arr = np.empty([columns, rows], dtype= Clock)

	#populate matrix with clock objects
	for j in range (rows):
		for i in range(columns):
			arr[i,j] = Clock(P1, i, j)	
			#print(arr[i,j].hour, arr[i,j].min)

	return arr


def drawClocks(win, numWide, numHigh, clockMatrix):

	for m in get_monitors():
		print(m.height)

	radius = (m.height-110)/numHigh/2
	hourArmLength = radius - 10
	minuteArmLength = radius - 1

	for j in range(numHigh):
		for i in range(numWide):
			xPT = radius*i*2+radius
			yPt = radius*j*2+radius
			clockMatrix[i,j].centre = Point(xPT, yPt)
			
			clockFrame = Circle(clockMatrix[i,j].centre, radius)
			clockFrame.draw(win)
			
			clockMatrix[i,j].hour = Line(clockMatrix[i,j].centre, Point(xPT+hourArmLength, yPt))
			clockMatrix[i,j].hour.draw(win)

			minuteArmP2 = Point(xPT, yPt + minuteArmLength)
			clockMatrix[i,j].minute = Line(clockMatrix[i,j].centre, minuteArmP2)
			clockMatrix[i,j].minute.draw(win)

	return minuteArmLength

	# for j in range(numHigh):
	# 	for i in range(numWide):
	# 		xPT = radius*i*2+radius
	# 		yPt = radius*j*2+radius
	# 		centrePt = Point(xPT, yPt)
			
	# 		clockFrame = Circle(centrePt, radius)
	# 		clockFrame.draw(win)
			
	# 		hourArm = Line(centrePt, Point(xPT+hourArmLength, yPt))
	# 		hourArm.draw(win)

	# 		minArmP2 = Point(xPT, yPt + minArmLength)
	# 		minArm = Line(centrePt, minArmP2)
	# 		minArm.draw(win)



def moveMin(window, clockMatrix, reps, delay, length):

	radsPerStep = math.radians(360/60/10) 

	P1 = clockMatrix[0,0].minute.getP1()
	P2 = clockMatrix[0,0].minute.getP2()
	angle = math.acos((P2.y - P1.y)/length) #get inital arm angle

	x = 0
	y = 0
	
	for i in range(reps):
		# for x in range(6):
			# for y in range(3):
		while y < 4:
			while x < 6:
				P1 = clockMatrix[x,y].minute.getP1()
				P2 = clockMatrix[x,y].minute.getP2()
				clockMatrix[x,y].minute.undraw()
				angle += radsPerStep
				if angle > math.radians(360):
					angle = 0
				P2.x = P1.x + (math.sin(angle))*length
				P2.y = P1.y + (math.cos(angle))*length
				clockMatrix[x,y].minute = Line(P1, P2)
				clockMatrix[x,y].minute.draw(window)
				x += 1
			x = 0
			y += 1
		# time.sleep(delay)
		y = 0
			

	



	# radsPerStep = math.radians(360/60) 
	# P1 = minArm.getP1()
	# P2 = minArm.getP2()
	# angle = math.acos((P2.y - P1.y)/length) #get inital arm angle

	# for i in range(reps):
	# 	minArm.undraw()
	# 	angle += radsPerStep
	# 	if angle > math.radians(360):
	# 		angle = 0
	# 	P2.x = P1.x + (math.sin(angle))*length
	# 	P2.y = P1.y + (math.cos(angle))*length
	# 	minArm = Line(P1, P2)
	# 	minArm.draw(window)
	# 	time.sleep(delay)


def main():

	#m = get_monitors()
	for m in get_monitors():
 #   print(str(m))
		print(m.width)

	#clock matrix size
	numWide = 6
	numHigh = 4
	

	win = GraphWin('clock window', width = (m.width-100), height = (m.height-100)) # create a window
	win.setCoords(0, 0, m.width-100, m.height-100) # set the coordinates of the window; bottom left is (0, 0) and top right is (10, 10)
	#frame = Rectangle(Point(1, 1), Point(m.width - 110, m.height - 110)) # create a rectangle from (1, 1) to (9, 9)
	#frame.draw(win) # draw it to the window



	i = 0
	j = 0

	#create clock matrix
	clockMatrix = makeMatrix(numWide, numHigh)
	print(clockMatrix[1,2].hour, clockMatrix[1,2].minute)

	#draw clock array 
	minArmLength = drawClocks(win, numWide, numHigh, clockMatrix)	


	moveMin(win, clockMatrix, 100, 0.1, minArmLength)



	win.getMouse() # pause before closing


main()	