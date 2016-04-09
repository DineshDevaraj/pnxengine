
#pragma once

class Input_t
{
	Input_t();

	int  ci; 		/* Current index */
   char ip[256];  /* Input value   */

	public :

	char gc();     /* Get char      */
	char uc();     /* Unget char    */

	int scan(int &);
	Input_t(const char *);
	void set(const char *);

};

int yyparse(void);