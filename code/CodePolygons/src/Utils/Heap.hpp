#ifndef ABETARE__HEAP_HPP_
#define ABETARE__HEAP_HPP_

#include "Utils/Map.hpp"
#include "Utils/Constants.hpp"
#include <vector>

namespace Abetare
{
    template <typename Key, typename LessFnData>
    class Heap
    {
    public:
        Heap(void)
	{
	}
	
	virtual ~Heap(void)
	{
	}

	typedef bool (*LessFn) (const Key , const Key , LessFnData);

	LessFn     m_lessFn;
	LessFnData m_lessFnData;

	bool IsEmpty(void) const
	{
	    return m_heap.empty();
	}

	int GetNrKeys(void) const
	{
	    return m_heap.size();
	}
		
	bool HasKey(const Key  key) const
	{
	    return m_map.HasKey(key);
	}

	int GetKeyPosition(const Key  key) const
	{
	    bool hadKey = false;

	    return m_map.GetData(key, Constants::ID_UNDEFINED, &hadKey);
	}

        void Insert(const Key  key)
	{
	    const int pos =  m_heap.size();
	    m_heap.push_back(key);
	    m_map.Insert(key, pos);
	    PrecolateUp(pos);
	}
	
 	void Update(const Key  key)
	{
	    UpdateAtPosition(m_map.GetData(key));
	}

 	void UpdateAtPosition(const int pos)
	{
	    PrecolateUp(pos);
	    PrecolateDown(pos);
	}

 	Key GetTop(void) const
	{
	    return m_heap[0];
	}

	Key GetKeyAtPosition(const int pos) const
	{
	    return m_heap[pos];
	}
	
	
	Key RemoveTop(void)
	{
	    const Key key = m_heap[0];	    
	    RemoveAtPosition(0);
	    return key;
	}

	void Remove(const Key  key)
	{
	    RemoveAtPosition(m_map.GetData(key));
	}

	void RemoveAtPosition(const int pos)
	{
	    const Key key  = m_heap[pos];
	    const int size = m_heap.size();
	    
	    m_heap[pos] = m_heap[size - 1];
	    m_heap.pop_back(); 
	    m_map.Update(m_heap[pos], pos);
	    m_map.Remove(key);
	    if(pos < size - 1)
		PrecolateDown(pos);    
	}

	void Clear(void)
	{
	    m_heap.clear();
	    m_map.Clear();
	}
	
	void Sort(Key * const keys, const int size)
	{
	    Clear();
	    for(int i = 0; i < size; i++)    
		m_heap.push_back(keys[i]);
	    for(int i = (size / 2) - 1; i >= 0; i--) 
		PrecolateDown(i);
	    for(int i = 0; i < size; i++)
		keys[i] = RemoveTop();
	}

    protected:
	void PrecolateDown(const int pos)
	{
	    const int size   = m_heap.size();
	    const Key key    = m_heap[pos];
	    int       schild = (pos + 1) << 1;
	    int       parent = pos;
	 
	    for(;schild < size; parent = schild, schild = ((schild + 1) << 1))
	    {
		if(m_lessFn(m_heap[schild - 1], m_heap[schild], m_lessFnData))
		    --schild;
		m_heap[parent] = m_heap[schild];
		m_map.Update(m_heap[parent], parent);
	    }
	    if(schild == size)
	    {
		m_heap[parent] = m_heap[schild - 1];
		m_map.Update(m_heap[parent], parent);
		parent         = schild -1;
	    }
	    if(parent != pos)
	    {
		m_heap[parent] = key;
		m_map.Update(key, parent);		
		PrecolateUp(parent);
	    }
	}

	void PrecolateUp(const int pos)
	{
	    const Key key    = m_heap[pos];
	    int       child  = pos;
	    int       parent = (child - 1) >> 1;
	    
	    for(; child > 0 && m_lessFn(key, m_heap[parent], m_lessFnData);
		child = parent, parent = (child - 1) >> 1)
	    {
		m_heap[child] = m_heap[parent];		
		m_map.Update(m_heap[child], child);
	    }   
	    if(child != pos)
	    {
		m_heap[child] = key;
		m_map.Update(key, child);
	    }
	}


    protected:
	MapDefault<Key, int> m_map;
	std::vector<Key>     m_heap;
	
    };
}

#endif
    
    
    
    







