
#include <cstdio>
#include <stdlib.h>
#include <algorithm>

#include "rtree.h"

RTree_t::RTree_t(int cint)
{
	ntype = Nodetype::Integer;
	child = next = last = 0;
	nval.cint = cint;
}

RTree_t::RTree_t(Nodetype_t aNtype, int val1, int val2)
{
	ntype = aNtype;
	child = next = last = 0;
	if(Nodetype::Range == ntype)
	{
		nval.from = val1;
		nval.thru = val2;
	}
	else if(Nodetype::Series == ntype)
	{
		nval.pser = new Intdeq_t;
		nval.pser->push_back(val1);
		/* avoid duplicate values in the list */
		if(val1 != val2) nval.pser->push_back(val2);
	}
	else
	{
		/* unhandled node type */
	}
}

void RTree_t::append_series(int val)
{
	Intdeqitr_t b = nval.pser->begin();
	Intdeqitr_t e = nval.pser->end();
	/* to avoid duplicate in teh series add
	   only if the number doesn't exist already */
	if(e == std::find(b, e, val))
		nval.pser->push_back(val);
}

RTree_t::RTree_t(RTree_t *aChild)
{
	ntype = Nodetype::Rule;
	rtype = Ruletype::Group;
	next	= last = 0;
	child = aChild;
}

void RTree_t::push_next(RTree_t *aNext)
{
   if(!next) next = aNext; 
   else last->next = aNext;
   last = aNext;
}

void RTree_t::print_tree(int pl)
{
		print_node(pl);
		
		if(child) 
			child->print_tree(pl+1);
		if(next) 
			next->print_tree(pl);
}

void RTree_t::print_node(int pl)
{
	char padl[64] = {}; /* padding left */
	
	printf("%*c\n", pl++*3+1, '{');
	sprintf(padl, "%*c", pl * 3, ' ');
	
	printf("%sThis  : 0x%p\n", padl, this);
	printf("%sNext  : 0x%p\n", padl, next);
    printf("%sChild : 0x%p\n", padl, child);
	
	if(Nodetype::Rule == ntype)
	{
		printf("%sNType : (%d) Rule\n", padl, ntype);
		printf("%sNVal  : (%c) - NA -\n", padl, '?');
	}
   else if(Nodetype::Range == ntype)
	{
		printf("%sNType : (%d) Range\n", padl, ntype);
		int ln = abs(nval.thru - nval.from) + 1;
		printf("%sNVal  : (%d) %d - %d\n", padl, ln, 
										nval.from, nval.thru);
	}
	else if(Nodetype::Series == ntype)
	{
		printf("%sNType : (%d) Series\n", padl, ntype);
		printf("%sNVal  : (%d) ", padl, nval.pser->size());
		Intdeqitr_t it = nval.pser->begin();
		for( ; it != nval.pser->end(); it++)
			printf("%d ", *it);
		printf("\n");
	}
   else if(Nodetype::Integer == ntype)
   {
	  printf("%sNType : (%d) Integer\n", padl, ntype);
	  printf("%sNVal  : (1) %d\n", padl, nval.cint);
   }
	else
	{
		printf("%sNType : (?) Invalid\n", padl);
	}
	
	if(Ruletype::Group == rtype)
		printf("%sRType : (%d) Group\n", padl, rtype);
	else if(Ruletype::Repeat == rtype)
		printf("%sRType : (%d) Repeat\n", padl, rtype);
	else if(Ruletype::Constant == rtype)
		printf("%sRType : (%d) Constant\n", padl, rtype);
	else if(Ruletype::Arithprog == rtype)
		printf("%sRType : (%d) Progress\n", padl, rtype);
	else printf("%sRType : (?) Invalid\n", padl);
	
	printf("%*c\n", --pl*3+1 , '}');
	fflush(stdout);
}
