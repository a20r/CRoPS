#!/usr/bin/env python
from prm import PRMGenerator
import boid
import pygame
import pygame.color as color
import mapparser as mp


class Configuration:
    """
    Static class that holds important global variables
    """

    ## Dimensions of the screen
    dim = xSize, ySize = 1000, 600

    ## Number of sample points to use in the PRM
    numSamplePoints = 300

    ## Defines the radius of all goals
    goalRadius = 20

    ## Maximum speed of the boids
    boidSpeed = 30

    ## Number of neighbours the boids will influence
    ## a boid's heading
    numNeighbours = 3

    ## The screen used to draw the simluation
    screen = pygame.display.set_mode(dim)

    ## The list of colors (used for debugging purposes)
    colorList = map(
        lambda k: color.THECOLORS[k],
        color.THECOLORS.keys()
    )


class PolyFileConfiguration(Configuration):
    """
    Extends the Configuration class. This configuration gets the
    obstacles from .map files that have been created.
    """
    def initVars(
        self,
        startPoint,
        endPoint,
        flockSize,
        **kwargs
    ):
        """
        Parses the file to get the obstacle list. Creates a PRM generator to
        create a global map of the environment. Gets the list of intermediate
        goals.  Also, creates the list of boids used in the simulation
        @param startPoint The starting point for the boids
        @param endPoint The ending point for the boids
        @param flockSize The size of the flock (number of boids)
        @param filename The name of the file that contains the environment map
        """

        ## List of obstacles
        # parse static obstalces
        self.obstacleList = mp.mparse(kwargs.get("map_file", "maps/m1.map"))
        static_obstacles = [obst for obst in self.obstacleList]
        # parse dynamic obstalces
        dynamic_obstacles_fp = kwargs.get("dynamic_obstacles", None)
        if dynamic_obstacles_fp is not None:
            dyn_obstacles = mp.mparse(dynamic_obstacles_fp, static_obstacles)
            for obstacle in dyn_obstacles:
                self.obstacleList.append(obstacle)

        ## Starting point
        self.startPoint = startPoint

        ## Ending point
        self.endPoint = endPoint

        ## Object containing variables and mehtods for the global planner
        self.prmGen = PRMGenerator(
            startPoint,
            endPoint,
            self.obstacleList,
            Configuration.xSize,
            Configuration.ySize,
            Configuration.numSamplePoints,
            Configuration.screen
        )

        ## List of intermediate goals derived by the global planner
        self.goalList = self.prmGen.generate(Configuration.goalRadius)

        ## List of boids in the flock
        self.boidList = [
            boid.Boid(
                startPoint,
                endPoint,
                Configuration.boidSpeed,
                Configuration.xSize,
                Configuration.ySize,
                Configuration.numNeighbours,
                boid.guassianFunc,
                self.obstacleList,
                self.goalList,
                self.prmGen,
                Configuration.screen,
                Configuration.colorList[i]
            ) for i in range(flockSize)
        ]
