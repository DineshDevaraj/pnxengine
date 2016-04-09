
%error-verbose

%{

   #include <cstdio>
   #include "rtree.h"
   #include "pnrule.tab.h"

   int yylex(void);
   int yyerror(const char *msg);

   RTree_t *np;
   extern char errmsg[];

%}

%union
{
   int ival;
   RTree_t *nptr; /* node pointer */
};

%token <ival> INT
%type  <ival> ntint
%type  <nptr> line input pnrule
%type  <nptr> value series range
%type  <nptr> repeat arithprog constant splitter

%%

line  : input        { np = $$; };

input : pnrule       
	    { 
			$$ = $1;
			debug_print("input : pnrule\n");
		}
      | input pnrule 
		{ 
			$$ = $1; 
			$1->push_next($2);
			debug_print("input : input pnrule\n");
		}
      ;


/*
 * The following two rules can be added if required in future, the
 * current RTree_t is implemented to handle recursive rules such
 * as show below. So extent the application if the below rules are
 * added in future
 *
 * '[' pnrule ']' '{' value '}'
 * '[' pnrule ':' value ':' value ']'
 *
 */

pnrule : constant
       {
            $$ = new RTree_t($1);
			debug_print("pnrule : constant\n");
       }
       | splitter
       {
            $$ = new RTree_t($1);
			debug_print("pnrule : splitter\n");
       }
	   | arithprog
	   {
            $$ = new RTree_t($1);
			debug_print("pnrule : arithprog\n");
       }
       | constant repeat
       {
            $2 ->next  = $1;
            $$ = new RTree_t($2);
			debug_print("pnrule : constant repeat\n");
       }
       | splitter repeat
       {
            $2 ->next = $1;
            $$ = new RTree_t($2);
			debug_print("pnrule : splitter repeat\n");
       }
	   | arithprog repeat
       {
            $2 ->next = $1;
            $$ = new RTree_t($2);
			debug_print("pnrule : arithprog repeat\n");
       }	
       | '(' input ')' repeat
       {
            $$ = $4;
            $4->push_next($2);
	debug_print("pnrule : ( intput ) repeat\n");
       }
	   | '[' splitter ':' value ':' value ']'
	   {
             $$ = new RTree_t($2);
             $$ ->rtype = Ruletype::Arithprog;
             $2 ->next = $4;
             $4 ->next = $6;
	debug_print("pnrule : [ splitter : value : value ]\n");
	   }
       ;


       /* non-terminal integer */
ntint    : INT      { $$ =  $1; }
         | '-' INT  { $$ = -$2; }
		 ;

       /* value with negative number support */
value  : ntint    { $$ = new RTree_t($1); }
       | range    { $$ = $1; }
       | series   { $$ = $1; }
       ;

        /* series with negative number support */
series : ntint ',' ntint    { $$ = new RTree_t(Nodetype::Series, $1, $3); }
       | series ',' ntint   { $1->append_series($3); $$ = $1; }
       ;

       /* range with negative number support */
range  : ntint '-' ntint   { $$ = new RTree_t(Nodetype::Range, $1, $3); }
       ;


/*--------------------------------------------.
 |   RClass rule class defenition             |
 |   rclass : repeat | arithprog | constant   |
 `--------------------------------------------*/

repeat    : '{' value '}'
          {
             $$ = $2;
             $2->rtype = Ruletype::Repeat;
          }
          ;

constant  : '[' value ']'
          {
             $$ = $2;
             $2->rtype = Ruletype::Constant;
          }
          ;

arithprog : '[' value ':' value ':' value ']'
          {
             $$ = new RTree_t($2);
             $$ ->rtype = Ruletype::Arithprog;
             $2 ->next = $4;
             $4 ->next = $6;
          }
          ;
			 
splitter  : '/' value ':' value ':' value '/'
          {
             $$ = new RTree_t($2);
             $$ ->rtype = Ruletype::Splitter;
             $2 ->next = $4;
             $4 ->next = $6;
          }
          ;
