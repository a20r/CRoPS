#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = "Alex Wallar <aw204@st-andrews.ac.uk>"

import pygame
import math as np
import sys


class PolyObstacle:
    """
    Object that represents the an obstacle represented
    by a series of points (in the node list) which make up a
    set of lines. These lines represent the exterior of an obstacle
    """
    def __init__(self, _nodes, _screen, **kwargs):
        """
        Creates a PolyObstacle instance and initializes certain global
        variables
        @param _nodes A list of nodes used to represent the vertices
        of the polygon
        @param _screen The PyGame screen that is used to draw the obstacle
        """

        ## A list of nodes used to represent the vertices
        self.nodes = _nodes

        ## A dictionary of colors defined in pygame
        self.colors = pygame.color.THECOLORS

        ## The PyGame screen that is used to draw the obstacle
        self.screen = _screen

        ## Defines wether the obstacle is dynamic or not
        self.dynamic = kwargs.get("dynamic", False)

        ## Velocity of the obstacle
        self.velocity = [1, 0]

        ## The displacement of the obstacle
        self.displacement = 0

        ## Max displacement allowed
        self.max_displacement = 100

        self.estimatePoly()

    def norm(self, p1, p2):
        """
        Gets the Eulidean distance between p1 and p2
        @param p1, p2 Points in space
        @return The distance between p1 and p2
        """
        return np.sqrt(
            pow(p1[0] - p2[0], 2) +
            pow(p1[1] - p2[1], 2)
        )

    def estimatePoly(self):
        """
        Tries to estimate the polygon as a circle (very useful for environments
        with many obstacles i.e. a random field of obstacles)
        """

        ## The average point in the polygon. Represents the
        ## center of the enclosing circle
        self.avgPoint = map(
            lambda p: p / len(self.nodes),
            reduce(
                lambda p1, p2: (
                    p1[0] + p2[0],
                    p1[1] + p2[1]
                ),
                self.nodes
            )
        )

        ## The maximum distance from any vertex and the average point
        self.maxDist = max(
            [
                self.norm(
                    p,
                    self.avgPoint
                ) for p in self.nodes
            ]
        )

    def detectCollision(self, pStart, pEnd):
        """
        Detects a if there is a collision with the obstacle and
        the line <pStart, pEnd>
        @param pStart The starting point of the line
        @param pEnd The ending point of the line
        @return A boolean value representing if a collision occurred
        """
        interCross = lambda p1, p2, q1, q2: (
            (
                (p1[0] - p2[0]) *
                (q1[1] - p1[1]) -
                (p1[1] - p2[1]) *
                (q1[0] - p1[0])
            ) *
            (
                (p1[0] - p2[0]) *
                (q2[1] - p1[1]) -
                (p1[1] - p2[1]) *
                (q2[0] - p1[0])
            )
        ) < 0
        interCheck = lambda p1, p2, q1, q2: (
            max(p1[0], p2[0]) >= min(q1[0], q2[0]) and
            max(q1[0], q2[0]) >= min(p1[0], p2[0]) and
            max(p1[1], p2[1]) >= min(q1[1], q2[1]) and
            max(q1[1], q2[1]) >= min(p1[1], p2[1])
        )
        vecList = [[self.nodes[0], self.nodes[-1]]]
        for k in range(len(self.nodes) - 1):
            vecList += [
                [
                    self.nodes[k],
                    self.nodes[k+1]
                ]
            ]
        return any(
            map(
                lambda p: (
                    interCross(
                        p[0],
                        p[1],
                        pStart,
                        pEnd
                    ) and
                    interCheck(
                        p[0],
                        p[1],
                        pStart,
                        pEnd
                    )
                ),
                vecList
            )
        )

    def getClosestPoint(self, a, b, p):
        """
        Gets the closest point on line <a, b> to point p
        @param a The starting point on the line
        @param b The ending point of the line
        @param p The point in which the closest distance will be checked
        @return The closest point on line <a, b> to point p
        """
        if (
            p[0] >= max(a[0], b[0]) or \
            p[0] <= min(a[0], b[0])) and \
            (
                p[1] >= max(a[1], b[1]) or
                p[1] <= min(a[1], b[1]
            )
        ):
            if self.norm(a, p) < self.norm(b, p):
                return a
            else:
                return b
        else:
            a_to_p = [
                float(p[0] - a[0]),
                float(p[1] - a[1])
            ]
            a_to_b = [
                float(b[0] - a[0]),
                float(b[1] - a[1])
            ]
            atb2 = a_to_b[0] ** 2 + a_to_b[1] ** 2
            atp_dot_atb = a_to_p[0] * a_to_b[0] + a_to_p[1] * a_to_b[1]
            t = float(atp_dot_atb) / float(atb2)
            return (
                float(a[0]) + a_to_b[0] * t,
                float(a[1]) + a_to_b[1] * t
            )

    def rayintersectseg(self, p, edge):
        """
        Determines if a ray from point p intersects with an edge, edge.
        Used to determine if a point p in inside the polygon
        @param p The point to be checked
        @param edge The edge that will be checked
        @return True if a ray from point p intersects with edge and false
        otherwise
        """
        _eps = 0.00001
        _huge = sys.float_info.max
        _tiny = sys.float_info.min
        a, b = edge
        if a[1] > b[1]:
            a, b = b, a
        if p[1] == a[1] or p[1] == b[1]:
            p = (p[0], p[1] + _eps)

        intersect = False

        if (p[1] > b[1] or p[1] < a[1]) or (p[0] > max(a[0], b[0])):
            return False

        if p[0] < min(a[0], b[0]):
            intersect = True
        else:
            if abs(a[0] - b[0]) > _tiny:
                m_red = (b[1] - a[1]) / float(b[0] - a[0])
            else:
                m_red = _huge
            if abs(a[0] - p[0]) > _tiny:
                m_blue = (p[1] - a[1]) / float(p[0] - a[0])
            else:
                m_blue = _huge
            intersect = m_blue >= m_red
        return intersect

    def _odd(self, x):
        """
        Determines if an integer, x, is odd
        @param x The integer to be checked
        @return True if x is odd, false otherwise
        """
        return x % 2 == 1

    def pointInPoly(self, p):
        """
        Determines if a point p is inside the polygon represented
        by this PolyObstacle object. It does this by checking the
        number ray intersections that occur is odd or even. If the number
        is odd, the point is inside the polygon, otherwise it is not.
        @param p The point to be checked
        @return True if the point is in the polygon and false otherwise
        """
        vecList = [[self.nodes[0], self.nodes[-1]]]
        for k in range(len(self.nodes) - 1):
            vecList += [[self.nodes[k], self.nodes[k+1]]]
        return self._odd(
            sum(
                self.rayintersectseg(p, edge) for edge in vecList
            )
        )

    def pointAllowed(self, b, p):
        """
        Checks if a point is allowed, meaning no collisions occur
        @param b The boid object that will be checked
        @param p The point that will be checked
        @return True if allowed, false otherwise
        """
        return (
            (
                self.norm(
                    self.getPoint(p),
                    p
                ) > b.radius
            ) and
            (
                not self.pointInPoly(p)
            )
        )

    def getPoint(self, p):
        """
        Gets the closest point from the polygon to p
        @param p The point to be checked
        @return The closest point that lies on the polygon exterior
        to p
        """
        vecList = list()  # [[self.nodes[0],self.nodes[-1]]]
        for k in range(-1, len(self.nodes) - 1):
            vecList += [[self.nodes[k], self.nodes[k+1]]]
        cpList = map(
            lambda v: self.getClosestPoint(v[0], v[1], p),
            vecList
        )
        dList = map(
            lambda pv: self.norm(p, pv),
            cpList
        )
        return [
            cpList[i] for i, j in enumerate(dList) if j == min(dList)
        ][0]

    def getRadius(self):
        """
        Gets the 'radius' of the checking point. Only used for
        conformity with circle obstacles that have not been included
        in this repository
        @return 1
        """
        return 1

    def translate(self):
        """
        Translate obstacle
        """
        for i in range(self.nodes.__len__()):
            # tuples are immutable hence convert
            coord = list(self.nodes[i])
            orig_coord = list(self.nodes[i])

            # translate x, y
            coord[0] += self.velocity[0]
            coord[1] += self.velocity[1]

            # convert back to tuple and replace old node
            self.nodes[i] = tuple(coord)

            # record displacement
            self.displacement += self.norm(
                orig_coord,
                coord
            )

        # reverse direction if max displacement reached
        if self.displacement >= self.max_displacement:
            self.velocity[0] = self.velocity[0] * -1
            self.velocity[1] = self.velocity[1] * -1
            self.displacement = 0

    def draw(self):
        """
        Draws the polygon on the PyGame screen
        """
        if self.dynamic:
            self.translate()

        pygame.draw.polygon(
            self.screen,
            self.colors["red"],
            self.nodes
        )
