#ifndef ABETARE__MISC_HPP_
#define ABETARE__MISC_HPP_

#include <vector>
#include <cstdlib>
#include <cstring>
#include "Utils/PseudoRandom.hpp"

namespace Abetare
{
    static inline
    bool IsOdd(const int x) 
    {
	return x & 1;
    }
    
    static inline
    bool IsEven(const int x)
    {
	return !IsOdd(x);
    }
    
    static inline
    bool StrEqualCaseInsensitive(const char * const s1, const char * const s2)
    {
	int i = 0;
	char a, b;
	
	for(; s1[i] != '\0' && s2[i] != '\0'; ++i)
	{   
	    if(s1[i] >= 'a' && s1[i] <= 'z' && s2[i] >= 'A' && s2[i] <= 'Z')
	    {
		if((s1[i] - 'a') != (s2[i] - 'A'))
		    return false;
	    }
	    else if(s1[i] >= 'A' && s1[i] <= 'Z' && s2[i] >= 'a' && s2[i] <= 'z')
	    {
		if((s1[i] - 'A') != (s2[i] - 'a'))
		    return false;
	    }
	    else if(s1[i] != s2[i])
		return false;
	}
	
	return s1[i] == s2[i];
    }
    
    template <typename Item>
    static inline void DeleteItems(const int n, Item items[])
    {
	for(int i = 0; i < n; ++i)
	    if(items[i])
		delete items[i];	
    }
    
    template <typename Item>
    static inline void DeleteItems(std::vector<Item> * const items)
    {
	DeleteItems<Item>(items->size(), &((*items)[0]));	
    }
    
    template <typename Item>
    static inline void FreeItems(const int n, Item items[])
    {
	for(int i = 0; i < n; ++i)
	    if(items[i])
		free(items[i]);	
    }
    
    template <typename Item>
    static inline void FreeItems(std::vector<Item> * const items)
    {
	FreeItems<Item>(items->size(), &((*items)[0]));	
    }
    
    template <typename Item>
    static inline int FindItem(const int n, const Item items[], const Item & item)
    {
	for(int i = 0; i < n; ++i)
	    if(items[i] == item)
		return i;
	return -1;
    }
    
    template <typename Item>
    static inline int FindItem(const std::vector<Item> * const items, const Item & item)
    {
	return FindItem<Item>(items->size(), &((*items)[0]), item);
    }
    
    template <typename Item>
    static inline void ReverseItems(const int n, Item items[])
    {
	Item tmp;	
	for(int i = 0, j = n - 1; i < j; ++i, --j)
	{
	    tmp      = items[i];
	    items[i] = items[j];
	    items[j] = tmp;	    
	}	
    }
    
    template <typename Item>
    static inline void ReverseItems(std::vector<Item> * const items)
    {
	ReverseItems<Item>(items->size(), &((*items)[0]));	
    }
    
    
    template <typename Item>
    static inline int MoveDuplicateItemsToEnd(const int n, Item items[])
    {
	int nnew = n;
	
	for(int i = 1; i < nnew;)
	{
	    const int pos = FindItem<Item>(i, items, items[i]);
	    if(pos >= 0)
	    {
		Item tmp = items[i];
		items[i] = items[nnew - 1];
		items[nnew - 1] = tmp;
		--nnew;		
	    }
	    else
		++i;
	}
	return nnew;
    }
    
    template <typename Item>
    static inline int MoveDuplicateItemsToEnd(std::vector<Item> * const items)
    {
	return MoveDuplicateItemsToEnd<Item>(items->size(), &((*items)[0]));
    }
    
    template <typename Item>
    static void PermuteItems(const int n, Item items[], const int r)
    {
	for(int i = 0; i < r; i++)
	{
	    int  j = RandomUniformInteger(i, n - 1);
	    Item t = items[i]; items[i] = items[j]; items[j] = t;	
	}
    }
    
    template <typename Item>
    static inline void PermuteItems(std::vector<Item> * const items, const int r)
    {
	PermuteItems<Item>(items->size(), &((*items)[0]), r);
    }
    
    template <typename Type>
    static inline bool NextPermutation(const int n, Type items[])
    {
	int i, j, k, t;
	
	//starting from right to left examine the array to find
	//the largest tail that is in descending order
	for(i = n - 1; i > 0 && items[i] < items[i-1]; i--);
	
	//the longest tail is the entire permutation
	//i.e.,  n, n-1,..., 2, 1 is the largest possible permutation
	if(i == 0) 
	    return true;
	
	//permutation looks like 
	// items[0] < ... < items[i-2] < items[i-1] < items[i] > items[i+1] > items[i+2] >... > items[n-1]
	
	//since we are after next largest permutation we should keep
	//items[0], items[1],...,items[i-2] unchanged and change items[i-1] to something
	//larger but as small as possible.
	//Need to find items[k] > items[i-1] among the longest tail which is the
	//smallest possible
	for(k = i, j = i+1; j < n; j++)
	    if(items[j] > items[i-1] && items[j] < items[k]) 
		k = j;
	
	//swap items[k] with items[i-1]
	t         = items[k]; 
	items[k]  = items[i-1]; 
	items[i-1]= t;
	
	//reverse tail items[i..n-1]
	k = (n - i) / 2;    
	for(j = 0; j < k; j++)
	{
	    t                = items[i + j]; 
	    items[i+j]       = items[n - 1 - j]; 
	    items[n - 1 - j] = t;
	}
	return true;
    }
    
    static inline
    bool NextCombination(const int n, int combo[], const int r)
    {
	if(r == 0 || combo[0] >= n - r)
	    return false;
	
	int i = r - 1;
	while (combo[i] == n - r + i) 
	    i--;
	
	combo[i] = combo[i] + 1;
	for (int j = i + 1; j < r; j++) 
	    combo[j] = combo[i] + j - i;
	return true;    
    }

}	


#endif



