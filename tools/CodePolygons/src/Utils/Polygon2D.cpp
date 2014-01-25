#include "Utils/Polygon2D.hpp"
#include "External/ShewchukTriangle.hpp"
#include "Utils/GDraw.hpp"

namespace Abetare
{
    int Polygon2D::GetNrTriangles(void)
    {
	return GetTriangleIndices()->size() / 3;
    }

    const std::vector<int>* Polygon2D::GetTriangleIndices(void)
    {
	if(m_triRecompute)
	{
	    m_triRecompute = false;
	    m_triIndices.clear();		
	    TriangulatePolygonWithNoHoles2D(false, -1, -1,
					    m_vertices.size() / 2, &m_vertices[0],
					    NULL, &m_triIndices, NULL);
	}	
	return &m_triIndices;
    }

    void Polygon2D::GetTriangleVertices(const int i, double tri[6])
    {
	GetTriangleIndices();
	tri[0] = m_vertices[2 * m_triIndices[3 * i    ]    ];
	tri[1] = m_vertices[2 * m_triIndices[3 * i    ] + 1];
	tri[2] = m_vertices[2 * m_triIndices[3 * i + 1]    ];
	tri[3] = m_vertices[2 * m_triIndices[3 * i + 1] + 1];
	tri[4] = m_vertices[2 * m_triIndices[3 * i + 2]    ];
	tri[5] = m_vertices[2 * m_triIndices[3 * i + 2] + 1];	
    }
    

    const std::vector<double>* Polygon2D::GetTriangleAreas(void)
    {
	if(m_areasRecompute)
	{
	    m_areasRecompute = false;
	    m_triAreas.clear();
	    const int n = GetNrTriangles();
	    double tri[6];
	    double area = 0;
	    
	    m_triAreas.resize(n);
	    m_area = 0;
	    for(int i = 0; i < n; ++i)
	    {
		GetTriangleVertices(i, tri);
		m_triAreas[i] = fabs(SignedAreaPolygon2D(3, tri));
		m_area += m_triAreas[i];

		if(m_triAreas[i] > area)
		{
		    m_triLargestArea = i;
		    area = m_triAreas[i];
		}		
	    }
	}

	return &m_triAreas;
    }
    
    double Polygon2D::GetArea(void)
    {
	GetTriangleAreas();
	return m_area;
    }
    

    void Polygon2D::GetSomePointInside(double p[2])
    {
/*	GetTriangleIndices();
	
	const int t = 3 * RandomUniformInteger(0, m_triIndices.size()/3  - 1);
	SampleRandomPointInsideTriangle2D(&m_vertices[2 * m_triIndices[t]],
					  &m_vertices[2 * m_triIndices[t + 1]],
					  &m_vertices[2 * m_triIndices[t + 2]], p);
*/
	double tri[6];	
	GetTriangleVertices(0, tri);
	const double a1 = 0.35;
	const double a2 = 0.45;
	
		
	p[0] = tri[0] + a1 * (tri[2] - tri[0]) + a2 * (tri[4] - tri[0]);
	p[1] = tri[1] + a1 * (tri[3] - tri[1]) + a2 * (tri[5] - tri[1]);
    }
    
    void Polygon2D::SampleRandomPointInside(double p[2])
    {
	const int t = 3 * SelectTriangleBasedOnArea();
	SampleRandomPointInsideTriangle2D(&m_vertices[2 * m_triIndices[t]],
					  &m_vertices[2 * m_triIndices[t + 1]],
					  &m_vertices[2 * m_triIndices[t + 2]], p);
    }

    int Polygon2D::SelectTriangleBasedOnArea(void)
    {
	const int    n = GetNrTriangles();
	const double r = RandomUniformReal(0, GetArea());
	double       w = 0;
	
	GetTriangleAreas();
	for(int i = 0; i < n; ++i)
	{
	    w += m_triAreas[i];
	    if(w >= r)
		return i;
	}
	return n - 1;
    }
    

    void Polygon2D::OccupiedGridCells(const Grid * const       grid, 
				      std::vector<int> * const cellsInside,
				      std::vector<int> * const cellsIntersect)
    {		
	const double *bbox = GetBoundingBox();
	const int     n    = m_vertices.size() / 2;
	const double *poly = &m_vertices[0];
	const bool    checkIntersection = false;
	
	int coord_min[2];
	int coord_max[2];
	int coords[2];
	double min[2], max[2];
	
	cellsInside->clear();
	cellsIntersect->clear();		
	grid->GetCoords(&bbox[0], coord_min);
	grid->GetCoords(&bbox[2], coord_max);       
	for(int x = coord_min[0]; x <= coord_max[0]; ++x)
	{
	    coords[0] = x;		    
	    for(int y = coord_min[1]; y <= coord_max[1]; ++y)
	    {
		coords[1] = y;
		
		grid->GetCellFromCoords(coords, min, max);
		double box[8];
		AABoxAsPolygon2D(min, max, box);
		
		if(IntersectPolygons2D(4, box, n, poly))
		    cellsIntersect->push_back(grid->GetCellIdFromCoords(coords));
		else if(IsPolygonInsidePolygon2D(4, box, n, poly, checkIntersection))
		    cellsInside->push_back(grid->GetCellIdFromCoords(coords));
		else if(IsPolygonInsideConvexPolygon2D(n, poly, 4, box))
		    cellsIntersect->push_back(grid->GetCellIdFromCoords(coords));
			
	    }
	    
	}
    }   

    bool Polygon2D::CollisionPolygon(Polygon2D * const poly)
    {
	const double *bbox1 = GetBoundingBox();
	const double *bbox2 = poly->GetBoundingBox();
	
	if(CollisionAABoxes2D(bbox1, &bbox1[2],
			      bbox2, &bbox2[2]) == false)
	    return false;
	
	
	const bool c1 = IsConvex();
	const bool c2 = poly->IsConvex();
	
	if(c1 && c2)
	    return CollisionConvexPolygons2D(m_vertices.size() / 2, &m_vertices[0],
					     poly->m_vertices.size() / 2, &(poly->m_vertices[0]));
	else if(c1 && !c2)
	    return CollisionPolygonConvexPolygon2D(poly->m_vertices.size() / 2, &(poly->m_vertices[0]),
						   m_vertices.size() / 2, &m_vertices[0]);
	else if(!c1 && c2)
	    return CollisionPolygonConvexPolygon2D(m_vertices.size() / 2, &m_vertices[0],
						   poly->m_vertices.size() / 2, &(poly->m_vertices[0]));
	else
	    return CollisionPolygons2D(m_vertices.size() / 2, &m_vertices[0],
				       poly->m_vertices.size() / 2, &(poly->m_vertices[0]));
    }
    
    
    void Polygon2D::Draw(void)
    {
	if(IsConvex())
	    GDrawConvexPolygon2D(m_vertices.size() / 2, &m_vertices[0]);
	else
	{
	    GetTriangleIndices();
	    GDrawPolygon2D(m_vertices.size() / 2, &m_vertices[0],
			   m_triIndices.size() / 3, &m_triIndices[0]);
	}
	

/*	GDrawWireframe(true);
	const double *bbox = GetBoundingBox();
	
	GDrawAABox2D(bbox, &bbox[2]);
	GDrawWireframe(false);
*/	
    }
}	



