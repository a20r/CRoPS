#ifndef ABETARE__STATISTICS_HPP_
#define ABETARE__STATISTICS_HPP_

#include <vector>

namespace Abetare
{
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
    
    
    
}	


#endif



