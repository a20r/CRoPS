#include "Utils/Geometry.hpp"
#include "Utils/Constants.hpp"
#include "Utils/PseudoRandom.hpp"
#include "Utils/Algebra2D.hpp"
#include "Utils/Polygon2D.hpp"
#include "Utils/GenerateMaze2D.hpp"
#include <vector>
#include <cstdlib>
#include <cstdio>

using namespace Abetare;

const double bounds[4] = {-10.5, -8.5, 10.5, 16.5};
	

extern "C" int GenerateScene1(int argc, char **argv)
{
    std::vector<double>    *poly;
    std::vector<std::vector<double>*> polys;
    std::vector<double>     skel;
    const double thick = 0.3;
    double       t     = 4.0;
    double       x     = 0;
    double       y     = 0;
    double       open  = argc > 2 ? atof(argv[2]) : 7.0/8;
    
    do
    {
	x =     0.45 * pow(t, open) * cos(t);
	y = 5 + 0.50 * pow(t, open) * sin(t);
	skel.push_back(x);
	skel.push_back(y);

	t += 0.3;
    }
    while(x >= bounds[0] && x <= bounds[2] &&
	  y >= bounds[1] && y <= bounds[3]);

    
    poly = new std::vector<double>();    
    Abetare::FromSkeletonToPolygon2D(skel.size() / 2, &skel[0], thick, poly);
    polys.push_back(poly);

    poly = new std::vector<double>();    
    GenerateArcAsPolygon2D(-8, -7, 2.5,
			   Constants::DEG2RAD * 50, Constants::DEG2RAD * 20, 7,
			   thick, poly);
    polys.push_back(poly);
   
    poly = new std::vector<double>();    
    GenerateArcAsPolygon2D(8, -7, 2.,
			   Constants::DEG2RAD * 140, Constants::DEG2RAD * 20, 7,
			   thick, poly);
    polys.push_back(poly);
    

    poly = new std::vector<double>();    
    GenerateArcAsPolygon2D(9, 13, 2.5,
			   Constants::DEG2RAD * 50, Constants::DEG2RAD * 20, 7,
			   thick, poly);
    polys.push_back(poly);


    poly = new std::vector<double>();    
    poly->resize(16);
    CircleAsPolygon2D(-0.75, 5.9, 0.6, 8, &(*poly)[0]);
    polys.push_back(poly);
    

    poly = new std::vector<double>();    
    poly->resize(8);
    CircleAsPolygon2D(3, -4, 0.4, 4, &(*poly)[0]);
    polys.push_back(poly);
    

    poly = new std::vector<double>();    
    poly->resize(10);
    CircleAsPolygon2D(-4, -3, 0.4, 5, &(*poly)[0]);
    polys.push_back(poly);
   
    FILE *out = fopen(argv[1], "w");
    fprintf(out, "%d\n", polys.size());
    for(int i = 0; i < polys.size(); ++i)
	Abetare::PrintPolygon2D(out, polys[i]->size() / 2, &(*(polys[i]))[0]);
    fclose(out);
    
    
}



extern "C" int GenerateScene2(int argc, char **argv)
{
    std::vector<std::vector<double>*> polys;
    std::vector<double> *poly;
    
    double x    = 0;
    double y    = 0;    
    double rmin = 4;    
    double rmax = 22;
    double thetaStart = 0;
    int    nrRays     = 8;
    double thetaInc   = 360 * Abetare::Constants::DEG2RAD / nrRays;
    double thick      = 0.5;
    
    Abetare::GenerateRadialPatternAsPolygons2D(x, y, rmin, rmax, 
					       thetaStart, thetaInc, nrRays, thick, &polys);
    
    poly = new std::vector<double>();    
    Abetare::GenerateArcAsPolygon2D(x, y, rmax + 1, thetaStart + thetaInc, thetaInc, nrRays, thick, poly);
    polys.push_back(poly);
    
    
    for(int i = 0; i < nrRays; ++i)
    {
	poly = new std::vector<double>();
	Abetare::GenerateArcAsPolygon2D(x, y, rmax - 2, 
					thetaStart + (i + 0.25) * thetaInc, 0.5 * thetaInc, 
					2, 3 * thick, poly);
	polys.push_back(poly);	
    }
    
    FILE *out = fopen(argv[1], "w");
    fprintf(out, "%d\n", polys.size());
    for(int i = 0; i < polys.size(); ++i)
	Abetare::PrintPolygon2D(out, polys[i]->size() / 2, &(*(polys[i]))[0]);
    fclose(out);
    
    return 0;
}



extern "C" int GenerateScene4(int argc, char **argv)
{
    std::vector<std::vector<double>*> polys;
    std::vector<double> *poly;
    std::vector<double>  skel;
    const double minTheta = -2 * M_PI;
    const double maxTheta =  2 * M_PI;
    const double incTheta =  25 * Abetare::Constants::DEG2RAD;
    const double xscale   = 2;
    const double yscale   = 6;
    const double thick    = 0.5;
        
    for(double theta = minTheta - 0.5 * M_PI; theta <= maxTheta; theta += incTheta)
    {
	skel.push_back(theta * xscale);
	skel.push_back(6 + sin(theta) * yscale);	
    }
    
    poly = new std::vector<double>();    
    Abetare::FromSkeletonToPolygon2D(skel.size() / 2, &skel[0], thick, poly);
    polys.push_back(poly);

    skel.clear();    
    for(double theta = minTheta; theta <= maxTheta + 0.5 * M_PI; theta += incTheta)
    {
	skel.push_back(theta * xscale);
	skel.push_back(-6 + sin(theta) * yscale);	
    }
    poly = new std::vector<double>();    
    Abetare::FromSkeletonToPolygon2D(skel.size() / 2, &skel[0], thick, poly);
    polys.push_back(poly);
    
    skel.clear();
//top-left
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(-19, 14, 8, 
				     70 * Abetare::Constants::DEG2RAD,
				     20 * Abetare::Constants::DEG2RAD,
				     12, thick, poly);
    polys.push_back(poly);
 
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(-19, 14, 4, 
				     -150 * Abetare::Constants::DEG2RAD,
				      20 * Abetare::Constants::DEG2RAD,
				      13, thick, poly);
    polys.push_back(poly);

//bottom-right
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(22, -14, 8, 
				     70 * Abetare::Constants::DEG2RAD,
				     20 * Abetare::Constants::DEG2RAD,
				     12, thick, poly);
    polys.push_back(poly);
 
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(22, -14, 4, 
				     -150 * Abetare::Constants::DEG2RAD,
				      20 * Abetare::Constants::DEG2RAD,
				      13, thick, poly);
    polys.push_back(poly);
    
 
//top-right
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(19, 14, 8, 
				     -70 * Abetare::Constants::DEG2RAD,
				     20 * Abetare::Constants::DEG2RAD,
				     12, thick, poly); 
    polys.push_back(poly);

    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(19, 14, 4, 
				     -70 * Abetare::Constants::DEG2RAD,
				      20 * Abetare::Constants::DEG2RAD,
				      13, thick, poly);
    polys.push_back(poly);
    

//bottom-left
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(-20, -15, 8, 
				     -70 * Abetare::Constants::DEG2RAD,
				     20 * Abetare::Constants::DEG2RAD,
				     12, thick, poly); 
    polys.push_back(poly);

    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(-20, -15, 4, 
				     -70 * Abetare::Constants::DEG2RAD,
				      20 * Abetare::Constants::DEG2RAD,
				      13, thick, poly);
    polys.push_back(poly);
    

//close waves
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D(-3, 13, 5, 
				      20 * Abetare::Constants::DEG2RAD,
				      15.5 * Abetare::Constants::DEG2RAD,
				      10, thick, poly);
    polys.push_back(poly);
   
    poly = new std::vector<double>();
    Abetare:: GenerateArcAsPolygon2D( 3, -13, 5, 
				     -160 * Abetare::Constants::DEG2RAD,
				      15.5 * Abetare::Constants::DEG2RAD,
				      10, thick, poly);
    polys.push_back(poly);
    

 
    FILE *out = fopen(argv[1], "w");
    fprintf(out, "%d\n", polys.size());
    for(int i = 0; i < polys.size(); ++i)
	Abetare::PrintPolygon2D(out, polys[i]->size() / 2, &(*(polys[i]))[0]);
    fclose(out);
    
    return 0;
}


extern "C" int GenerateScene3(int argc, char **argv)
{
    std::vector<std::vector<double>*> polys;
    std::vector<double> *poly;
    
    double x    = 0;
    double y    = 0;    
    double rmin = 4;    
    double rmax = 25;
    double thetaStart = 0;
    int    nrRays     = 0;
    double thetaInc   = 0;
    double theta      = 0;    
    double thick      = 0.5;
    double gap    = 2;    
    int    nrSegs = 4;
    double seg    = 0;
    double thetagap= 0;
    double thetaprev= Abetare::RandomUniformReal(-M_PI, M_PI);
    
    
    
    for(double r = rmin; r <= rmax; r += 4)
    {
	gap         = Abetare::RandomUniformReal(2, 3);
	thetagap    = gap / r;
	theta       = (2 * M_PI - nrSegs * thetagap) / nrSegs;
	thetaStart  = thetaprev + 
	              Abetare::RandomUniformReal(30 * Abetare::Constants::DEG2RAD, 60 * Abetare::Constants::DEG2RAD);	
	thetaprev   = thetaStart;	
	nrRays      = 5;
	thetaInc    = theta / (nrRays - 1);

	for(int i = 0; i < nrSegs; ++i)
	{
	    poly       = new std::vector<double>();    
	    Abetare::GenerateArcAsPolygon2D(x, y, r, thetaStart, thetaInc, nrRays, thick, poly);
	    polys.push_back(poly);
	    thetaStart += theta + thetagap;
	}
    }
    
    poly = new std::vector<double>();
    poly->push_back(-1); poly->push_back(-1);
    poly->push_back( 1); poly->push_back(-1);
    poly->push_back( 1); poly->push_back( 1);
    poly->push_back(-1); poly->push_back( 1);
    polys.push_back(poly);
    
    
    FILE *out = fopen(argv[1], "w");
    fprintf(out, "%d\n", polys.size());
    for(int i = 0; i < polys.size(); ++i)
	Abetare::PrintPolygon2D(out, polys[i]->size() / 2, &(*(polys[i]))[0]);
    fclose(out);
    
    return 0;
}


extern "C" int Maze2D(int argc, char **argv)
{
    const int xdim = 24;
    const int ydim = 24;
    const double width = 0.1 * 60 / xdim;
    
    Abetare::GenerateMaze2D gmaze;
    
    gmaze.m_min[0]  = -30;
    gmaze.m_min[1]  = -27.5;
    gmaze.m_max[0]  =  30;
    gmaze.m_max[1]  =  27.5;
    gmaze.m_dims[0] =  xdim;
    gmaze.m_dims[1] =  ydim;
    
    gmaze.RandomizedKruskal();
    
    FILE *out = fopen(argv[1], "w");
    gmaze.PrintWalls(width, out);
    const double x = Abetare::RandomUniformReal(-29.5, 28.5);
    const double y = Abetare::RandomUniformReal( 28.7, 29.0);
    
    fprintf(out, "1 4 %f %f %f %f %f %f %f %f\n", x, y, x + 1, y, x + 1, y + 1, x, y + 1);    
    fclose(out);

    return 0;
    
}


extern "C" int GenerateRandomPolygons2D(int argc, char **argv)
{
    const char *fname = argv[1];
    const double perc = argc > 2 ? atof(argv[2]) : 0.10;
    const double d    = argc > 3 ? atof(argv[3]) : 2.0;
    

    const double rmin = 0.5;
    const double rmax = 2.0;
    bool         tooClose = false;
    
    std::vector< std::vector<double>* > polys;
    std::vector<double> poly;
    double pmin1[2], pmin2[2], x, y, r;
    double TR[Abetare::Algebra2D::TransRot_NR_ENTRIES];
    
    
    
    std::vector<double> *v = new std::vector<double>();
    x = Abetare::RandomUniformReal(-30, 30);
    y = Abetare::RandomUniformReal( 28.5, 29);    
    v->push_back(x); v->push_back(y);
    v->push_back(x + 1); v->push_back(y);
    v->push_back(x + 1); v->push_back(y + 1);
    v->push_back(x); v->push_back(y + 1);
    polys.push_back(v);
    

    double areas = 0;
    int    i     = 0;
    
    while(areas < 3600 * perc)
    {
	if(i % 100 == 0)
	    printf("generating polygon %d [d = %f areas = %f %f]\n", i, d, areas, areas/3600);
	++i;
	
	do
	{
	    do
	    {
		x = Abetare::RandomUniformReal(-30, 30);
		y = Abetare::RandomUniformReal(-28, 28);
		r = Abetare::RandomUniformReal(rmin, rmax);
	    }
	    while((r + x) > 30 || (x - r) < -30 || (y - r) < -28.5 || (y + r) > 28.5);
	    
	    const double rnum = Abetare::RandomUniformReal();
	    
	    //  if(rnum < 0.65)
	    {
		const int nv = Abetare::RandomUniformInteger(3, 6);
		poly.resize(2 * nv);
		Abetare::CircleAsPolygon2D(x, y, r, nv, &poly[0]);
	    }
/*
	    else if(rnum < 0.55)
	    {
		poly.clear();
		const double length = r;
		const double width  = r * Abetare::RandomUniformReal(0.1, 0.3);
		poly.clear();
		poly.push_back(-0.5 * length); poly.push_back(-0.5 * width);
		poly.push_back( 0.5 * length); poly.push_back(-0.5 * width);
		poly.push_back( 0.5 * length); poly.push_back( 0.5 * width);
		poly.push_back(-0.5 * length); poly.push_back( 0.5 * width);

		double angle = Abetare::RandomUniformReal(-M_PI, M_PI);
		TR[0] = x;
		TR[1] = y;		
		Abetare::Algebra2D::AngleAsRot(angle, &TR[2]);
		Abetare::ApplyTransRotToPolygon2D(TR, poly.size() / 2, &poly[0], &poly[0]);
	    }
	    else
	    {
		poly.clear();
		poly.push_back(0); 	 poly.push_back(0);
		poly.push_back(r);       poly.push_back(0);
		poly.push_back(r);       poly.push_back(0.3 * r);
		poly.push_back(0.3 * r); poly.push_back(0.3 * r);
		poly.push_back(0.3 * r); poly.push_back(r);
		poly.push_back(0);       poly.push_back(r);

		double angle = Abetare::RandomUniformReal(-M_PI, M_PI);
		TR[0] = x;
		TR[1] = y;		
		Abetare::Algebra2D::AngleAsRot(angle, &TR[2]);
		Abetare::ApplyTransRotToPolygon2D(TR, poly.size() / 2, &poly[0], &poly[0]);		
	    }*/
	    Abetare::MakePolygonCCW2D(poly.size() / 2, &poly[0]);
	    
	    tooClose = false;	    
	    for(int j = 0; j < polys.size() && !tooClose; ++j)
		if((Abetare::DistSquaredPolygons2D(polys[j]->size() / 2,
						  &(*(polys[j]))[0],
						   poly.size() / 2, &poly[0], pmin1, pmin2) < d * d) ||
		   Abetare::IsPolygonInsidePolygon2D(polys[j]->size() / 2,
						     &(*(polys[j]))[0],
						     poly.size() / 2, &poly[0]) ||
		   Abetare::IsPolygonInsidePolygon2D(poly.size() / 2, &poly[0],
						     polys[j]->size() / 2,
						     &(*(polys[j]))[0]))
		    tooClose = true;
		    
	}
	while(tooClose);
	
	v = new std::vector<double>();
	v->assign(poly.begin(), poly.end());
	polys.push_back(v);

	areas += fabs(Abetare::SignedAreaPolygon2D(poly.size() / 2, &poly[0]));
	
    }


    FILE *out = fopen(fname, "w");
    fprintf(out, "%d\n", polys.size() - 1);
    for(int i = 1; i < polys.size(); ++i)
	Abetare::PrintPolygon2D(out, polys[i]->size() / 2, &(*(polys[i]))[0]);
    fprintf(out, "1\n");    
    Abetare::PrintPolygon2D(out, polys[0]->size() / 2, &(*(polys[0]))[0]);
    fclose(out);

    printf("areas = %f %f\n", areas, areas / 3600);
    
    return 0;
}




