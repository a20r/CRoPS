#ifndef ABETARE__GENERATE_MAZE2D_HPP_
#define ABETARE__GENERATE_MAZE2D_HPP_

#include <vector>
#include <cstdio>

namespace Abetare
{       
    class GenerateMaze2D
    {
    public:
	GenerateMaze2D(void)
	{
	}
	
	
	~GenerateMaze2D(void)
	{
	}
	
	double m_min[2];
	double m_max[2];
	int    m_dims[2];

	void RandomizedKruskal(void);

	int  GetNrWalls(void) const
	{
	    return m_walls.size();
	}
	
	void GetWall(const int i, const double width, double min[2], double max[2]) const;

	void PrintWalls(const double width, FILE * const out) const;
 	
    protected:
	struct Wall
	{
	    int m_cids[2];	    
	};
	
	    
	std::vector<Wall> m_walls;	
    };
};

#endif










