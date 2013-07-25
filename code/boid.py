# -*- coding: utf-8 -*-

__author__ = "Alex Wallar <aw204@st-andrews.ac.uk>"

import pygame
import random
import math as np
import prm
import dijkstra
import goal

def guassianFunc(dX, dAvg = 10, dSigma = 1):
    """
    Gamma function used to give a probability distribution of the flock 
    in order to choose an appropriate neighbour.
    @param dX Distance from the boid to the prospective neighbour
    @param dAvg Dynamic variable used to define the average prospective Distance
    @param dSigma Standard deviation of the average distances
    """
    return np.exp(-0.5*((dX - dAvg)/dSigma)**2)/(2.5066282746310002*dSigma)

class Boid:
    """
    Class which represents one boid
    """
    def __init__(self, _sPos, _ePos, _speed, _xSize, _ySize, _neighborSize, _gammaFunc, _obstacleList, _goalList, _prmGen, _screen, _color):
        """
        Initializes all of the variables given as input to the constructor used by the boid
        @param self The object pointer
        @param _sPos The starting position of the boid (some noise added when initializing the flock)
        @param _ePos The ending position of the flock
        @param _xSize The size of the x axis of the pygame screen
        @param _neighbourSize The number of neighbours that will influence the boid's heading
        @param _obstacleList List of obstacles generated by mapparser
        @param _goalList List of goals used by the boid
        @param _prmGen Object that stores all of the data about the global planner
        @param _screen PyGame screen
        @param _color Unique color used for debugging purposes
        @return An instance of a boid
        """

        ## Function used to choose a neighbour
        self.gammaFunc = lambda x: _gammaFunc(x)

        ## Class which holds the details about the global path planner
        self.prmGen = _prmGen

        ## PyGame screen
        self.screen = _screen

        ## Radius of the boid
        self.radius = 4

        ## Initial random heading
        self.heading = (random.randint(int(-_speed), int(_speed)), random.randint(int(-_speed), int(_speed)))

        ## Dimensions of the screen
        self.dim = self.xSize, self.ySize = _xSize, _ySize

        ## Number of neighbours that will influence the boid
        self.neighborSize = _neighborSize

        ## Unique color used to distinguish the boid (only used in debugging and visualization)
        self.color = _color

        ## Maximum speed of the boid
        self.speed = _speed

        ## List of obstacles that were parsed by mapparser 
        self.obstacleList = _obstacleList

        ## Goals used by the boid
        self.goalList = _goalList

        ## Used to store what goal the boid is currently looking at
        self.goalCounter = 0

        ## Initializes the current goal
        self.goal = self.goalList[0]

        ## Defines if the boid is stuck
        self.stuck = False

        ## Starting position of the boid
        self.sPos = _sPos

        ## Position of the boids
        self.ePos = _ePos

        ## Sets the position of the boid
        self.position = [random.randint(_sPos[0] - 10, _sPos[0] + 10), random.randint(_sPos[1] - 10, _sPos[1] + 10)]

        ## Used to tell if the boid is stuck or not
        self.positionBuffer = [(self.position[0] + 5*i, self.position[1] + 5*i) for i in range(20)]

        self.initFunctionParameters()

    def sumDivide(self, lt, s):
        """
        Special sort of reduce that sums components in a list of vectors
        and divides each final component with a certain number
        @param self The object pointer
        @param lt List of vectors that will be summed over and divided
        @param s Number that will divide each component by at the end
        """
        _sumDivide = lambda lt, s: map(lambda p: p / s, reduce(lambda p1, p2: (p1[0] + p2[0], p1[1] + p2[1]), lt))
        try:
            return _sumDivide(lt, s)
        except (TypeError, ZeroDivisionError):
            return (0, 0)

    def norm(self, p1, p2):
        """
        Gets the distance between two points
        @param p1, p2 points whose distance will be returned
        @return The Euclidean distance between p1 and p2
        """
        return np.sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2))

    def getVar(self, searchList, ind):
        """
        Gets multiple variables from a list with one call
        @param searchList The list that the values will be taken from
        @param ind the indicies that will be queried
        @return A list of values from search list
        """
        return [searchList[i] for i in ind]

    def obstacleFunc(self, beta, b, o):
        """
        Defines the potential between a boid and an obstacle
        @param beta Constant used to increase the weight of the function
        @param b The boid that is being comapred
        @param o The obstacle that is being compared
        @return A value representing the potential between b and o
        """
        return b.radius*o.getRadius()*beta/pow(self.norm(b.position, o.getPoint(b.position)) - o.getRadius() - b.radius , 2)

    def mag(self, vec):
        """
        Gets the magnitude of a vector
        @param vec A vector represented as a list
        @return The magnitude of the vector
        """
        return np.sqrt(vec[0]**2 + vec[1]**2)

    def sigmoidFunc(self, alpha, beta, delta, const, b_r, g_r, b_pos, g_pos):
        """
        Defines a sigmoidal curve used for goal attractself._sumDivide   = lambda lt, s: map(lambda p: p / s, reduce(lambda p1, p2: (p1[0] + p2[0], p1[1] + p2[1]), lt))ion and for boid repulsion
        @param alpha, beta, delta, const Constants that are used to modify the shape of the curve
        @param b_r, b_pos The radius and position of the boid
        @param g_r, g_pos The radius and position of a goal / boid
        """
        return b_r*g_r*alpha / (1 + beta*np.exp(delta*self.norm(b_pos, g_pos))) + const

    def inGoal(self, p):
        """
        Checks if a piont is in the current goal
        @param p The point that is going to be checked
        @return A boolean value representing if the point is in the current goal
        """
        return all(map(lambda q, r: abs(q - r) < self.goal.radius, p, self.goal.position))

    def inWorld(self, p):
        """
        Checks if a point is in the world
        @param p The point that is going to be checked
        @return A boolean value representing if the point is in the world
        """
        return p[0] > -100 and p[0] < self.xSize + 100 and p[1] > -100 and p[1] < self.ySize + 100

    def pointAllowed(self, p):
        """
        Checks if a point is inside or collides with any of the obstacles
        @param p The point that will be checked
        """
        return all(map(lambda ob: ob.pointAllowed(self, p), self.obstacleList))

    def initFunctionParameters(self):
        # function parameters

        ## The radius of influence used when filtering
        ## the number of obstacles it needs to check
        self.obInfluenceR   = 50

        ## The radius of influence used when filtering
        ## the number of boids it needs to check
        self.bInfluenceR    = 20 + 2 * self.radius

        ## Priori constant for obstacle repulsion (increasing it
        ## gives more priority to the repulsive obstacle field)
        self.obBeta         = 300

        ## Scales the value returned by the sigmoid function 
        ## for goal attraction
        self.gAlpha         = 0.5

        ## Helps scale the value returned by the sigmoid function
        ## for goal attraction
        self.gBeta          = 50

        ## Constant that is used in the sigmoidal curve for goal
        ## attraction
        self.gDelta         = -0.7

        ## Priori constant for goal attraction (increasing it
        ## gives more priority to the attractive goal field)
        self.gConst         = 60

        ## Scales the value returned by the sigmoid function for 
        ## boid repulsion 
        self.bAlpha         = 1

        ## Helps scale the value returned by the sigmoid function 
        # for boid repulsion
        self.bBeta          = 60

        ## Constant that is used in the sigmoid curve for boid 
        ## repulsion
        self.bDelta         = -0.7

        ## Priroi constant for boid repulsion (increasing it
        ## gives more priority to the repulsive boid field)
        self.bConst         = 40

        ## Amount of movement in the position buffer needs to be less
        ## that this value for a boid to be considered stuck
        self.stuckConst     = 30

        ## The average distance from a neighbour when a boid is stuck.
        ## Used to pick closer neighbours when stuck to help get out of 
        ## the situation
        self.stuckDAvg      = 5

        ## The standard deviation for a neighbour probability distribution
        ## Helps boid pick closer neighbours when it is stuck
        self.stuckDSigma    = 0.5

        ## The average distance from a neighbour when a boid is not stuck
        self.nStuckDAvg     = 10

        ## The standard deviation in a neighbour distance distribution when the 
        ## boid is not stuck
        self.nStuckDSigma   = 1

        ## Maximum x random walk
        self.randomWalkX    = 7

        ## Maximum y random walk
        self.randomWalkY    = 7

        ## Stores the number of times a random walk has occurred
        self.randWalkCount  = 0

        ## Weights how much the previous heading affects the new heading
        self.headWeightList = [3, 1]

    def setBoidList(self, _boidList):
        """
        Setter method used to set the list of boids
        @param _boidList The list of boids in the flock
        """
        self.boidList = _boidList

    def updatePositionBuffer(self):
        """
        Updates the position buffer
        @return The displacement of a boid over a certain number of frames
        """
        self.positionBuffer += [self.position]
        del self.positionBuffer[0]
        xDiff = self.positionBuffer[-1][0] - self.positionBuffer[0][0]
        yDiff = self.positionBuffer[-1][1] - self.positionBuffer[0][1]
        return abs(xDiff) + abs(yDiff)

    def findMax(self, searchThrough, counter):
        """
        Gets the n maximum values from a list
        @param searchThrough The list that the maximums will be extracted from
        @param counter The number of values to be extracted
        @return A list of the counter maximum values from searchThrough
        """
        maxList    = list()
        searchList = list()
        searchList.extend(searchThrough)
        for _ in range(counter):
            maxPos   = [i for i,j in enumerate(searchList) if j == max(searchList)][0]
            maxList += [maxPos]
            searchList[maxPos] = 0
        return maxList

    def reduceWeightValues(self, wList, *vList):
        """
        Works in cohesion with sumDivide. Weights values in a list and divides
        by the sum of those weights
        @param wList List of Weights
        @param *vList Values that will be weighted
        @return An average vector that represents the average heading due to the potential fields
        """
        return self.sumDivide([(w*v1, w*v2) for (v1, v2), w in zip(vList, wList)], sum(wList))

    def getDirectionVector(self, vector):
        """
        Gets a scaled direction vector from an unscaled vector
        @param vector Vector to be scaled
        @return A vector whose maximum magnitude is less than the specified maximum speed
        """
        return map(lambda comp: comp * self.speed / self.mag(vector), vector)

    def getObstacleVectorList(self):
        """
        Gets the potential vectors to a boid due to the repulsive obstacle field
        @return A list of scaled vectors that will be used to determine
        the influence of obstacles on the heading. Also returns the sum of the 
        potential values
        """
        influenceOb = filter(lambda o: self.norm(o.getPoint(self.position), self.position) < self.obInfluenceR, self.obstacleList)
        magnitudeList = map(lambda o: self.obstacleFunc(self.obBeta if not self.stuck else self.obBeta/10, self, o), influenceOb)
        vectorList    = map(self.getDirectionVector, map(lambda o: [self.position[0] - o.getPoint(self.position)[0], \
            self.position[1] - o.getPoint(self.position)[1]], influenceOb))
        return map(lambda c, p: [c * p[0], c * p[1]], magnitudeList, vectorList), sum(magnitudeList)

    def getGoalVector(self):
        """
        Gets the potential vectors to a boid due to the attractive goal field
        @return A list of scaled vectors that will be used to determine the 
        influence of the goal on the heading. Also returns the sum of the potential
        values
        """
        magnitude = self.sigmoidFunc(self.gAlpha, self.gBeta, self.gDelta, self.gConst, self.radius, self.goal.radius, self.position, self.goal.position)
        gVector   = self.getDirectionVector([self.goal.position[0] - self.position[0], self.goal.position[1] - self.position[1]])
        return [magnitude * gVector[0], magnitude * gVector[1]], magnitude

    def getBoidVectorList(self):
        """
        Gets the potential vectors to a boid due to the repulsive boid field
        @return A list of scaled vectors that will be used to determine the 
        influence of the boids on the heading. ALso returns the sum of the potential
        """
        influenceBo   = filter(lambda b: self.norm(self.position, b.position) < self.bInfluenceR and self.norm(self.position, b.position) > 0 and not self.inGoal(b.position), self.boidList)
        magnitudeList = map(lambda bo: self.sigmoidFunc(self.bAlpha, self.bBeta, self.bDelta, self.bConst, self.radius, self.radius, self.position, bo.position), influenceBo)
        vectorList    = map(self.getDirectionVector, map(lambda bo: [self.position[0] - bo.position[0], \
            self.position[1] - bo.position[1]], influenceBo))
        return map(lambda c, p: [c * p[0], c * p[1]], magnitudeList, vectorList), sum(magnitudeList)

    # determines the neighbors and their velocities
    def getNeighborVectorList(self):
        """
        Gets the heading vectors of the neighbours
        @return A list of scaled vectors that represent the neighbour headings.
        Also returns the indicies in the boid list in which the neighbours are stored
        """
        distList         = [self.norm(b.position, self.position) for b in self.boidList]
        flockProbability = map(lambda prob, b: 0 if b.stuck or b.goal.position == self.goal.position else prob, map(self.gammaFunc, distList), self.boidList)
        nIndexes         = self.findMax(flockProbability, self.neighborSize)
        return self.getVar(map(lambda b: b.heading, self.boidList), nIndexes), nIndexes

    def setNewGoal(self):
        """
        Sets the new goal
        """
        self.goalCounter += 1
        self.goal         = self.goalList[self.goalCounter]
        self.gConst       = self.sigmoidFunc(self.gAlpha, self.gBeta, self.gDelta, self.gConst, self.radius, self.goal.radius, self.position, self.goal.position)

    def determineRandomWalk(self):
        """
        Increments the random walk counter, changes the current goal if necessary.
        @return New random walk vectors where the maximum component value is determeined
        by the randomWalk fields
        """
        self.randWalkCount  += 1
        if self.randWalkCount == 3:
            if self.goalCounter > 0:
                self.goalCounter -= 1
        #if self.randWalkCount == 6:
            #if self.goalCounter < len(self.goalList) - 2:
                #self.goalCounter += 2

        self.goal = self.goalList[self.goalCounter]

        return (random.randint(-self.randomWalkX, self.randomWalkX), random.randint(-self.randomWalkY, self.randomWalkY))

    def update(self):
        """
        Updates the boid's heading and position due to the potential fields
        """
        if not self.inGoal(self.position):
            neighborVectorList, nIndexes = self.getNeighborVectorList()
            bVectorList, bMagSum         = self.getBoidVectorList()
            obstacleVectorList, obMagSum = self.getObstacleVectorList()
            gVector, gMagSum             = self.getGoalVector()

            obVecSum = self.sumDivide(obstacleVectorList, obMagSum)
            boVecSum = self.sumDivide(bVectorList, bMagSum)
            goVecSum = self.sumDivide([gVector], gMagSum)

            #self.stuck = False

            if not self.stuck:
                self.randWalkCount = 0
                self.gammaFunc = lambda dX: guassianFunc(dX, dAvg = self.nStuckDAvg, dSigma = self.nStuckDSigma)
                neVecSum       = self.sumDivide(neighborVectorList, self.neighborSize)
                self.compWeightList = [50 * self.neighborSize, bMagSum, gMagSum, obMagSum]
                nHeading       = self.reduceWeightValues(self.compWeightList, neVecSum, boVecSum, goVecSum, obVecSum)
                self.heading   = self.reduceWeightValues(self.headWeightList, self.heading, nHeading)
                newPos         = (self.position[0] + self.heading[0], self.position[1] + self.heading[1])
                if self.inWorld(newPos) and self.pointAllowed(newPos):
                    self.position = newPos
            else:
                self.gammaFunc = lambda dX: guassianFunc(dX, dAvg = self.stuckDAvg, dSigma = self.stuckDSigma)
                rX, rY         = self.determineRandomWalk()
                neVecSum       = self.sumDivide(neighborVectorList, self.neighborSize)
                self.compWeightList = [5 * self.neighborSize, bMagSum, gMagSum, obMagSum]
                nHeading       = self.reduceWeightValues(self.compWeightList, neVecSum, boVecSum, goVecSum, obVecSum)
                self.heading   = self.reduceWeightValues(map(lambda v: -1 * v, self.headWeightList), self.heading, nHeading)
                newPos         = (self.position[0] + self.heading[0] + rX, self.position[1] + self.heading[1] + rY)
                if self.inWorld(newPos) and self.pointAllowed(newPos):
                    self.position = newPos

        # if the boid is not at the last goal
        elif self.goalCounter < len(self.goalList) - 1:
            self.setNewGoal()
        self.stuck = self.updatePositionBuffer() < self.stuckConst

    def draw(self):
        """
        Draws the boid as a pygame circle in the pygame screen 
        """
        pygame.draw.circle(self.screen, (0, 255, 100) if self.stuck else (0,0,255), map(int, self.position), self.radius)
