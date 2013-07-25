#include "Utils/GManager.hpp"
#include "Utils/Geometry.hpp"
#include "Utils/Algebra2D.hpp"
#include <vector>
#include <cstdio>
#include <cstring>

namespace Abetare
{
    class GEditor2D : public GManager
    {
    public:
	GEditor2D(const char fname[]) : GManager()
	{
	    Defaults2D();
	    
	    MENU_EDITOR_BEGIN_NEW_POLYGON           = ++m_nrMenuItems;
	    MENU_EDITOR_END_NEW_POLYGON             = ++m_nrMenuItems;
	    MENU_EDITOR_ADD_VERTEX_POLYGON          = ++m_nrMenuItems;
	    MENU_EDITOR_COPY_POLYGON                = ++m_nrMenuItems;
	    MENU_EDITOR_REMOVE_POLYGON              = ++m_nrMenuItems;
	    MENU_EDITOR_REMOVE_ALL_POLYGONS         = ++m_nrMenuItems;
	    MENU_EDITOR_REMOVE_ALL_INVALID_POLYGONS = ++m_nrMenuItems;
	    MENU_EDITOR_REMOVE_VERTEX_POLYGON       = ++m_nrMenuItems;
	    MENU_EDITOR_TRANSLATE_POLYGON           = ++m_nrMenuItems;
	    MENU_EDITOR_TRANSLATE_VERTEX_POLYGON    = ++m_nrMenuItems;
	    MENU_EDITOR_ROTATE_POLYGON              = ++m_nrMenuItems;
	    MENU_EDITOR_TRANSLATE_ROTATION_CENTER   = ++m_nrMenuItems;
	    MENU_EDITOR_FILE_READ                   = ++m_nrMenuItems;
	    MENU_EDITOR_FILE_SAVE                   = ++m_nrMenuItems;
	    MENU_EDITOR_FILE_NAME                   = ++m_nrMenuItems;
	    MENU_EDITOR_TOL                         = ++m_nrMenuItems;

	    GDrawSetParam(GDRAW_MINX, -30);
	    GDrawSetParam(GDRAW_MINY, -30);
	    GDrawSetParam(GDRAW_MAXX,  30);
	    GDrawSetParam(GDRAW_MAXY,  30);
	    	    
	    m_center[0] = m_center[1] = 0.0;
	    m_option    = -1;
	    m_edit      = false;
	    m_transform = false;
	    m_selVertex = -1;
	    m_tol       = (GDrawGetParam(GDRAW_MAXX) - GDrawGetParam(GDRAW_MINX)) / 200;

	    if(fname)
	    {
		strcpy(m_fname, fname);
		HandleEventOnMenu(MENU_EDITOR_FILE_READ);
	    }	    
	    else
		m_fname[0] = '\0';	    

//	    m_gCamera.RotateAroundAxisAtCenter(M_PI/2, 1, 0, 0);	    
	    
	}

	virtual ~GEditor2D(void)
	{
	}
	
	virtual void MenuOptions(void)
	{
	    GManager::MenuOptions();
	    
	    m_idMenuEditor = CreateMenu();
	    AddMenuEntry("Begin polygon", MENU_EDITOR_BEGIN_NEW_POLYGON);
	    AddMenuEntry("End polygon", MENU_EDITOR_END_NEW_POLYGON);
	    AddMenuEntry("Add polygon vertex", MENU_EDITOR_ADD_VERTEX_POLYGON);
	    AddMenuEntry("Copy polygon", MENU_EDITOR_COPY_POLYGON);
	    AddMenuEntry("Remove polygon", MENU_EDITOR_REMOVE_POLYGON);
	    AddMenuEntry("Remove all polygons", MENU_EDITOR_REMOVE_ALL_POLYGONS);
	    AddMenuEntry("Remove all invalid polygons", MENU_EDITOR_REMOVE_ALL_INVALID_POLYGONS);
	    AddMenuEntry("Remove polygon vertex", MENU_EDITOR_REMOVE_VERTEX_POLYGON);
	    AddMenuEntry("Translate polygon", MENU_EDITOR_TRANSLATE_POLYGON);
	    AddMenuEntry("Translate polygon vertex", MENU_EDITOR_TRANSLATE_VERTEX_POLYGON);
	    AddMenuEntry("Rotate polygon", MENU_EDITOR_ROTATE_POLYGON);
	    AddMenuEntry("Translate rotation center", MENU_EDITOR_TRANSLATE_ROTATION_CENTER);
	    AddMenuEntry("File read", MENU_EDITOR_FILE_READ);
	    AddMenuEntry("File save", MENU_EDITOR_FILE_SAVE);
	    AddMenuEntry("File name", MENU_EDITOR_FILE_NAME);
	    AddMenuEntry("Set vertex selection tolerance", MENU_EDITOR_TOL);

	    SetMenu(m_idMenuMain);	    
	    AddSubMenu("Editor", m_idMenuEditor);
	}
	
	virtual void HandleEventOnDisplay(void)
	{
	    GManager::HandleEventOnDisplay();
	    GDrawColor(0.8, 0.8, 0.8);
	    GDrawGrid2D(GDrawGetParam(GDRAW_MINX),
			  GDrawGetParam(GDRAW_MINY),
			  GDrawGetParam(GDRAW_MAXX),
			  GDrawGetParam(GDRAW_MAXY), 120, 60);
	      
	    GDrawLineWidth(1.0);

	    
	    
	    if(m_option == MENU_EDITOR_TRANSLATE_ROTATION_CENTER || 
	       m_option == MENU_EDITOR_ROTATE_POLYGON)
	    {
		GDrawColor(0.0, 0.0, 1.0);
		GDrawCircle2D(m_center[0], m_center[1], m_tol);
	    }
	    
	    GDrawColor(1.0, 0.0, 0.0);
	    const int n = m_polys.size();
	    for(int i = 0; i < n; ++i)
	    {
		const int     nv     = m_polys[i]->size() / 2;
		const double *v      = &(*m_polys[i])[0];
		const bool    closed = m_edit == false || i < n - 1;
		
		for(int j = 0; j < nv; ++j)
		    GDrawCircle2D(v[2 * j], v[2 * j + 1], m_tol);
		for(int j = 0; j < nv - 1; ++j)
		    GDrawSegment2D(v[2 * j], v[2 * j + 1], v[2 * j + 2], v[2 * j + 3]);
		if(closed && nv > 0)
		    GDrawSegment2D(v[2 * nv - 2], v[2 * nv - 1], v[0], v[1]);
	    }	    
	}
	
	virtual bool HandleEventOnMouseLeftBtnDown(const int x, const int y)
	{
	    double wx, wy, wz;
	    MousePosFromScreenToWorld(x, y, &wx, &wy, &wz);
	    
	    GManager::HandleEventOnMouseLeftBtnDown(x, y);
	    
	    if(m_option == MENU_EDITOR_TRANSLATE_ROTATION_CENTER)
	    {
		m_center[0] = wx;
		m_center[1] = wy;
		return true;
	    }
	    else if(m_option == MENU_EDITOR_ADD_VERTEX_POLYGON || m_option == MENU_EDITOR_BEGIN_NEW_POLYGON)
	    {
		if(m_edit == false)
		{
		    if(SelectPolygon(wx, wy))
		    {
			m_edit = true;
			return true;
		    }
		}
		else if(m_polys.size() > 0)
		{
		    m_polys[m_polys.size() - 1]->push_back(wx);
		    m_polys[m_polys.size() - 1]->push_back(wy);
		    return true;
		}	    
	    }
	    else if(m_option == MENU_EDITOR_TRANSLATE_VERTEX_POLYGON)
		m_selVertex = SelectVertex(wx, wy);
	    else if(m_option == MENU_EDITOR_REMOVE_VERTEX_POLYGON)
	    {
		if((m_selVertex = SelectVertex(wx, wy)) >= 0)
		{
		    std::vector<double> *poly = m_polys[m_polys.size() - 1];
		    const int            n    = poly->size();
		    for(int i = 2 * m_selVertex; i < n - 2; ++i)
			(*poly)[i] = (*poly)[i + 2];
		    if(n > 0)
		    {
			poly->pop_back();
			poly->pop_back();
		    }
		    if(poly->size() == 0)
		    {
			m_polys.pop_back();
			delete poly;
		    }
		    m_selVertex = -1;
		    return true;
		}	    
	    }
	    else if(m_option == MENU_EDITOR_TRANSLATE_POLYGON ||
		    m_option == MENU_EDITOR_ROTATE_POLYGON)
	    {
		if((m_transform = SelectPolygon(wx, wy)))
		{
		    m_prev[0] = wx;
		    m_prev[1] = wy;
		}
	    }
	    else if(m_option == MENU_EDITOR_REMOVE_POLYGON)
	    {
		if(SelectPolygon(wx, wy))
		{
		    std::vector<double> *poly = m_polys[m_polys.size() - 1];
		    m_polys.pop_back();
		    delete poly;
		    return true;
		}
	    }
	    else if(m_option == MENU_EDITOR_COPY_POLYGON)
	    {
		if(SelectPolygon(wx, wy))
		{
		    std::vector<double> *poly    = m_polys[m_polys.size() - 1];
		    const int            n       = poly->size();
		    std::vector<double> *newpoly = new std::vector<double>();
		    
		    for(int i = 0; i < n; ++i)
			newpoly->push_back((*poly)[i] + 3 * m_tol);
		    m_polys.push_back(newpoly);
		    return true;
		}
	    }	
	    return false;
	}
	
	
	virtual bool HandleEventOnActiveMouseMove(const int x, const int y)
	{
	    double wx, wy, wz;
	    MousePosFromScreenToWorld(x, y, &wx, &wy, &wz);

	    GManager::HandleEventOnActiveMouseMove(x, y);
	    
	    
	    if(m_option == MENU_EDITOR_TRANSLATE_POLYGON)
	    {
		if(m_transform)
		{
		    std::vector<double> *poly = m_polys[m_polys.size() - 1];
		    const int n = poly->size() / 2;
		    for(int i = 0; i < n; ++i)
		    {
			(*poly)[2 * i] += wx - m_prev[0];
			(*poly)[2 * i + 1] += wy - m_prev[1];
		    }
		    m_prev[0] = wx;
		    m_prev[1] = wy;
		    glutPostRedisplay();
		}
	    }
	    else if(m_option == MENU_EDITOR_ROTATE_POLYGON)
	    {
		if(m_transform)
		{
		    const double angle0 = atan2(m_prev[1] - m_center[1], m_prev[0] - m_center[0]);
		    const double angle1 = atan2(wy - m_center[1], wx - m_center[0]);
		    double TR[Algebra2D::TransRot_NR_ENTRIES];
		    
		    Algebra2D::RotateAroundPointAsTransRot(angle1 - angle0, m_center, TR);
		    
		    std::vector<double> *poly = m_polys[m_polys.size() - 1];
		    const int n = poly->size() / 2;
		    
		    for(int i = 0; i < n; ++i)
			Algebra2D::TransRotMultPoint(TR, &(*poly)[2 * i], &(*poly)[2 * i]);
		    
		    m_prev[0] = wx;
		    m_prev[1] = wy;
		    return true;
		}
	    }
	    else if(m_option == MENU_EDITOR_TRANSLATE_VERTEX_POLYGON)
	    {
		if(m_selVertex >= 0)
		{
		    (*(m_polys[m_polys.size() - 1]))[2 * m_selVertex + 0] = wx;
		    (*(m_polys[m_polys.size() - 1]))[2 * m_selVertex + 1] = wy;
		    return true;
		}	    
	    }
	    
	}
	
	
	virtual bool HandleEventOnNormalKeyPress(const int key)
	{
	    if(GManager::HandleEventOnNormalKeyPress(key))
		return key;
	    
	    switch(key)
	    {
	    case 'b': case 'B':
		HandleEventOnMenu(MENU_EDITOR_BEGIN_NEW_POLYGON);
		return true;
	    	    
	    case 'e': case 'E':
		HandleEventOnMenu(MENU_EDITOR_END_NEW_POLYGON);
		return true;
	    
	    case 'a': case 'A':
		HandleEventOnMenu(MENU_EDITOR_ADD_VERTEX_POLYGON);
		return true;
		
	    case 'c': case 'C':
		HandleEventOnMenu(MENU_EDITOR_COPY_POLYGON);
		return true;
	    
	    case 'r': case 'R':
		HandleEventOnMenu(MENU_EDITOR_REMOVE_POLYGON);
		return true;

	    case 's':
		HandleEventOnMenu(MENU_EDITOR_FILE_SAVE);
		return true;
	    }
	}
	
	
	virtual bool HandleEventOnMenu(const int item)
	{
	    if(GManager::HandleEventOnMenu(item))
		return true;
	    
	    if(item != MENU_EDITOR_END_NEW_POLYGON && m_edit == true)
	    {
		printf("A polygon is alread being edited.\nFinish that first!\n");
		return true;
	    }
	    
	    m_option = item;
	    
	    if(item == MENU_EDITOR_BEGIN_NEW_POLYGON)
	    {
		m_edit = true;
		m_polys.push_back(new std::vector<double>());
		return true;
	    }
	    else if(item == MENU_EDITOR_END_NEW_POLYGON)
	    {
		m_edit = false;
		return true;
	    }					
	    else if(item == MENU_EDITOR_REMOVE_ALL_POLYGONS)
	    {
		for(int i = 0; i < (int) m_polys.size(); ++i)
		    delete m_polys[i];
		m_polys.clear();
		m_edit = false;
		m_selVertex = -1;
		return true;
	    }
	    else if(item == MENU_EDITOR_REMOVE_ALL_INVALID_POLYGONS)
	    {
		int i = 0;
		while(i < (int) m_polys.size())
		{
		    if(ValidatePolygon(i))
			++i;
		}
		return true;
	    }
	    else if(item == MENU_EDITOR_FILE_NAME)
	    {
		printf("enter file name:");
		scanf("%s", m_fname);
		return true;		
	    }
	    else if(item == MENU_EDITOR_FILE_READ)
	    {
		if(m_fname[0] == '\0')
		{
		    printf("enter file name:");
		    scanf("%s", m_fname);
		}
		FILE *in = fopen(m_fname, "r");
		if(in)
		{
		    printf("reading from <%s>\n", m_fname);
		    int n;		    
		    char name[100];
		    std::vector<double> *poly = new std::vector<double>();
		    
		    fscanf(in, "%d", &n);
		    for(int i = 0; i < n; ++i)
		    {
			printf("reading polygon %d out of %d\n", i, n);
			poly = new std::vector<double>();
			if(ReadPolygon2D(in, poly))
			{
			    m_polys.push_back(poly);
			    poly = new std::vector<double>();
			}
		    }
		    delete poly;
		    fclose(in);
		}
		else
		    printf("failed to open <%s> for reading\n", m_fname);
		return true;
	    }
	    else if(item == MENU_EDITOR_FILE_SAVE)
	    {	
		if(m_fname[0] == '\0')
		{
		    printf("enter file name:");
		    scanf("%s", m_fname);
		}
		
		FILE *out = fopen(m_fname, "w");
		if(out)
		{
		    fprintf(out, "%d\n", m_polys.size());
		    for(int i = 0; i < (int) m_polys.size(); ++i)
			PrintPolygon2D(out, m_polys[i]->size() / 2, &(*(m_polys[i]))[0]);
		    fclose(out);
		    printf("wrote %d polygons to <%s>\n", m_polys.size(), m_fname);
		}
		else 
		    printf("failed to open <%s> for writing\n", m_fname);
		return true;
	    }
	    else if(item == MENU_EDITOR_TOL)
	    {
		printf("enter vertex selection tolerance:");
		scanf("%lf", &m_tol);
		return true;
	    }
	    
	    return false;	    
	}
	
    protected:
	bool SelectPolygon(const double posX, const double posY)
	{
	    const double p[] = {posX, posY};
	    const int n = m_polys.size();
	    for(int i = 0; i < n; ++i)
		if(IsPointInsidePolygon2D(p, m_polys[i]->size() / 2, &((*(m_polys[i]))[0])))
		{
		    std::vector<double> *poly = m_polys[i];
		    m_polys[i]                = m_polys[n - 1];
		    m_polys[n - 1]            = poly;
		    return true;
		}
	    return SelectVertex(posX, posY) >= 0;
	}
	
	int SelectVertex(const double posX, const double posY)
	{
	    const int            n    = m_polys.size();
	    std::vector<double> *poly = NULL;
	    int                  nv   = 0;
	    double               d    = 0.0;
	    
	    for(int i = 0; i < n; ++i)
	    {
		poly = m_polys[i];
		nv   = poly->size() / 2;
		for(int j = 0; j < nv; ++j)
		{
		    d = (posX - (*poly)[2 * j + 0]) * (posX - (*poly)[2 * j + 0]) +
			(posY - (*poly)[2 * j + 1]) * (posY - (*poly)[2 * j + 1]);
		    if(d < m_tol)
		    {
			m_polys[i] = m_polys[n - 1];
			m_polys[n - 1] = poly;
			return j;		    
		    }
		}
	    }
	    return -1;
	}
	
	bool ValidatePolygon(const int i)
	{	
	    const int n = m_polys[i]->size() / 2;
	    if(n < 3 || SelfIntersectPolygon2D(n, &((*(m_polys[i]))[0])))
	    {
		std::vector<double> *poly = m_polys[i];
		m_polys[i] = m_polys[m_polys.size() - 1];
		m_polys[m_polys.size() - 1] = poly;
		m_polys.pop_back();
		return false;
	    }
	    return true;
	}

	std::vector< std::vector<double>* > m_polys;
	
	int m_idMenuEditor;
	int MENU_EDITOR_BEGIN_NEW_POLYGON;
	int MENU_EDITOR_END_NEW_POLYGON;
	int MENU_EDITOR_ADD_VERTEX_POLYGON;
	int MENU_EDITOR_COPY_POLYGON;
	int MENU_EDITOR_REMOVE_POLYGON;
	int MENU_EDITOR_REMOVE_ALL_POLYGONS;
	int MENU_EDITOR_REMOVE_ALL_INVALID_POLYGONS;
	int MENU_EDITOR_REMOVE_VERTEX_POLYGON;
	int MENU_EDITOR_TRANSLATE_POLYGON;
	int MENU_EDITOR_TRANSLATE_VERTEX_POLYGON;
	int MENU_EDITOR_ROTATE_POLYGON;
	int MENU_EDITOR_TRANSLATE_ROTATION_CENTER;
	int MENU_EDITOR_FILE_READ;
	int MENU_EDITOR_FILE_SAVE;
	int MENU_EDITOR_FILE_NAME;
	int MENU_EDITOR_TOL;

	
	int    m_option;
	double m_tol;
	double m_center[2];
	double m_prev[2];
	char   m_fname[200];
	
	bool   m_edit;
	bool   m_transform;
	int    m_selVertex;
    };	
}

extern "C" int RunGEditor2D(int argc, char **argv)
{
    Abetare::GEditor2D gManager(argc > 1 ? argv[1] : NULL);

    gManager.MainLoop("GEditor2D", 800, 800);
        
    return 0;
}
