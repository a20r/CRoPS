#include "Utils/GenerateMaze2D.hpp"
#include "Utils/DisjointSet.hpp"
#include "Utils/Misc.hpp"

namespace Abetare
{       
    void GenerateMaze2D::RandomizedKruskal(void)
    {
	DisjointSet dset;
	Wall        wall;
	std::vector<DisjointSet::Elem*> cells;
	
	for(int x = 0; x < m_dims[0]; ++x)
	    for(int y = 0; y < m_dims[1]; ++y)
	    {
		if(x > 0)
		{
		    wall.m_cids[0] = y * m_dims[0] + x;
		    wall.m_cids[1] = wall.m_cids[0] - 1;
		    m_walls.push_back(wall);
		}
		if(y > 0)
		{
		    wall.m_cids[0] = y * m_dims[0] + x;
		    wall.m_cids[1] = wall.m_cids[0] - m_dims[0];
		    m_walls.push_back(wall);
		}
		cells.push_back(dset.Make());
	    }
	PermuteItems<Wall>(&m_walls, m_walls.size());
	
	int nkeep = 0;
	for(int i = 0; i < m_walls.size(); ++i)
	{
	    wall = m_walls[i];
	    if(dset.Same(cells[wall.m_cids[0]], cells[wall.m_cids[1]]) == false)
		dset.Join(cells[wall.m_cids[0]], cells[wall.m_cids[1]]);
	    else
		m_walls[nkeep++] = wall;
	}
	m_walls.resize(nkeep);

	for(int i = 0; i < cells.size(); ++i)
	    delete cells[i];
    }

    void GenerateMaze2D::GetWall(const int i, const double width, double min[2], double max[2]) const
    {
	const double units[] = {(m_max[0] - m_min[0]) / m_dims[0],
				(m_max[1] - m_min[1]) / m_dims[1]};
	Wall         wall    = m_walls[i];
	const int    coords[]= {wall.m_cids[0] % m_dims[0], wall.m_cids[0] / m_dims[0]};
	const int    which   = (wall.m_cids[0] == (wall.m_cids[1] + 1)) ? 0 : 1;
	
	min[which]     = m_min[which]     + coords[which] * units[which] - 0.5 * width;
	min[1 - which] = m_min[1 - which] + coords[1 - which] * units[1 - which] + 0.05 * units[1 - which];
	max[which]     = min[which] + width;
	max[1 - which] = min[1 - which] + 0.9 * units[1 - which];
    }

    void GenerateMaze2D::PrintWalls(const double width, FILE * const out) const
    {
	double min[2], max[2];

	const int n = (int) (0.8 * m_walls.size());
	
	fprintf(out, "%d\n", n);
	for(int i = 0; i < n; ++i)
	{
	    GetWall(i, width, min, max);
	    fprintf(out, "4 %f %f %f %f %f %f %f %f\n",
		    min[0], min[1],
		    max[0], min[1],
		    max[0], max[1],
		    min[0], max[1]);
	}
	
    }
    

}










