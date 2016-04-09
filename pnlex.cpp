
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "rtree.h"
#include "pnlex.h"
#include "pnrule.tab.h"

Input_t ip("");
char errmsg[256];

Input_t::Input_t(const char *val)
{
	set(val);
}

void Input_t::set(const char *val)
{
	ci = 0;
   strcpy(ip, val);
}

char Input_t::gc()
{
	return ip[ci++];
}

char Input_t::uc()
{
	return ip[--ci];
}

int Input_t::scan(int &var)
{
	sscanf(ip + ci, "%d", &var);
   while(isdigit(ip[ci])) ci++;
	return var;
}

int yylex()
{
	char ch = ip.gc();

	if('\n' == ch or 0 == ch)
		return 0;

   /* skip space characters */
	while(ch == ' ' or ch == '\t')
      ch = ip.gc();

	if(isdigit(ch))
	{
		ip.uc();
		ip.scan(yylval.ival);
		return INT;
   }

	return ch;
}

int yyerror(const char *msg)
{
	strcpy(errmsg, msg);
}