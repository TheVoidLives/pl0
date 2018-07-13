#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl0.h"

#define MAX_SL_LENGTH 500

typedef struct Symbol
{
	int kind;            // const = 1, var = 2
	char name [10];      // Name of var/const
	int val;             // Value of var/const
	int level;           // Lexicograpical level
	int address;         // Offset from base address
	int mark;            // is it accesible? 0 = yes, 1 = no
} Symbol;


/* Function declarations Parser/CG */
int program();
int block();
int statement();
int condition();
int expression();
int term();
int factor();
int addToTable(Symbol simbol);
Symbol lookUp(char *symbol);

// Some more global variable cus why not
TableEntry *token = NULL;
Symbol toBeInserted;
Symbol badSearch? = NULL; // check to determine of you found the symbol you where looking for
Symbol symbolTable[MAX_SL_LENGTH];
int lastIndexOfST = 0; // keeps track of the las element in the symbol table
int errHandle = 0;
int currLexical = 0;
int varLexical = 0;
int searchLexical = 0;
int curInsertionOffset = 4;

int main (int argc, char** argv) 
{
	errHandle = program();

	if (errHandle != 0) 
	{
		return errHandle;
	}

	return 0;
}

int program ()
{
	token = tableHead;
	errHandle = block();

	if (errHandle != 0) 
	{
		return errHandle;
	}

	if (token != periodsym) 
	{
		// TODO: handle error
		return -1;
	}

	return 0;
}

int block()
{
	curInsertionOffset = 4;

	// If the token is a constant load all 
	// constants into the Symbol Table
	if (token->ID == constsym) 
	{
		toBeInserted.kind = 1;

		do 
		{
			token = tokem->next;

			if (token->ID != identsym)
			{
				// TODO: handle error
				return -1;
			}
			strcpy(toBeInserted.name, token->word);
			token = tokem->next;

			if (token->ID != eqsym)
			{
				//TODO: handle error
				return -1;
			}
			token = token->next

			if (token->ID != numbersym)
			{
				//TODO: handle error
				return -1;
			}
			toBeInserted.Value = atoi(token->word);
			toBeInserted.level = varLexical;
			toBeInserted.address = curInsertionOffset++;
			toBeInserted.mark = 0;
			addToTable(toBeInserted);

			token = token->next

		} while (token->ID == commasym);

		if (token->ID != semicolonsym) 
		{
			// TODO: handle error
			return -1;
		}
		token = token->next;
	}

	// If the token is a var load all 
	// variables into the Symbol Table
	if (token->ID == varsym)
	{
		toBeInserted.kind = 2;
		do 
		{
			token = token->next;

			if (token->ID != identsym)
			{
				//TODO: hanlde error
				return -1;
			}
			strcpy(toBeInserted.name, token->word);
			toBeInserted.Value = 0;
			toBeInserted.level = varLexical;
			toBeInserted.address = curInsertionOffset++;
			toBeInserted.mark = 0;

			token = token->next

		} while (token->ID == commasym);

		if (token->ID != semicolonsym) 
		{
			// TODO: handle error
			return -1;
		}
		token = token->next;
	}

	statement();
}

int statement();

int condition();

int expression();

int term();

int factor();

int addToTable(Symbol simbol)
{
	lastIndexOfST++;

	if (lastIndexOfST >= MAX_SL_LENGTH) 
	{
		//TODO: handle error to many symbols!!!!
		return -1;
	}

	symbolTable[lastIndexOfST] = toBeInserted;
	return 0;
}


// Be carefull with error handling in this one. 
// In case of error you return NULL instead of -1
Symbol lookUp(char *symbol)
{
	int i;

	strcpy(symbolTable[0]->name, symbol);

	for (i = lastIndexOfST -1; i >= 0; i--)
	{
		if (strcmp(symbol, symbolTable[i]->name) == 0)
		{
			if (i == 0)
			{
				// TODO: error variable/constant not defined
				return NULL;
			}
			else if (symbolTable[i]->mark == 1)
			{
				// TODO: error variable outside of scope
				return NULL;	
			}
			else
			{
				return symbolTable[i];
			}
		}
	}
}