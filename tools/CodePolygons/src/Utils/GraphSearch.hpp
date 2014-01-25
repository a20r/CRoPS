#ifndef ABETARE__GRAPH_SEARCH_HPP_
#define ABETARE__GRAPH_SEARCH_HPP_

#include "Utils/Definitions.hpp"
#include "Utils/Map.hpp"
#include "Utils/Heap.hpp"
#include "Utils/Misc.hpp"
#include <stack>
#include <queue>
#include <cstdio>

namespace Abetare
{
    template <typename Key>
    class GraphSearchInfo
    {
    public:
	GraphSearchInfo(void)
	{
	}
	
	virtual ~GraphSearchInfo(void)
	{
	}

	virtual void GetOutEdges(const Key u, 
				 std::vector<Key> * const edges,
				 std::vector<double> * const costs = NULL) const = 0;
	
	virtual bool IsGoal(const Key key) const = 0;	

	virtual double HeuristicCostToGoal(const Key u) const
	{
	    return 0;
	}

	virtual void PrintKey(const Key u) const
	{
	}
	
    };
    
	
    template <typename Key>
    class GraphSearch
    {
    public:
	GraphSearch(void)
	{
	    m_heap.m_lessFn     = LessFn;
	    m_heap.m_lessFnData = &m_map;
	    m_info              = NULL;
	}
	
	virtual ~GraphSearch(void)
	{
	}
	
	GraphSearchInfo<Key> *m_info;

	bool DFS(const Key start, const bool randomize, Key * const goal);
	
	bool BFS(const Key start, const bool randomize, Key * const goal);
	
	bool AStar(const Key start, const bool breakEarly, Key * const goal);
	
	void GetReversePathFromStart(const Key u, std::vector<Key> * const rpath) const
	{
	    if(!m_map.HasKey(u))
		return;
	    
	    Key p = u, v;
	    
	    do
	    {
		v = p;
		rpath->push_back(v);		
		p = m_map.GetData(v).m_parent;
	    }
	    while(!(v == p));
	}
	
	void GetPathFromStart(const Key u, std::vector<Key> * const path) const
	{
	    GetReversePathFromStart(u, path);
	    ReverseItems<Key>(path);
	}

	int GetPathLengthFromStart(const Key u) const
	{
	    if(!m_map.HasKey(u))
		return -1;
	    
	    Key p = u, v;
	    int count = 0;	    
	    do
	    {
		v = p;
		++count;
		p = m_map.GetData(v).m_parent;
	    }
	    while(!(v == p));
	    
	    return count;
	}

	double GetPathCostFromStart(const Key u) const
	{
	    if(!m_map.HasKey(u))
		return HUGE_VAL;
	    return m_map.GetData(u).m_gCost;
	}
	
	
	
    protected:
	struct Data
	{
	    Key    m_parent;
	    double m_gCost;
	    double m_hCost;
	};

	static bool LessFn(const Key u, const Key v, MapDefault<Key, Data> * const map);

	MapDefault<Key, Data>              m_map;	
	std::vector<Key>                   m_stack;
	Heap<Key, MapDefault<Key, Data>* > m_heap;
    };

    template <typename Key>
    bool GraphSearch<Key>::DFS(const Key start, const bool randomize, Key * const goal)
    {
	Key  u, v;
	Data data;
	
	std::vector<Key> edges;
	
	m_stack.clear();
	m_map.Clear();
	
	data.m_parent = start;
	m_map.Insert(start, data);
	if(m_info->IsGoal(start))
	{
	    *goal = start;	    
	    return true;
	}
	
	
	m_stack.push_back(start);
	while(!m_stack.empty())
	{
	    u = m_stack.back();
	    m_stack.pop_back();
	    
	    edges.clear();	    
	    m_info->GetOutEdges(u, &edges);
	    const int n = edges.size();

	    if(randomize)
		PermuteItems<Key>(&edges, n);
	    
	    for(int i = 0; i < n; ++i)
	    {
		v = edges[i];
		if(!m_map.HasKey(v))
		{
		    data.m_parent = u;
		    m_map.Insert(v, data);
		    if(m_info->IsGoal(v))
		    {
			*goal = v;			
			return true;
		    }
		    m_stack.push_back(v);
		}
	    }
	}

	return false;
    }


    template <typename Key>
    bool GraphSearch<Key>::BFS(const Key start, const bool randomize, Key * const goal)
    {
	Key u, v;
	Data data;	
	std::vector<Key> edges;
	std::queue<Key>  q;
	
	data.m_parent = start;	
	m_map.Clear();	
	m_map.Insert(start, data);
	if(m_info->IsGoal(start))
	{
	    *goal = start;	    
	    return true;
	}
	
	
	q.push(start);
	while(!q.empty())
	{
	    u = q.front();
	    q.pop();
	    
	    edges.clear();	    
	    m_info->GetOutEdges(u, &edges);
	    const int n = edges.size();

	    if(randomize)
		PermuteItems<Key>(&edges, n);
	    
	    for(int i = 0; i < n; ++i)
	    {
		v = edges[i];
		if(!m_map.HasKey(v))
		{
		    data.m_parent = u;		    
		    m_map.Insert(v, data);
		    if(m_info->IsGoal(v))
		    {
			*goal = v;			
			return true;
		    }
		   q.push(v);
		}
	    }
	}

	return false;
    }    
    
    template <typename Key>
    bool GraphSearch<Key>::LessFn(const Key u, const Key v, MapDefault<Key, Data> * const map)
    {
	Data datau = map->GetData(u);
	Data datav = map->GetData(v);
	
	return (datau.m_gCost + datau.m_hCost) < (datav.m_gCost + datav.m_hCost);
    }
    
	
    template <typename Key>
    bool GraphSearch<Key>::AStar(const Key start, const bool breakEarly, Key * const goal)
    {
	Key u, v;
	Data datau, datav;	
	bool hadv;	
	std::vector<Key> edges;
	std::vector<double> costs;
	
	m_map.Clear();
	m_heap.Clear();
	
	datau.m_parent= start;	
	datau.m_gCost = 0;
	datau.m_hCost = m_info->HeuristicCostToGoal(start);
	m_map.Insert(start, datau);
	m_heap.Insert(start);

	m_info->PrintKey(start);
	
	while(!m_heap.IsEmpty())
	{
	    u = m_heap.RemoveTop();
	    m_info->PrintKey(u);
	    if(m_info->IsGoal(u))
	    {
		*goal = u;	
		return true;
	    }
	    
	    edges.clear();
	    costs.clear();
	    

	    datau = m_map.GetData(u);
	    m_info->GetOutEdges(u, &edges, &costs);

	    const int n = edges.size();
	    for(int i = 0; i < n; ++i)
	    {
		v = edges[i];

		const double w_uv = costs[i]; //m_info->EdgeCost(u, v);

		datav = m_map.GetData(v, datau, &hadv);	     

		
		if(!hadv)
		{
		    datav.m_parent = u;
		    datav.m_gCost  = datau.m_gCost + w_uv;
		    datav.m_hCost  = m_info->HeuristicCostToGoal(v);
		    m_map.Insert(v, datav);
		    m_heap.Insert(v);

		    if(breakEarly && m_info->IsGoal(v))
		    {
			*goal = v;
			return true;
		    }		    
		}
		else if((datau.m_gCost + w_uv) < datav.m_gCost)
		{
		    datav.m_parent= u;
		    datav.m_gCost = datau.m_gCost + w_uv;
		    m_map.Update(v, datav);
		    m_heap.Update(v);
		}
	    }
	}
	return false;
    }
    
}

#endif
    
    
    
    







