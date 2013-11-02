
import boidsimulation as bs

# generates stats for the project
def generateStats(
	mapFile, iterations,
	startPoint, endPoint):
	"""
	Generates the statistics and saves them in a well known
	location
	@param mapFile The file that stores information about the environment
	@param iterations the number of experiments to run per number of bots
	@param startPoint, endPoint Defines the starting and ending points
	of the flock
	"""
	getFileFormat = lambda nBots, nObs, it: str(
		"stats/" +
		mapFile.split("/")[-1].split(".")[0] +
		"_" + str(nBots) + "_" + str(nObs) + "_" + str(it)
	)

	for nBots in range(10, 55, 5):
	    for nObs in range(10, 40, 10):
	        for i in range(iterations):
	            #reload(bs)
	            print mapFile, " : ", nBots, ":", nObs, ":", i
	            dFile = getFileFormat(nBots, nObs, i)
	            fSim = bs.FlockSim(
	                nBots,
	                startPoint,
	                endPoint,
	                map_file = mapFile,
	                obstacle_file = None,
	                auto_gen_obst = True,
	                auto_gen_number = nObs,
	                data_file = dFile
	            )
	            fSim.render()

## A list of dictionaries used to store the map files,
#starting and ending points of the boids
mapList = [
	{
		"filename": "maps/scene2.map",
		"startPoint": (494, 213),
		"endPoint": (404, 20)
	},
	{
		"filename": "maps/scene3.map",
		"startPoint": (356, 42),
		"endPoint": (852, 450)
	},
	{
		"filename": "maps/scene1.map",
		"startPoint": (50, 600),
		"endPoint": (980, 30)
	}
]

if __name__ == "__main__":
	testList = [mapList[2], mapList[2]]
	for tData in testList:
		generateStats(
			tData["filename"],
			10,
			tData["startPoint"],
			tData["endPoint"]
		)


