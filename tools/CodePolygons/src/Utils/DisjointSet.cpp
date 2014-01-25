#include "Utils/DisjointSet.hpp"
#include <cstdlib>

namespace Abetare
{
    DisjointSet::Elem* DisjointSet::Make(void)
    {
	Elem *elem = new Elem();
	
	elem->m_rank   = 0;
	elem->m_parent = NULL;
	++m_nrComps;

	return elem;
    }
    
    DisjointSet::Elem* DisjointSet::Find(DisjointSet::Elem * elem)
    {
	Elem *parent = NULL;
	Elem *root   = elem;
	
	while(root->m_parent != NULL)
	    root = root->m_parent;
	while(elem != root)
	{
	    parent        = elem->m_parent;
	    elem->m_parent= root;
	    elem          = parent;
	}
	return root;
    }
    
    void DisjointSet::Join(DisjointSet::Elem *  x, DisjointSet::Elem *  y)
    {
	Elem *xRoot = Find(x);
	Elem *yRoot = Find(y);
	
	if(xRoot != yRoot)
	{
	    if(xRoot->m_rank > yRoot->m_rank)
		yRoot->m_parent = xRoot;
	    else if(xRoot->m_rank < yRoot->m_rank)
		xRoot->m_parent = yRoot;
	    else 
	    {
		yRoot->m_parent = xRoot;
		++(xRoot->m_rank);
	    }
	    --m_nrComps;
	}
    }
};


