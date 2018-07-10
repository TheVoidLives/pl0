#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
   nulsym = 1, identsym, numbersym, plussym, minussym,
   multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
   gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
   periodsym, becomessym, beginsym, endsym, ifsym, thensym,
   whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
   readsym , elsesym } token_type;

   

int main (int argc, char** argv) 
{
	int i;
	int f_l = 0;
	int f_a = 0;
	int f_v = 0;
	char* filename;

	for (i = 1, i < argc, i++)
	{
		if (strcmp(argv[i], "-l")) // Argument == -l
		{
			f_l = 1;
		}
		else if (strcmp(argv[i], "-a")) // Argument == -a
		{
			f_a = 1;
		}
		else if (strcmp(argv[i], "-v")) // Argument == -v
		{
			f_v = 1;
		}
		else // program filename
		{
			filename = argv[i];
		}
	}

	if (filename == NULL) 
	{
		fprintf(stderr, "no code file found", );
	}


	//call lexer
	//create symbol table
	//call parser
	//cal virtual machine
}