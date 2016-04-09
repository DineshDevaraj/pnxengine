
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "rtree.h"
#include "pnlex.h"
#include "pnrule.tab.h"
#include "interpreter.h"

extern Input_t ip;
extern RTree_t *np;
extern char errmsg[];

static void ltrim(char *s)
{
	int I = 0;
	while(' ' == s[I] or 
			'\t' == s[I] or 
			'\r' == s[I] or
			'\n' == s[I]) I++;
	strcpy(s, s+I);
	return;
}

static void print_premium_number(RTree_t *nptr, FILE *fhop)
{
    #ifdef DEBUG
	   nptr->print_tree();
    #endif

	/* premium number deque */
	Strdeq_t pnd = rule_interpreter(nptr);
	Strdeqitr_t pni = pnd.begin();
	for( ; pni != pnd.end(); pni++)
		fprintf(fhop, "%s\n", (*pni).data());
	return;
}

static int process_inputline(char *ips, FILE *fhop)
{
	/* last index */
	int l = strlen(ips) - 1; 
	if('\n' == ips[l]) ips[l] = 0;
	
	fprintf(fhop, "Processign rule : %s", ips);
	
	ip.set(ips);
	int r = yyparse();

	if(r)
	{
		fprintf(fhop, " - rule invalid - %s\n", errmsg);
	}
	else 
	{ 
		fprintf(fhop, "\n");
		print_premium_number(np, fhop);
	}
	
	return r;
}

static int validate_argument(int argc, char *argv[], FILE * &fhip, FILE * &fhop)
{
	int I = 0;
	for(I = 1; I < argc; I++)
	{
		if('-' != argv[I][0])
			break;
		if(argc < I+1)
		{
			printf("Arguemt %s without value\n", argv[I]);
			return -1;
		}
		if(0 == strcmp("-f", argv[I]))
		{
			I++;
			fhip = fopen(argv[I], "r");
			if(NULL == fhip)
			{
				printf("Unable to read rules file %s : %s",
						  argv[I], strerror(errno));
				return -2;
			}
		}
		else if(0 == strcmp("-o", argv[I]))
		{
			I++;
			fhop = fopen(argv[I], "w");
			if(NULL == fhop)
			{
				printf("Unable to open output file %s : %s\n",
						  argv[I], strerror(errno));
				return -3;
			}
		}
		else
		{
			printf("Invalid flag %s\n", argv[I]);
			return -4;
		}
	}
	return I;
}

int main(int argc, char *argv[])
{
	/* interactive mode */
	if(argc < 2) for(;;)
	{
		/* Input string */
		char ips[256] = {};
		printf("Enter the rule : ");
		fflush(stdout);
		if(fgets(ips, sizeof ips, stdin))
		{
			if('q' == ips[0]) 
				return 0;
			process_inputline(ips, stdout);
		}
   }

	int  I;
	FILE *fhip = 0; /* input file handle */
	FILE *fhop = stdout; /* output file handle */

	I = validate_argument(argc, argv, fhip, fhop);
	if(I < 0) return I;

	if(!fhip)
	{
		if(argc < I+1)
		{
			printf("Neither rule nor rule file is given\n");
			return -5;
		}
		process_inputline(argv[I], fhop);
	}
	else
	{
		/* Input string */
		char ips[256] = {};
		while(fgets(ips, sizeof ips, fhip))
		{
			ltrim(ips);
			/* skip empty line */
			if(0 == ips[0]) continue;
			/* skip comment line */
			if('#' == ips[0]) continue;
			
			process_inputline(ips, fhop);
		}
	}

	return 0;
}
