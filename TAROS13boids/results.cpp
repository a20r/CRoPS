#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>

template <typename Type>
static inline double Sum(const int n, const Type items[])
{
    double s = 0;	
    for(int i = 0; i < n; ++i)
	s += items[i];
    return s;
}

template <typename Type>
static inline double Mean(const int n, const Type items[])
{
    return Sum(n, items) / n;	
}

template <typename Type>
static inline double Variance(const int n, const Type items[], const double mean)
{
    double s = 0;
    for(int i = 0; i < n; ++i)
	s += (items[i] - mean) * (items[i] - mean);
    return s / n;
}

template <typename Type>
static inline double Variance(const int n, const Type items[])
{
    return Variance(n, items, Mean(n, items));
}

template <typename Type>
static inline double MedianInSorted(const int n, const Type items[])
{
    return n % 2 ? items[n / 2] : ((items[n / 2] + items[n / 2 - 1]) / 2.0);
}

const int off = 0;

struct Result
{
    std::vector<double> m_times;
    std::vector<double> m_dists;
    std::vector<int>    m_boids;
    double              m_first;
    std::vector<double> m_dmin;
    
};

void ReadResult(const char fname[], Result * const r)
{
    printf("reading file <%s>\n", fname);
    
    FILE *in = fopen(fname, "r");
    char buffer[300];
    double time, dist, dmin;
    int    boids;

    const double radius = 4;
    
    
    if(!in)
	return;

    r->m_times.clear();
    r->m_dists.clear();
    r->m_boids.clear();
    
    r->m_first = -1;
    
    while(fscanf(in, "%s %lf %s %s %lf %s %s %lf %s %s %d", 
		 buffer, &time, 
                 buffer, buffer, &dist, 
                 buffer, buffer, &dmin,
		 buffer, buffer, &boids) == 11)
    {
	r->m_times.push_back(time);
	r->m_dists.push_back(dist / (2 * radius));
	r->m_dmin.push_back(dmin/(2 * radius));
	r->m_boids.push_back(boids);

	if(r->m_first < 0 && boids > 0)
	    r->m_first = time;
    }
    
    fclose(in);
}

void ReportTime(const char prefix[], const bool impede = false)
{
    const int boids[] = {1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    const int nrBoids = 11;
    const int nrRuns  = 20;
    
    Result r;
    char buffer[300];

    std::vector<double> times;
    double tavg, tstd;
    
    sprintf(buffer, "%s_times.txt", prefix);
    
    FILE *out = fopen(buffer, "w");
    

    std::vector<double> tuse;
    

    for(int i = 0; i < nrBoids; ++i)
    {
	times.clear();
	for(int run = 0; run < nrRuns; ++run)
	{
	    sprintf(buffer, "%s_%s_%d_%d", prefix, 
		    impede ? "random_walk" : "output", boids[i], run);
	    ReadResult(buffer, &r);
	    times.push_back(r.m_times.back());
	}

	std::sort(times.begin(), times.end());
	
	
	tavg = Mean<double>(times.size() - 2 * off, &(times[off]));
	tstd = sqrt(Variance<double>(times.size() - 2 * off, &(times[off]), tavg));

	fprintf(out, "%3d %6.2f %6.2f\n", boids[i], tavg, 0.5 * tstd);

	tuse.push_back(tavg);
	
    }

    for(int i = 0; i < tuse.size(); ++i)
	fprintf(out, "%3d %6.2f %6.2f\n", boids[i], tuse[i] / tuse[0], tuse[i] / (tuse[0] * boids[i]));
  
    fclose(out);
    
}


void ReportFirstLast(const char prefix[], const bool impede = false)
{
    const int boids[] = {1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    const int nrBoids = 11;
    const int nrRuns  = 20;
    
    Result r;
    char buffer[300];

    std::vector<double> times;
    double tavg, tstd;
    
    sprintf(buffer, "%s_fl.txt", prefix);
    
    FILE *out = fopen(buffer, "w");
    
    
    for(int i = 0; i < nrBoids; ++i)
    {
	times.clear();
	for(int run = 0; run < nrRuns; ++run)
	{

	    sprintf(buffer, "%s_%s_%d_%d", prefix, 
		    impede ? "random_walk" : "output", boids[i], run);
	    ReadResult(buffer, &r);
	    times.push_back(r.m_times.back() - r.m_first);
	}

	std::sort(times.begin(), times.end());
	
	
	tavg = Mean<double>(times.size() - 2 * off, &(times[off]));
	tstd = sqrt(Variance<double>(times.size() - 2 * off, &(times[off]), tavg));

	fprintf(out, "%3d %6.2f %6.2f\n", boids[i], tavg, 0.5 * tstd);
    }

    fclose(out);
    
}

void ReportDists(const char prefix[], const bool impede = false)
{
    const int boids[] = {1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    const int nrBoids = 11;
    const int nrRuns  = 20;
    
    Result r;
    char buffer[300];

    std::vector<double> dists;
    double davg, dstd;
    
    sprintf(buffer, "%s_dists.txt", prefix);
    
    FILE *out = fopen(buffer, "w");
    
    
    for(int i = 0; i < nrBoids; ++i)
    {
	dists.clear();
	for(int run = 0; run < nrRuns; ++run)
	{

	    sprintf(buffer, "%s_%s_%d_%d", prefix, 
		    impede ? "random_walk" : "output", boids[i], run);
	    ReadResult(buffer, &r);
	    dists.insert(dists.end(), r.m_dists.begin(), r.m_dists.end());
	}

	std::sort(dists.begin(), dists.end());

	
	
	
	davg = Mean<double>(dists.size() - 2 * off, &(dists[off]));
	dstd = sqrt(Variance<double>(dists.size() - 2 * off, &(dists[off]), davg));

	fprintf(out, "%3d %6.2f %6.2f\n", boids[i], davg, 0.5 * dstd);
    }

    fclose(out);
}

void ReportAnytime(const char fname[], const char fout[])
{
    Result r;

    ReadResult(fname, &r);
    
    
    FILE *out = fopen(fout, "w");
    for(int i = 0; i < r.m_times.size(); ++i)
	fprintf(out, "%f %f %d\n", r.m_times[i], r.m_dists[i], r.m_boids[i]);
}






int main(int argc, char **argv)
{
    ReportTime("stats/m7");
    ReportDists("stats/m7");
    ReportFirstLast("stats/m7");
  
    ReportTime("stats/scene1");
    ReportDists("stats/scene1");
    ReportFirstLast("stats/scene1");
  
    ReportTime("stats/scene2");
    ReportDists("stats/scene2");
    ReportFirstLast("stats/scene2");


    ReportTime("impede/m7", true);
    ReportDists("impede/m7", true);
    ReportFirstLast("impede/m7", true);
  
    ReportTime("impede/scene1", true);
    ReportDists("impede/scene1", true);
    ReportFirstLast("impede/scene1", true);
  
    ReportTime("impede/scene2", true);
    ReportDists("impede/scene2", true);
    ReportFirstLast("impede/scene2", true);
  
 
    //   ReportAnytime(argv[1], argv[2]);
    

}



    
