
import obstacle
import configuration as con

def mapVal(x, in_min, in_max, out_min, out_max):
	"""
	Maps a value that is between in_min and in_max to
	a value between out_min and out_max
	@param in_min The minimum value that the input value could be
	@param in_max The maximum value that the input value could be
	@param out_min The minimum value that the output value could be
	@param out_max The maximum value that the output value could be
	@return A scaled value based on a given input
	"""
	return int(
		(x - in_min) * 
		(out_max - out_min) / 
		(in_max - in_min) + 
		out_min
	)

def mparse(filename):
	"""
	Parses a map file into a list of obstacles
	@param filename The file name of the map file
	@return A list of obstacles
	"""
	with open(filename, "r+") as f:
		numberOfPolys = int(f.readline())
		polyList = list()
		for _ in range(numberOfPolys):
			line = filter(
				lambda s: s != "\n", 
				f.readline().split(" ")
			)[1:]
			intList = map(
				lambda s: int(float(s)), 
				line
			)
			polyList += [
				[
					(
						mapVal(
							intList[2*i], 
							-29, 
							29, 
							0, 
							con.Configuration.xSize
						), 
						con.Configuration.ySize - mapVal(
							intList[2*i + 1], 
							-29, 
							29, 
							0, 
							con.Configuration.ySize
						)
					) for i in range(len(intList) / 2)
				]
			]

	return [
		obstacle.PolyObstacle(
			pList, 
			con.Configuration.screen
		) for pList in polyList
	]


