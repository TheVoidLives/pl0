#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl0.h"

typedef struct Symbol
{
   int kind;          // const = 1, var = 2
   char name [11];    // Name of var/const (length 10. name[11] = \0)
   int value;         // Value of var/const
   int level;         // Lexicograpical level
   int address;       // Offset from base address
   int mark;          // is it accesible? 0 = yes, 1 = no
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
int lookUp(char *symbol);

// Some more global variable cus why not
TableEntry *token = NULL;
Symbol toBeInserted;
Symbol symbolTable[500];
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

   if (token->ID != periodsym) 
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
         token = token->next;

         if (token->ID != identsym)
         {
            // TODO: handle error
            return -1;
         }
         strcpy(toBeInserted.name, token->word);
         token = token->next;

         if (token->ID != eqsym)
         {
            //TODO: handle error
            return -1;
         }
         token = token->next;

         if (token->ID != numbersym)
         {
            //TODO: handle error
            return -1;
         }
         toBeInserted.value = atoi(token->word);
         toBeInserted.level = varLexical;
         toBeInserted.address = curInsertionOffset++;
         toBeInserted.mark = 0;
         addToTable(toBeInserted);

         token = token->next;

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
         toBeInserted.value = 0;
         toBeInserted.level = varLexical;
         toBeInserted.address = curInsertionOffset++;
         toBeInserted.mark = 0;

         token = token->next;

      } while (token->ID == commasym);

      if (token->ID != semicolonsym) 
      {
         // TODO: handle error
         return -1;
      }
      token = token->next;
   }

   errHandle = statement();

   if (errHandle != 0)
      return errHandle;
      
   return 0;
}


int statement();

int condition();

int expression();

int term();

int factor();

int addToTable(Symbol simbol);

int lookUp(char *symbol);
