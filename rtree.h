
/* RTree : Rule Tree */

#pragma once

#include <deque>

typedef std::deque<int> Intdeq_t;
typedef Intdeq_t::iterator Intdeqitr_t;

#ifdef DEBUG
  #define debug_print(...) printf(__VA_ARGS__)
#else
  #define debug_print(...)
#endif

struct Nodetype
{
   enum Values
   {
      Rule,
      Range,
      Series,
      Integer
   };
};

typedef Nodetype::Values Nodetype_t;

union Nodevalue_t
{
   int cint; /* constant integer */
             /* should be used for node type Integer */

	struct
	{
		int from; /* first value of range */
		int thru; /*  last value of range */
					 /* should be used for node type Range */
	};

   /* pointer to number series */
   /* should be used for node type Series */
   Intdeq_t *pser;
};

struct Ruletype
{
	enum Values
	{
		Group,
		Repeat,
		Constant,
		Splitter,
		Arithprog
	};
};

typedef Ruletype::Values Ruletype_t;

struct RTree_t; /* forward declaration */

struct RTree_t
{
   RTree_t *last;
   RTree_t *next;
   RTree_t *child;
   Nodetype_t ntype;
	Ruletype_t rtype;
   Nodevalue_t nval;

	/**
	 * Initialize constant integer values
	 **/
	RTree_t(int cint);

	/**
	 * Initialize completly resolved series and range
	 **/
	RTree_t(Nodetype_t , int, int);

	void append_series(int);

	/**
	 * Initialize premium number rule (pnrule)
	 **/
	RTree_t(RTree_t *);

	/**
	 * pl : Left padding length
	 **/
	void print_tree(int pl = 0);

	/**
	 * pl : Left padding length
	 **/
	void print_node(int pl = 0);

   void push_next(RTree_t *aNext);
};
