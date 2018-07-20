// Submission Date: *-**-**
// Authors:
//       manuel govea      | ma100985   
//       christian whitted | ch279244
//
// Program Purpose: 
//    PL/0 Compiler/VM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pl0.h"

//
//
// Here Be Dragons - Main Function
//
//

int main (int argc, char** argv)
{
   int i;
   int f_l = 0;
   int f_a = 0;
   int f_v = 0;
   int f_x = 0;
   char* lex_filename, *pm0_filename;
   int errCheck = 0;

   for (i = 1; i < argc; i++)
   {
      // strncmp returns 0 if s1 and s2 are equal
      if (strcmp(argv[i], "-l") == 0) // Argument == -l
      {
         f_l = 1;
      }
      else if (strcmp(argv[i], "-a") == 0) // Argument == -a
      {
         f_a = 1;
      }
      else if (strcmp(argv[i], "-v") == 0) // Argument == -v
      {
         f_v = 1;
      }
      else if (strcmp(argv[i], "-x") == 0) 
      {
         // NOTE: Only for debugging. -x means "next arg is lex_file name"
         f_x = 1;
         lex_filename = argv[i+1];
      }
      else if (strcmp(argv[i], "-m") == 0) 
      {
         // NOTE: Only for debugging. -m means "next arg is pm0_file name"
         pm0_filename = argv[i+1];
      }
      else // PL0 Program Filename
      {
         // TODO: Uncomment for RELEASE functionality.
         // TODO: Remove -x and -m flags
         if (f_x == 0)
            lex_filename = argv[i];
      }
   }

   // 
   if (lex_filename == NULL)
   {
      fprintf(stderr, "PLC Error: No code file found");
   }

   // open output file in WRITE PLUS mode
   output = fopen(OUTPUT_FILE, "w+");

   if (output == NULL) 
   {
      // TODO: HANDLE THIS GODFORSAKEN ERROR
      return -1;
   }

   // Invoke Lexical Analyzer
   errCheck = lexer(lex_filename, f_l);

   if (DEBUG)
      fprintf(stdout, "Lexer Complete - Err: %d", errCheck);

   // TODO; Handle Top Level Error Declarator
   if (errCheck != 0)
   {
      return errCheck;
   }

   if (DEBUG)
      fprintf(stdout, "ErrCheck Complete - Passed");

   // Invoke parser
   errCheck = parser(f_a);

   if (DEBUG)
      fprintf(stdout, "Parser Complete - Err: %d", errCheck);


   // TODO; Handle Top Level Error Declarator
   if (errCheck != 0) 
   {
      return errCheck;
   }

   // Invoke Virtual Machine
   errCheck = VM(pm0_filename, f_v);
   if (errCheck != 0)
   {
      return errCheck;
   }

   // TODO: CLEANUP Compiler
   fclose(output);
}

//----------------------------------//
//            Lexer Code            //
//----------------------------------//

//TODO: double check that compatibility changes didnt break the lexer code
//
// From Christian - Runs for inputLex.txt case. Haven't tested other hw2 cases. 
//

/* Main-lexer */
int lexer (char* filename, int printFlag) {
   char token[MAX_LENGTH_IDENTIFIER + 1];
   FILE *readFile;
   TrieNode *keywordHead = getTrieNode();
   int flag = 0, i;
   int integerFlag = 0;
   int wordFlag = 0;
   int commentFlag = 0;
   int goodSymbolFlag = 0;
   int goodSymbolFlagButNotG = 0;
   int KeepCharFlag = 0;
   int badSymbolFlag = 0;
   int tooLongFlag = 0;
   int iIncrementFlag = 0;
   int blankSpaceFlag = 0;
   int badVarNameFlag = 0;
   int wierdCommentFlag = 0;
   int tempID;
   char symbol;
   char extraChar;

   if (DEBUG) 
   {
      printf("\n\nEntered Main...\n");
      fflush(stdout);
   }

   readFile = fopen(filename, "r");

   if (DEBUG) 
   {
      printf("Opened Files...\n");
      fflush(stdout);
   }


   if (readFile == NULL)
   {
      fprintf(stdout, "File not found\n");
      return -1;
   }

   initKeywords(keywordHead); 

   if (DEBUG) 
   {
      printf("Initialized trie. Approaching Loop... OF DOOOM...\n");
      fflush(stdout);
   }

   while (!feof(readFile))
   {
      integerFlag = 0;
      wordFlag = 0;
      commentFlag = 0;
      goodSymbolFlag = 0;
      badSymbolFlag = 0;
      tooLongFlag = 0;
      blankSpaceFlag = 0;
      goodSymbolFlagButNotG = 0;
      KeepCharFlag = 0;
      badVarNameFlag = 0;
      wierdCommentFlag = 0;
      extraChar = '\0';


      if (DEBUG) 
      {
         printf("You've entered.... The TWILIGHT ZONE.\n");
         fflush(stdout);
      }


      for (i = 0; flag == 0; i++)
      {
         if (tooLongFlag == 0 && commentFlag == 0 && iIncrementFlag == 0)
         {
            token[i] = fgetc(readFile);
            token[i+1] = '\0';
         }
         else if ((tooLongFlag == 1 || commentFlag == 1) && wierdCommentFlag == 0)
         {
            extraChar = fgetc(readFile);
         }

         if (i == 0)
         {
            if (token[0] == '/')
            {
               extraChar = fgetc(readFile);
               if (extraChar == '*')
               {
                  commentFlag = 1;
               }
               else 
               {
                  flag = 1;
                  goodSymbolFlag = 1;
                  goodSymbolFlagButNotG = 1;
               }
            }
            else if (token[0] >= '0' && token[0] <= '9')
            {
               integerFlag = 1;
            }
            else if((token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')) 
            {
               wordFlag = 1;
            }
            else if (!invalidSymbol(token[0]))
            {
               flag = 1;
               goodSymbolFlag = 1;
               goodSymbolFlagButNotG = 1;
               token[1] = fgetc(readFile);
               token[2] = '\0';
               extraChar = token[1];
            }
            else if (invalidSymbol(token[0]))
            {
               handleError(27);
               flag = 1;
               badSymbolFlag = 1;
               return -1;
            }
            else 
            {
               flag = 1;
               blankSpaceFlag = 1;
            }

            iIncrementFlag = 0;
         }
         else 
         {
            if (integerFlag == 1) 
            {
               if (i >= MAX_LENGTH_INTEGER) 
               {
                  tooLongFlag = 1;
                  extraChar = token[i];
                  token[i] = '\0';
                  handleError(25);
                  return -1;
               }
               else if (!(token[i] >= '0' && token[i] <= '9'))
               {
                  if ((token[i] >= 'a' && token[i] <= 'z') || 
                        (token[i] >= 'A' && token[i] <= 'Z'))
                  {
                     handleError(24);
                     return -1;
                  }
                  else
                  {
                     flag = 1;
                     if (tooLongFlag == 0)
                     {
                        extraChar = token[i];
                        token[i] = '\0';
                     } 
                  }
               }
            }
            else if (wordFlag == 1)
            {
               if (i >= MAX_LENGTH_IDENTIFIER)
               {
                  tooLongFlag = 1;
                  extraChar = token[i];
                  token[i] = '\0';
                  handleError(26);
                  return -1;
               }
               else if (!((token[i] >= '0' && token[i] <= '9') ||
                        (token[i] >= 'a' && token[i] <= 'z') || 
                        (token[i] >= 'A' && token[i] <= 'Z')))
               {
                  flag = 1;
                  if (tooLongFlag == 0)
                  {
                     extraChar = token[i];
                     token[i] = '\0';
                  }
               }
            }
            else if (commentFlag == 1) 
            {
               if (extraChar == '*')
               {
                  extraChar = fgetc(readFile);
                  if (extraChar == '/')
                  {
                     extraChar = fgetc(readFile);
                     flag = 1;
                  }
                  else if (extraChar == '*') 
                  {
                     wierdCommentFlag = 1;
                  }
               }
            }
         }
      }

      if (DEBUG) 
      {
         printf("Current Token: %s\n", token);
      }


      if (integerFlag == 1) 
      {
         // insert to linked list token as word and 3 for id 
         if (tableHead == NULL)
         {
            tableHead = tableTail = insertTableEntry(tableTail, token, 3);
         }
         else
         {
            tableTail = insertTableEntry(tableTail, token, 3);
         }
      }
      else if (wordFlag == 1)
      {
         // do trie stuff insert token as word
         // and value from trie as id 
         // unless value is 0 in which case insert 2
         tempID = searchKeyword(keywordHead, token); 
         if (tempID == 0)
         {
            tempID = 2;
         }

         if (tableHead == NULL)
         {
            tableHead = tableTail = insertTableEntry(tableTail, token, tempID);
         }
         else
         {
            tableTail = insertTableEntry(tableTail, token, tempID);
         }
      }
      else if (goodSymbolFlag == 1 || goodSymbolFlagButNotG == 1)
      {
         symbol = token[0];
         // switch on token to get ID
         switch (symbol)
         {
            case '+':
               tempID = plussym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '-':
               tempID = minussym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '*':
               tempID = multsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '/':
               tempID = slashsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '(':
               tempID = lparentsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case ')':
               tempID = rparentsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '=':
               tempID = eqsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case ',':
               tempID = commasym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '.':
               tempID = periodsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break;
            case '<':
               if (token[1] == '=') 
               {
                  tempID = leqsym;
                  extraChar = fgetc(readFile);
                  KeepCharFlag = 1;
               }
               else if (token[1] == '>')
               {
                  tempID = neqsym;
                  extraChar = fgetc(readFile);
                  KeepCharFlag = 1;
               }
               else 
               {
                  tempID = lessym;
                  KeepCharFlag = 1;
                  token[1] = '\0';
               }
               break;
            case '>':
               if (token[1] == '=')
               {
                  tempID = geqsym;
                  extraChar = fgetc(readFile);
                  KeepCharFlag = 1;
               }
               else
               {
                  tempID = gtrsym;
                  KeepCharFlag = 1;
                  token[1] = '\0';
               }
               break;
            case ';':
               tempID = semicolonsym;
               KeepCharFlag = 1;
               token[1] = '\0';
               break; 
            case ':': 
               if (token[1] == '=') 
               {
                  tempID = becomessym;
                  extraChar = fgetc(readFile);
                  KeepCharFlag = 1;
               }
               else
               {
                  handleError(27);
                  KeepCharFlag = 1;
                  token[1] = '\0';
               }
               break;
         }
         // insert token as word
         if (tableHead == NULL)
         {
            tableHead = tableTail = insertTableEntry(tableTail, token, tempID);
         }
         else
         {
            tableTail = insertTableEntry(tableTail, token, tempID);
         }
      }

      if (extraChar == ' ' || extraChar == '\r' || extraChar == '\0' || extraChar == '\n' || extraChar == '\t' || blankSpaceFlag == 1)
      {
         while (extraChar == ' ' || extraChar == '\r' || extraChar == '\0' || extraChar == '\n' || extraChar == '\t' || blankSpaceFlag == 1)
         {
            blankSpaceFlag = 0;
            extraChar = fgetc(readFile);
         }
         token[0] = extraChar;
         token[1] = '\0';
         iIncrementFlag = 1;
      }
      else if ((badSymbolFlag == 1 || goodSymbolFlag == 1 || commentFlag == 1) && (goodSymbolFlagButNotG == 0 || KeepCharFlag == 0))
      {
         token[0] = '\0';
         iIncrementFlag = 0;
      }
      else 
      {
         token[0] = extraChar;
         token[1] = '\0';
         iIncrementFlag = 1;
      }
      flag = 0;
   }

   if (printFlag == 1) 
   {
      wrapUp(tableHead, readFile, output);
   }

   return 0;
}

int invalidSymbol(char c) 
{
   switch(c) 
   {
      case '$':
      case '#':
      case '&':
      case '!':
      case '_':
      case '~':
      case '`':
      case '?':
         return 1;
      default:
         return 0;
   }
}

//TODO include all the other errors
void handleError(int err)
{
   char errBuff[240];
   // Assume i is errror indication
   switch(err)
   {
      case 1:
         strncpy(errBuff, "Error: Expected \'=\' not \':=\'", 240);
         break;
      case 2:
         strncpy(errBuff, "Error: Expected number after \'=\'", 240);
         break;
      case 3:
         strncpy(errBuff, "Error: Expected \':=\' after identifier", 240);
         break;
      case 4: 
         strncpy(errBuff, "Error: Declaration (const, var, or procedure) must be followed by identifier", 240);
         break;
      case 5:
         strncpy(errBuff, "Error: Expected semicolon \';\' or comma \',\'", 240);
         break;
      case 6: 
         strncpy(errBuff, "Error: Incorrect symbole after procedure declaration", 240);
         break;
      case 7:
         strncpy(errBuff, "Error: Expected statement", 240);
         break;
      case 8:
         strncpy(errBuff, "Error: Unexpected symbol after statement in block", 240);
         break;
      case 9:
         strncpy(errBuff, "Error: Expected period \'.\'", 240);
         break;
      case 10:
         strncpy(errBuff, "Error: Expected semicolon \';\' between statements", 240);
         break;
      case 11:
         strncpy(errBuff, "Error: Undeclared identifier", 240); // You done fricked up
         break;
      case 12:
         strncpy(errBuff, "Error: Assignment to constant or procedure unallowed", 240);
         break;
      case 13: 
         strncpy(errBuff, "Error: Expected assignment operator \':=\'", 240);
         break;
      case 14: 
         strncpy(errBuff, "Error: Call must be followed by identifier.", 240);
         break;
      case 15: 
         strncpy(errBuff, "Error: Cannot Call a constant or variable.", 240);
         break;
      case 16:
         strncpy(errBuff, "Error: Expected \'then\' ", 240);
         break;
      case 17:
         strncpy(errBuff, "Error: Expected semicolon \';\' or right bracket \'}\'", 240);
         break;
      case 18:
         strncpy(errBuff, "Error: Expected \'do\'", 240);
         break;
      case 19:
         strncpy(errBuff, "Error: Unexpected symbol following statement", 240);
         break;
      case 20:
         strncpy(errBuff, "Error: Missing expected relational operator", 240);
         break;
      case 21: 
         strncpy(errBuff, "Error: Expression must not contain a procedure identifier", 240);
         break;
      case 22:
         strncpy(errBuff, "Error: Expected right-parenthesis \')\'", 240);
         break;
      case 23:
         strncpy(errBuff, "Error: The preceding factor cannot begin with this symbol", 240);
         break;
      case 24:
         strncpy(errBuff, "Error: Variable must begin with a letter.", 240);
         break;
      case 25:
         strncpy(errBuff, "Error: Number too large.", 240);
         break;
      case 26:
         strncpy(errBuff, "Error: Identifier too large.", 240);
         break;
      case 27:
         strncpy(errBuff, "Error: Invalid Symbol.", 240);
         break;
      case 28:
         strncpy(errBuff, "Error: Expected \'end\'.", 240);
         break;
      case 29:
         strncpy(errBuff, "Parser Error: Lexeme Table is null!", 240);
         break;
      case 30:
         strncpy(errBuff, "Parser Error: Exceded max size of Symbol Table.", 240);
         break;
      case 31:
         strncpy(errBuff, "Error: Variable out of scope", 240);
         break;
      case 32:
         strncpy(errBuff, "Error: Exceded max register capacity", 240);
         break;

      default: // How ???
         return;
   }

   fprintf(stdout, "%s\n", errBuff);
   fprintf(output, "%s\n", errBuff);

   fclose(output);

   // PANIC - Exit Program 
   exit(1);
}


/* Inserts a new Lexeme at the tail of the Lexeme
 * table, represented as a linked list.
 * */
TableEntry *insertTableEntry(TableEntry *tail, char *word, int id) 
{
   TableEntry *tmp = NULL;

   if (tail == NULL)
   {
      tmp = malloc(sizeof(TableEntry));
   }
   else 
   {
      tmp = malloc(sizeof(TableEntry));
      tail->next = tmp;
   }

   strncpy(tmp->word, word, (MAX_LENGTH_IDENTIFIER+1));
   tmp->ID = id;
   tmp->next = NULL;
   return tmp;
}

void initKeywords(TrieNode *head) 
{
   insertKeyword(head, "const", constsym);
   insertKeyword(head, "var", varsym);
   insertKeyword(head, "procedure", procsym);
   insertKeyword(head, "begin", beginsym);
   insertKeyword(head, "end", endsym);
   insertKeyword(head, "if", ifsym);
   insertKeyword(head, "then", thensym);
   insertKeyword(head, "while", whilesym);
   insertKeyword(head, "do", dosym);
   insertKeyword(head, "call", callsym);
   insertKeyword(head, "read", readsym);
   insertKeyword(head, "write", writesym);
   insertKeyword(head, "odd", oddsym);
   insertKeyword(head, "else", elsesym);
   if (DEBUG) 
   {
      printf("It freaking worked...?");
   }
}

TrieNode *getTrieNode()
{
   TrieNode *tmp = NULL;
   if ((tmp = malloc(sizeof(TrieNode)))) 
   {
      for (int i = 0; i < 26; i++) 
      {
         tmp->children[i] = NULL;
      }
   }
   return tmp;
}

/* Inserts keywords into thge Keyword Trie. Called 
 * only on setup.
 * */
void insertKeyword(TrieNode *head, char *key, int enumValue) 
{
   TrieNode *tmp = NULL;

   if (head == NULL || key == NULL) 
   {
      return;
   }

   tmp = head;
   while(*key)
   {
      // If the child doesnt exist add the new node
      if (tmp->children[*key-'a'] == NULL) 
      {
         tmp->children[*key-'a'] = getTrieNode();
      }

      // Move to next node
      tmp = tmp->children[*key-'a'];

      // Move String to next cha`r
      key++;
   }

   // Mark this as a keyword
   tmp->keyValue = enumValue;
}

/* Searches the KeywordTrie. If a key is found
 * to match a keyword, the corresponding symbol
 * is returned (keyvalue). 
 * */
int searchKeyword(TrieNode *head, char *key) 
{
   TrieNode *tmp = head;
   if (head == NULL || key == NULL) 
   {
      return 0;
   }

   while(*key) 
   {
      // Check for invalid char. 
      if (*key - 'a' > 26 || *key - 'a' < 0) 
      {
         return 0;
      }

      // Check for Node Existence
      if (tmp->children[*key-'a'] == NULL) 
      {
         return 0;
      }

      tmp = tmp->children[*key-'a'];
      key++;
   }

   return tmp->keyValue;
}

/* Final functional Call. Should print, cleanup, and bail. */
void wrapUp(TableEntry *head, FILE *src, FILE *out) 
{
   printSource(src, out);
   printLexemeTable(head, out);
   printLexemeList(head, out);

   // Close Source. Leave Global output file open.
   fclose(src);
}

/*Prints the original source file line by line. */
void printSource(FILE *src, FILE *out) 
{
   char buff[500];
   if (src == NULL || out == NULL) 
   {
      return;
   }
   fprintf(stdout, "Source Program:\n");
   fprintf(out, "Source Program:\n");
   rewind(src);
   while((fgets(buff, 500, src)))  
   {
      fprintf(stdout,"%s", buff);
      fprintf(out,"%s", buff);
   }
   fprintf(stdout,"\n\n");
   fprintf(out,"\n\n");
}

/* Prints the Lexeme Table indicated by HEAD. */
void printLexemeTable(TableEntry *head, FILE *out) 
{
   TableEntry *tmp = head;
   if (tmp == NULL || out == NULL) 
   {
      return;
   }
   else 
   {
      fprintf(stdout, "Lexeme Table:\nlexeme token type\n");
      fprintf(out, "Lexeme Table:\nlexeme token type\n");
      while (tmp != NULL) 
      {
         fprintf(stdout,"%s %d\n", tmp->word, tmp->ID);
         fprintf(out,"%s %d\n", tmp->word, tmp->ID);
         tmp = tmp->next;
      }
   }
   fprintf(stdout,"\n\n");
   fprintf(out,"\n\n"); 
}

/*Prints the Lexeme List indicate by HEAD */
void printLexemeList(TableEntry *head, FILE *out) 
{
   TableEntry *tmp = head;
   if (tmp == NULL || out == NULL) 
   {
      return;
   }
   else 
   {
      fprintf(stdout, "Lexeme List:\n");
      fprintf(out, "Lexeme List:\n");
      while (tmp != NULL) 
      {
         if ( tmp->ID == 2 || tmp->ID == 3)
         {
            fprintf(stdout,"%d %s ", tmp->ID, tmp->word);
            fprintf(out,"%d %s ", tmp->ID, tmp->word);
         }
         else 
         {
            fprintf(stdout,"%d ", tmp->ID);
            fprintf(out,"%d ", tmp->ID);
         }
         tmp = tmp->next;
      }
      fprintf(stdout,"\n\n");
      fprintf(out,"\n\n");
   }
}

//----------------------------------//
//           Parser Code            //
//----------------------------------//

// Note: The INPUT to the parser is the HEAD of the Lexeme 
// Table as generated by the Lexer.
int parser(int printFlag) 
{
   if (tableHead == NULL) 
   {
      handleError(29);
      return -1;
   }

   errHandle = program();

   // Return Failsafe
   if (errHandle != 0) 
   {
      return errHandle;
   }

   if (printFlag == 1) 
   {
      fprintf(stdout, "No errors encountered. Program is Syntatically correct.\n");
      fprintf(output, "No errors encountered. Program is Syntatically correct.\n");
      printIR();
   }

   return 0;
}

int program ()
{
   token = tableHead;
   errHandle = block();

   // Return Failsafe
   if (errHandle != 0) 
   {
      return errHandle;
   }

   if (token == NULL || token->ID != periodsym) 
   {
      handleError(9);
      return -1;
   }

   gen(9,0,0,3);
   return 0;
}

int block()
{
   int procPC, incrementOffset=0, startSize=lastIndexOfST;
   char funcName[1024] = "hellp";
   currLexical++;
   curInsertionOffset = 4;

   // Initial Jump | Recursion issues`
   gen(7,0,0,(currPC + 1));

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
            handleError(4);
            return -1;
         }
         strcpy(toBeInserted.name, token->word);
         token = token->next;

         if (token->ID != eqsym)
         {
            handleError(1);
            return -1;
         }
         token = token->next;

         if (token->ID != numbersym)
         {
            handleError(2);
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
         handleError(5); 
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
         //printf("b4 get next\n\tID: %d\n\tWord:%s\n", token->ID, token->word);

         token = token->next;
         //printf("after get next\n\tID: %d\n\tWord:%s\n", token->ID, token->word);

         if (token->ID != identsym)
         {

            //printf("in var\n\tID: %d\n\tWord:%s\n", token->ID, token->word);
            handleError(4);
            return -1;
         }
         strcpy(toBeInserted.name, token->word);
         toBeInserted.value = 0;
         toBeInserted.level =currLexical;
         toBeInserted.address = curInsertionOffset++;
         toBeInserted.mark = 0;
         addToTable(toBeInserted);

         token = token->next;

      } while (token->ID == commasym);

      if (token->ID != semicolonsym) 
      {
         handleError(5);
         return -1;
      }
      token = token->next;
   }

   // Initialize increment offset
   incrementOffset = (lastIndexOfST - startSize) + 4;

   if (token->ID == procsym)
   {
      //save this address
      procPC = currPC;

      toBeInserted.kind = 3;
      toBeInserted.level = currLexical;
      token = token->next;

      if (token->ID != identsym)
      {
         //TODO: handle err identifier expected
         return -1;
      }
      strcpy(toBeInserted.name, token->word);
      strcpy(funcName, token->word);

      //TODO: handle addres to start the function
      toBeInserted.address = currPC + 1;
      toBeInserted.mark = 0;
      toBeInserted.value = 0;
      
      //TODO: insert here to table??
      addToTable(toBeInserted);

      token = token->next;
      if (token->ID != semicolonsym)
      {
         //TODO: handle error semicolon expected
         return -1;
      }
      token = token->next;

      block();

      if (token->ID != semicolonsym)
      {
         // TODO: handle error expected semicolon
         return -1;
      }
      token = token->next;

      IR[procPC].M = currPC;
      updateAddress(toBeInserted.name, currPC);
   }


   // currInsertionOffset - Size of AR allocate in STACK
   gen(6, 0, 0, incrementOffset);

   errHandle = statement();

   // Return failsafe
   if (errHandle != 0)
      return errHandle;


   //printf("nameto be inserted: %s",toBeInserted.name);
   if (currLexical > 0)
   {
      gen(2,0,0,0);
      markUnusable(funcName);
   }
   currLexical--;
   return 0;
}


int statement() 
{
   // ifPC is local to the ifsym func.
   // w1PC and w2PC are local to the whilesym func.
   int ifPC, w1PC, w2PC;
   Symbol *currentSymbol = NULL;

   //printf("\n\tID: %d\n\tWord: %s", token->ID, token->word); 
   switch (token->ID) 
   {
      case identsym:
         currentSymbol = lookUp(token->word);
         token = token->next;

         if (currentSymbol == NULL) 
         {
            handleError(11);
            return -1;
         }

         if (token->ID != becomessym) 
         {
            handleError(3);
            return -1;
         }
         token = token->next;
         errHandle = expression();
         // Return Failsafe
         if (errHandle != 0) 
         {
            return errHandle;
         }

         searchLexical = abs(currLexical - currentSymbol->level);
         gen(4, currRegPos, searchLexical, currentSymbol->address);
         return 0;
      case callsym:
         // Get the procedure identifier
         token = token->next;

         if (token->ID != identsym) 
         {
            // TODO: Handle Identifier (Procedure) Error
            return -1;
         }

         currentSymbol = lookUp(token->word);
         gen(5, 0, currentSymbol->level+1, currentSymbol->address+1);

         // Move off of ident sym
         token = token->next;
         return 0;
      case beginsym:
         token = token->next;
         errHandle = statement();

         // Return Failsafe
         if (errHandle != 0) 
         {
            return errHandle;
         }

         while (token->ID == semicolonsym) 
         {
            token = token->next;
            if (token == NULL) 
            {
               handleError(28);
               return -1;
            }
            if (token->ID == periodsym || token->ID == endsym) continue;

            errHandle = statement();

            // Return Failsafe
            if (errHandle != 0) 
            {
               return errHandle;
            }
         }

         if (token->ID != endsym)
         {
            handleError(28);
            return -1;
         }
         token = token->next;
         return 0;
      case ifsym:
         token = token->next;
         errHandle = condition();

         if (errHandle != 0) 
         {
            return errHandle;
         }

         if (token->ID != thensym) 
         {
            handleError(16);
            return -1;
         }

         token = token->next; 
         ifPC = currPC;

         gen(8, currRegPos, 0, 0);
         errHandle = statement();

         // Return Failsafe
         if (errHandle != 0) 
         {
            return errHandle;
         }

         IR[ifPC].M = currPC;

         // Check for else 
         if (token->next->ID == elsesym) 
         {
            // Generate Jump
            gen(7,0,0,0);

            // Save current PC
            ifPC = currPC;

            // Get what comes after else
            token = token->next->next;

            errHandle = statement();
            IR[ifPC].M = currPC;
         }

         return 0;
      case whilesym:
         // Save Current PC for later acces 
         w1PC = currPC;
         token = token->next;
         errHandle = condition();

         // Return Failsafe
         if (errHandle != 0) 
         {
            return errHandle;
         }

         w2PC = currPC;
         gen(8, currRegPos, 0, 0);
         if (token->ID != dosym) 
         {
            handleError(18); 
            return -1;
         }
         token = token->next;
         errHandle = statement();

         // Return failsafe
         if (errHandle != 0) 
         {
            return errHandle;
         }

         gen(7, 0, 0, w1PC);
         IR[w2PC].M = currPC;
         return 0;
      case writesym:
         gen(9, currRegPos, 0, 1);
         token = token->next;
         return 0;
      case readsym:
         currRegPos++;
         gen(9, currRegPos, 0, 2);
         token = token->next;
         return 0;
      default:
         handleError(7);
         return -1; // Death to your compiler :(
   }
}


int condition()
{
   if (token->ID == oddsym)
   {
      // somehow save the id of what you are rn
      // so you can produce the instuction later
      token = token->next;
      errHandle = expression();

      // Return Failsafe
      if (errHandle != 0)
      {
         return errHandle;
      }

      // generate instruction here
      gen(15, currRegPos, currRegPos, 0);
   }
   else 
   {
      errHandle = expression();

      // Return Failsafe
      if (errHandle != 0)
      {
         return errHandle;
      }

      switch (token->ID)
      {
         // somehow save the id of what you are rn 
         // so you can produce the instuction later
         case eqsym:
            token = token->next;
            errHandle = expression();

            // Return Failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(17, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         case neqsym:

            token = token->next;
            errHandle = expression();

            // Return failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(18, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         case lessym:

            token = token->next;
            errHandle = expression();

            // Return failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(19, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         case leqsym:
            token = token->next;
            errHandle = expression();

            // Return failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(20, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         case gtrsym:
            token = token->next;
            errHandle = expression();

            // Return failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(21, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         case geqsym:

            token = token->next;
            errHandle = expression();

            // Return failsafe
            if (errHandle != 0)
            {
               return errHandle;
            }

            // generate instruction here
            gen(22, currRegPos-1, currRegPos-1, currRegPos);
            currRegPos--;
            break;
         default:

            // TODO: handle error relational operator expected
            return -1;
      }
   }
   return 0;
}

int expression()
{
   int addop;

   if (token->ID == plussym || token->ID == minussym)
   {
      addop = token->ID;
      token = token->next;

      errHandle = term();

      // Return failsafe
      if (errHandle != 0)
      {
         return errHandle;
      }

      if (addop == minussym) 
      {
         gen(10, currRegPos, currRegPos, 0);
         //token = token->next;
      }
   }
   else 
   {
      // token = token->next;
      term();

      while (token->ID == plussym || token->ID == minussym)
      {
         addop = token->ID;
         token = token->next;

         errHandle = term();

         // Return failsafe
         if (errHandle != 0)
         {
            return errHandle;
         }

         if (addop == plussym) 
         {
            gen(11, currRegPos-1,currRegPos-1,currRegPos);
            currRegPos--;
            //token = token->next;
         }
         else 
         {
            gen(12, currRegPos-1,currRegPos-1, currRegPos);
            currRegPos--;
            //token = token->next;
         }
      }
   }

   return 0;
}


int term()
{
   int mulop;
   errHandle = factor();

   // Return failsafe
   if (errHandle != 0) 
   {
      return -1;
   } 

   while (token->ID == multsym || token->ID == slashsym) 
   {
      mulop = token->ID;
      token = token->next;
      errHandle = factor();

      // Return failsafe
      if (errHandle != 0) 
      {
         return -1;
      }

      if (mulop == multsym) 
      {
         gen(13, currRegPos-1, currRegPos-1, currRegPos);
         currRegPos--;
      }
      else 
      {
         gen(14, currRegPos-1, currRegPos-1, currRegPos);
         currRegPos--;
      }
   }

   // End
   return 0;
}


int factor() 
{
   Symbol *currentSymbol = NULL;

   if (token->ID == identsym)
   {
      currentSymbol = lookUp(token->word);
      if (currentSymbol == NULL) 
      {
         handleError(11);
         return -1;
      }

      gen(3, currRegPos, searchLexical, curInsertionOffset);
   }
   else if (token->ID == numbersym)
   {
      gen(1, currRegPos, searchLexical, atoi(token->word));
   }
   else if (token->ID == lparentsym)
   {
      // Get Next Token
      token = token->next;
      expression();

      if (token->ID != rparentsym)
      {
         handleError(22);
         return -1;
      }
   }
   token = token->next;
   return 0;
}



int addToTable(Symbol symbol)
{
   lastIndexOfST++;

   if (lastIndexOfST >= MAX_SL_LENGTH) 
   {
      handleError(30);
      return -1;
   }

   symbolTable[lastIndexOfST] = symbol;
   return 0;
}


// Be carefull with error handling in this one. 
// In case of error you return NULL instead of -1
Symbol *lookUp(char *symbol)
{
   int i;

   strcpy(symbolTable[0].name, symbol);

   for (i = lastIndexOfST; i >= 0; i--)
   {
      if (strcmp(symbol, symbolTable[i].name) == 0)
      {
         if (i == 0)
         {
            handleError(11);
            return NULL;
         }
         else if (symbolTable[i].mark == 1)
         {
            handleError(31);
            return NULL;	
         }
         else
         {
            return &symbolTable[i];
         }
      }
   }
   // This should never occur.
   handleError(11);
   return NULL;
}

void updateAddress(char *name, int currPC)
{
   int i;

   strcpy(symbolTable[0].name, name);

   for (i = lastIndexOfST; i >= 0; i--)
   {
      if (strcmp(name, symbolTable[i].name) == 0)
      {
         if (i == 0)
         {
            //TODO handle error procedure not there??;

            return;
         }
         else
         {
            symbolTable[i].address = currPC;
         }
      }
   }
}

void markUnusable(char* name)
{
   int i;
   int j;

   strcpy(symbolTable[0].name, name);

   for (i = lastIndexOfST; i >= 0; i--)
   {
      if (strcmp(name, symbolTable[i].name) == 0)
      {
         if (i == 0)
         {
            //TODO handle error procedure not there??;

            return;
         }
         else
         {
            for (j = i + 1; symbolTable[j].kind != 3; j++)
            {
               symbolTable[j].mark = 1;
            }
         }
      }
   }
}

// Helper Func.
int gen(int OP, int REG, int L, int M) 
{
   // Build Instruction at current PC (according to Parser)
   if (currRegPos > MAX_REG) 
   {
      handleError(32);
      return -1;
   }

   IR[currPC].OP = OP;
   IR[currPC].REG = REG;
   IR[currPC].L = L;
   IR[currPC].M = M;

   if (OP == 1 || OP == 3) 
   {
      currRegPos++;
   }

   // Increment currPC to empty slot
   currPC++;
   return 0;
}

// Helpr Function: Prints the entirety of the Instruction Register
void printIR() 
{
   fprintf(stdout, "\nInstruction(s)\n--------------------\n");
   fprintf(output, "\nInstruction(s)\n--------------------\n");

   for (int i =0; i < currPC; i ++) 
   {
      fprintf(stdout, "%02d %02d %02d %02d\n", IR[i].OP, IR[i].REG, IR[i].L, IR[i].M);
      fprintf(output, "%02d %02d %02d %02d\n", IR[i].OP, IR[i].REG, IR[i].L, IR[i].M);
   }
   fprintf(stdout, "\n\n");
   fprintf(output, "\n\n");
}

//----------------------------------//
//              VM Code             //
//----------------------------------//

// Invoke the PM/0 (PL/0) Virtual Machine 
// Note: The Instruction Register MUST be initialized prior to running the VM
//
//
// From Christian - Runs for factorialTest.txt case. Haven't tested other hw1 cases. 
//
int VM(char *filename, int printFlag)
{
   // Initialize the Virtual Machine
   initializeVM(filename);

   // Print VM Output Header if print Flag is thrown
   if (printFlag) printf("\n OP   Rg Lx Vl[ PC BP SP]\n");

   // Halt is initialized to 0;
   while (Halt != 1)
   {
      // The Fetch cycle is executed within the parentheses of
      // Instruction Decode (e.g.; IR[PC]) where we retreive the
      // current Instruction to be executed from the IR "Register"
      //
      //The Execute Cycle takes place within the instDecode function.
      //
      // DumpVM Only serves to print the executed instruction and the appropriate.
      // Function will be deprecated.
      instDecode(IR[PC]);
      if (printFlag) DumpVM(PPC);
   }

   // TODO: Return 1 for success 0 for failure. Error Handling?
   return 1;
}

/*Initialize PM/0*/
void initializeVM(char *filename)
{
   FILE *file;

   // Replaces For-Loops
   memset(REG, 0, MAX_REG*sizeof(REG[0]));               // (1) Initialize Registers to 0
   memset(STACK, 0, MAX_STACK_HEIGHT*sizeof(STACK[0]));  // (2) Initialize Stack to 0

   // (1) Initialize instruction set if necessary
   if ((file = fopen(filename, "rw+")) == NULL)
   {
      // fprintf(stderr, "PM0VM Warning: File not found. Assuming IR is populated...");
      return;
   }

   fprintf(stderr, "PM0VM Warning: File found. Attempting to Populate IR From File...");
   memset(IR, 0, MAX_CODE_LENGTH*sizeof(IR[0]));               // (1) Initialize IR to 0

   // Read the Instructions into the IR Register (Array).
   for (int i = 0; !feof(file); i++)
   {
      fscanf(file, "%d %d %d %d", &IR[i].OP, &IR[i].REG, &IR[i].L, &IR[i].M);
   }
}


void instDecode(Instruction inst)
{
   PPC = PC;
   PC++;
   switch(inst.OP)
   {
      case 1:  // Load Constant     LIT R 0 M
         REG[inst.REG] = inst.M;
         break;
      case 2:     // Return            RTN 0 0 0
         SP = BP - 1;
         BP = STACK[(SP + 3)];
         PC = STACK[(SP + 4)];

         // decrease the number of activation records for printing purposes
         numAR--;
         break;
      case 3:     // Load to register  LOD R L M
         REG[inst.REG] = STACK[(base(inst.L, BP) + inst.M)];
         break;
      case 4:     // Store to stack    STO R L M
         STACK[(base(inst.L, BP) + inst.M)] = REG[inst.REG];
         break;
      case 5:     // Call Func         CAL 0 L M
         STACK[SP + 1] = 0;
         STACK[SP + 2] = base(inst.L, BP);
         STACK[SP + 3] = BP;
         STACK[SP + 4] = PC;
         BP = SP + 1;
         PC = inst.M;

         // increase the number of activation records for printing purposes
         numAR++;
         break;
      case 6:  // Increment         INC 0 0 M
         SP = SP + inst.M;
         break;
      case 7:  // Jump              JMP 0 0 M
         PC = inst.M;
         break;
      case 8:  // Jump if zero      JPC R 0 M
         if (REG[inst.REG] == 0)
            PC = inst.M;
         break;
      case 9:  // SIO aka the edgy one
         switch (inst.M)
         {
            case 1: // Print Reg
               printf("%d\n", REG[inst.REG]);
               break;
            case 2: // Read into REG
               scanf("%d", &REG[inst.REG]);
               break;
            case 3: // Set HALT == 1
               Halt = 1;
               break;
         }
         break;
      case 10:    // NEG   R[i] R[j] R [k]   (R[i] <- -R[j])
         REG[inst.REG] = -1 * REG[inst.L];
         break;
      case 11:    // ADD   R[i] R[j] R [k]   (R[i] <- R[j] + R[k])
         REG[inst.REG] = REG[inst.L] + REG[inst.M];
         break;
      case 12:    // SUB   R[i] R[j] R [k]   (R[i] <- R[j] - R[k])
         REG[inst.REG] = REG[inst.L] - REG[inst.M];
         break;
      case 13:    // MUL   R[i] R[j] R [k]   (R[i] <- R[j] * R[k])
         REG[inst.REG] = REG[inst.L] * REG[inst.M];
         break;
      case 14:    // DIV   R[i] R[j] R [k]   (R[i] <- R[j] / R[k])
         REG[inst.REG] = REG[inst.L] / REG[inst.M];
         break;
      case 15:    // ODD   R[i] R[j] R [k]   (R[i] <- R[i] % 2)
         REG[inst.REG] = REG[inst.L] % 2;
         break;
      case 16:    // MOD   R[i] R[j] R [k]   (R[i] <- R[j] % R[k])
         REG[inst.REG] = REG[inst.L] % REG[inst.M];
         break;
      case 17:    // EQL   R[i] R[j] R [k]   (R[i] <- R[j] == R[k])
         REG[inst.REG] = (REG[inst.L] == REG[inst.M]) ? 1 : 0;
         break;
      case 18:    // NEQ   R[i] R[j] R [k]   (R[i] <- R[j] != R[k])
         REG[inst.REG] = (REG[inst.L] != REG[inst.M]) ? 1 : 0;
         break;
      case 19:    // LSS   R[i] R[j] R [k]   (R[i] <- R[j] < R[k])
         REG[inst.REG] = (REG[inst.L] < REG[inst.M]) ? 1 : 0;
         break;
      case 20:    // LEQ   R[i] R[j] R [k]   (R[i] <- R[j] <= R[k])
         REG[inst.REG] = (REG[inst.L] <= REG[inst.M]) ? 1 : 0;
         break;
      case 21:    // GTR   R[i] R[j] R [k]   (R[i] <- R[j] > R[k])
         REG[inst.REG] = (REG[inst.L] > REG[inst.M]) ? 1 : 0;
         break;
      case 22:    // GEQ   R[i] R[j] R [k]   (R[i] <- R[j] >= R[k])
         REG[inst.REG] = (REG[inst.L] >= REG[inst.M]) ? 1 : 0;
         break;
      default:
         fprintf(stderr, "Invalid OP Code Entered.");
         break;
   }
}

/* Given by Professor Montagne - Base Incrementer  */
int base(int l, int base) // l stand for L in the instruction format
{
   int b1; //find base L levels down b1 = base;
   b1 = base;
   while (l > 0)
   {
      b1 = STACK[b1 + 1];
      l--;
   }
   return b1;
}

void DumpVM(int i)
{
   // The op code, register, lexical level, value, PC, BP, & SP
   fprintf(stdout, "%-4s%3d%3d%3d[%3d%3d%3d] ", parseOP(IR[i].OP), IR[i].REG, IR[i].L, IR[i].M, PC, BP, SP);
   fprintf(output, "%-4s%3d%3d%3d[%3d%3d%3d] ", parseOP(IR[i].OP), IR[i].REG, IR[i].L, IR[i].M, PC, BP, SP);

   // The STACK section (Use the other function)
   printStack(SP, BP, STACK, numAR);

   fprintf(stdout, "\n");
   fprintf(output, "\n");

   // The REGISTERS
   fprintf(stdout, "\tRegisters:[%3d%3d%3d%3d%3d%3d%3d%3d]\n", REG[0], REG[1], REG[2], REG[3], REG[4], REG[5], REG[6], REG[7]);
   fprintf(output, "\tRegisters:[%3d%3d%3d%3d%3d%3d%3d%3d]\n", REG[0], REG[1], REG[2], REG[3], REG[4], REG[5], REG[6], REG[7]);
}


char *parseOP(int i)
{
   switch(i)
   {
      case 1: return "LIT";
      case 2: return "RTN";
      case 3: return "LOD";
      case 4: return "STO";
      case 5: return "CAL";
      case 6: return "INC";
      case 7: return "JMP";
      case 8: return "JPC";
      case 9: return "SIO";
      case 10: return "NEG";
      case 11: return "ADD";
      case 12: return "SUB";
      case 13: return "MUL";
      case 14: return "DIV";
      case 15: return "ODD";
      case 16: return "MOD";
      case 17: return "EQL";
      case 18: return "NEQ";
      case 19: return "LSS";
      case 20: return "LEQ";
      case 21: return "GTR";
      case 22: return "GEQ";
      default:    fprintf(stderr, "Invalid OP Code Entered."); return NULL;
   }
}

/* Given by Professor Montagne - Validator  */
void printStack(int sp, int bp, int* stack, int numAR){
   int i;
   if (bp == 1) {
      if (numAR > 0) {
         fprintf(stdout, "|");
         fprintf(output, "|");
      }
   }
   else {
      //Print the lesser lexical level
      printStack(bp-1, stack[bp + 2], stack, numAR-1);
      fprintf(stdout, "|");
      fprintf(output, "|");
   }
   //Print the stack contents - at the current level
   for (i = bp; i <= sp; i++) {
      fprintf(stdout, "%3d ", stack[i]);
      fprintf(output, "%3d ", stack[i]);
   }
}
