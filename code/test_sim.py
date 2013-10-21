import boidsimulation as bs
import sys

"""
Main module to run for testing purposes
"""

if __name__ == "__main__":
    mapDict = {
    	"maps/scene2.map": {
    		"startPoint": (494, 213),
    		"endPoint": (404, 20)
    	},
    	"maps/scene3.map": {
    		"startPoint": (356, 42),
    		"endPoint": (852, 450)
    	},
    	"maps/scene1.map": {
    		"startPoint": (50, 50), #(50, 600)
    		"endPoint": (980, 30)
    	}
    }

    if len(sys.argv) <= 2:
        print("Not enough commandline args!")
        print("Need at least 2 commands!")
    else:
        flockSize = int(sys.argv[2])
        startPoint = mapDict[sys.argv[1]]["startPoint"]
        endPoint = mapDict[sys.argv[1]]["endPoint"]
        mapFilePath = sys.argv[1]

        if len(sys.argv) >= 4:
            obstacleFilePath = sys.argv[3]
        else:
            obstacleFilePath = None

        fs = bs.FlockSim(
            flockSize,
            startPoint,
            endPoint,
            map_file = mapFilePath,
            obstacle_file = obstacleFilePath
        )
        fs.animate()

