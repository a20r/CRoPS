
import boidsimulation as bs

# generates stats for the project
def generateStats(mapFile, iterations, startPoint, endPoint):
	"""
	Generates the statistics and saves them in a well known
	location
	@param mapFile The file that stores information about the environment
	@param iterations the number of experiments to run per number of bots 
	@param startPoint, endPoint Defines the starting and ending points
	of the flock
	"""
	getFileFormat = lambda nBots, it: "simdata/" + mapFile.split("/")[-1].split(".")[0] + "_simulation_data_" + str(nBots) + "_" + str(it)
	getPRMFormat  = lambda nBots, it: "simdata/" + mapFile.split("/")[-1].split(".")[0] + "_prm_data_" + str(nBots) + "_" + str(it)
	for nBots in [50]:
		for i in range(iterations):
			reload(bs)
			print mapFile, " : ", nBots, ":", i
			with open(getFileFormat(nBots, i), "w") as dFile:
				fSim = bs.FlockSim(nBots, startPoint, endPoint, _mapFile = mapFile, _dataFile = dFile)
				fSim.render()
			with open(getPRMFormat(nBots, i), "w") as pFile:
				pFile.write(str(map(lambda g: tuple(map(int, g)), fSim.config.prmGen.subGoalPositionList)) + "\n")
				pFile.write(str(map(lambda g: tuple(map(int, g.position)), fSim.config.goalList)))

## A list of dictionaries used to store the map files, starting and ending points of the boids
mapList = [{"filename": "maps/scene2.map", "startPoint": (494, 213), "endPoint": (404, 20)},
		   {"filename": "maps/scene3.map", "startPoint": (356, 42), "endPoint": (852, 450)},
		   {"filename": "maps/scene1.map", "startPoint": (50, 600), "endPoint": (980, 30)}]

if __name__ == "__main__":
	testList = [mapList[1], mapList[2]]
	for tData in testList:
		generateStats(tData["filename"], 1, tData["startPoint"], tData["endPoint"])
