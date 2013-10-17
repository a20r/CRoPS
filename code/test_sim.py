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
    fs = bs.FlockSim(
    	int(sys.argv[2]),
    	mapDict[sys.argv[1]]["startPoint"],
    	mapDict[sys.argv[1]]["endPoint"]
    )
    fs.animate()
