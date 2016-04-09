
#include <cstdio>
#include <string.h>
#include <stdlib.h>

#include "interpreter.h"

typedef std::deque<Strdeq_t> Deqdeq_t;
typedef Deqdeq_t::iterator Deqdeqitr_t;

static int lindex(const char *s) /* last index */
{
   int i=0;
   while(s[i+1]) i++;
   return i;
}

static char * strrev(const char *s, char *r)
{
    int j = 0;
    int i = lindex(s);

    for(char ch = 0; i > j; )
    {
        r[j] = s[i];
        r[i] = s[j];
        i--; j++;
    }

    return r;
}

static Str repeat_string(const char *s, const int l)
{
   int ol = 0; 		 	/* output length */
   char rs[32] = {};
   char op[256] = {}; 	/* output 			*/
   bool in = (l < 0); 	/* is negative 	*/
   bool fr = (-1 == l);	/* reverse flag 	*/

   strrev(s, rs);
	int a = abs(l);
   for(int i = 0; i < a; i++)
   {
      ol += sprintf(op + ol, "%s", fr?rs:s);
      if(in) fr = !fr;
   }

   return Str(op);
}

#ifdef DEBUG
static void print_deque(Strdeq_t &sd)
{
	Strdeqitr_t si = sd.begin();
	for( ; si != sd.end(); si++)
		printf("%s\n", (*si).data());
	return;
}

static void print_deqdeq(Deqdeq_t &dd)
{
	Deqdeqitr_t di = dd.begin();
	for( ; di != dd.end(); di++)
	{
		print_deque(*di);
		printf("\n");
	}
    printf("\n");
	return;
}
#else
  #define print_deque(arg) 
  #define print_deqdeq(arg)
#endif

Intdeq_t get_series(const RTree_t *nptr)
{
   switch(nptr->ntype)
   {
      case Nodetype::Range : 
		{
			Intdeq_t rd;
			int I = nptr->nval.from;
			for( ; I <= nptr->nval.thru; I++)
			{
				rd.push_back(I);
			}
			return rd;
		}
      case Nodetype::Series : return *(nptr->nval.pser);
      case Nodetype::Integer : 
		{
			Intdeq_t id;
			id.push_back(nptr->nval.cint);
			return id;
		}
   }
}

Strdeq_t repeat_rclass(const RTree_t *nptr)
{
   Strdeq_t opsd; /* output string deque */
   Intdeq_t ideq = get_series(nptr); /* next rule string deque */
   Strdeq_t nrsd = rule_interpreter(nptr->next);
   Intdeqitr_t i = ideq.begin();
   print_deque(nrsd);
   for( ; i != ideq.end(); ++i)
   {
      Strdeqitr_t s = nrsd.begin();
      for( ; s != nrsd.end(); ++s)
         opsd.push_back(repeat_string((*s).data(), *i));
   }
   return opsd;
}

Strdeq_t constant_rclass(const RTree_t *nptr)
{
   char ca[32];
   Strdeq_t opsd;
   Intdeq_t ideq = get_series(nptr);
   Intdeqitr_t i = ideq.begin();
   for( ; i != ideq.end(); ++i)
   {
      sprintf(ca, "%d", *i);
      opsd.push_back(ca);
   }
   return opsd;
}

static char * arithmetic_progression(int a, int d, int l)
{
	int sl = 0;
	static char sap[32] = {};	/* arithmetic progression as string */
	for(int I = 0; I < l; I++)
	{
		sl += sprintf(sap + sl, "%d", abs(a));
		 a += d;
	}
	return sap;
}

static void arithprog_splitter(Strdeq_t &opsd, int a, int d, int l)
{
	char sb[32] = {};	/* string buffer */
	for(int I = 0; I < l; I++)
	{
		sprintf(sb, "%d", abs(a));
		opsd.push_back(sb);
		a+=d;
	}
}

Strdeq_t arithprog_rclass(const RTree_t *nptr)
{
	Strdeq_t opsd;
	const RTree_t *t = nptr;
Intdeq_t sd;
	t = t->child; 						/* temp pointer */
	if(Ruletype::Splitter == t->rtype)
	{
		Strdeq_t str_deq = arithprog_rclass(t);
		Strdeqitr_t sdi = str_deq.begin();
		for( ; sdi != str_deq.end(); ++sdi)
			sd.push_back(atoi((*sdi).data()));
	}
	else sd = get_series(t);	/* starting number deque */
	t = t->next;
	Intdeq_t dd = get_series(t);	/* difference number deque */
	t = t->next;
	Intdeq_t ld = get_series(t);	/* length deque */

	Intdeqitr_t li = ld.begin();	/* length iterator */
	for( ; li != ld.end(); ++li)
	{
		/* If length is zero there is nothing todo */
		if(0 == *li) continue;

		/* If length is one the difference has no significance
			and all the elements in the stating number deque
			should be added 											 */
		if(1 == *li)
		{
			/* string buffer */
			char sb[32] = {};
			Intdeqitr_t si = sd.begin();
			for( ; si != sd.end(); ++si)
			{
				sprintf(sb, "%d", *si);
				opsd.push_back(sb);
			}
			continue;
		}

		Intdeqitr_t di = dd.begin();
		for( ; di != dd.end(); ++di)
		{
			Intdeqitr_t si = sd.begin(); /* starting number interator */
			for( ; si != sd.end(); ++si)
			{
				if(Ruletype::Splitter == nptr->rtype)
					arithprog_splitter(opsd, *si, *di, *li);
				else opsd.push_back(arithmetic_progression(*si, *di, *li));
			}
		}
	}

	return opsd;
}

static void genrate_premnum(Deqdeq_t &d2, Strdeq_t &opsd,
									 int s = 0, const char *p = "")
{
	Str po(p);
	Deqdeqitr_t d2i = d2.begin() + s;

	if(d2.end() == d2i)
	{
		return opsd.push_back(po);
	}

	Str pc; /* prefix copy */
	Strdeqitr_t sdi = (*d2i).begin();	/* string deque itrator */
	for( ; sdi != (*d2i).end(); ++sdi)
	{
		pc = po;
		po += *sdi;
		genrate_premnum(d2, opsd, s+1, po.data());
		po = pc;
	}

	return;
}

Strdeq_t rule_interpreter(const RTree_t *nptr)
{
   Deqdeq_t d2; 	/* deque of deque */
   Strdeq_t sd;
   Strdeq_t opsd;
	
   while(nptr)
   {
	   switch(nptr->rtype)
	   {
		   case Ruletype::Repeat : 
		   {
			   sd = repeat_rclass(nptr);
			   nptr = nptr->next;
			   d2.push_back(sd);
			   break;
		   }

		   case Ruletype::Constant : 
		   {
			   sd = constant_rclass(nptr);
			   d2.push_back(sd);
			   break;
		   }

		   case Ruletype::Splitter	:
		   case Ruletype::Arithprog : 
		   {
			   sd = arithprog_rclass(nptr);
			   d2.push_back(sd);
			   break;
		   }
			
		   case Ruletype::Group : if(nptr->child)
		   {
			   sd = rule_interpreter(nptr->child);
			   d2.push_back(sd);
			   break;
		   }

	   } /* end of switch nptr->rtype */

	   nptr = nptr->next;

   } /* end of while nptr */

   genrate_premnum(d2, opsd);
   //print_deqdeq(d2);
   return opsd;
}
