// Submission Date: *-**-**
// Authors:
//       manuel govea      | ma100985   
//       christian whitted | ch279244
//
// Program Purpose: 
//    PL/0 compiler/VM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* VM Constants */
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3
#define MAX_REG 16

/*Lexer Constants */
#define MAX_LENGTH_IDENTIFIER 11
#define MAX_LENGTH_INTEGER 5
#define SIZE_ALPHABET 26

/*Debug Flag*/
#define DEBUG 0

/* Enum Declaration - Lexer */
typedef enum {
   nulsym = 1,
   identsym,
   numbersym,
   plussym,
   minussym,
   multsym,
   slashsym,
   oddsym,
   eqsym,
   neqsym,
   lessym,
   leqsym,
   gtrsym,
   geqsym,
   lparentsym,
   rparentsym,
   commasym,
   semicolonsym,
   periodsym,
   becomessym,
   beginsym,
   endsym,
   ifsym,
   thensym,
   whilesym,
   dosym,
   constsym,
   varsym,
   writesym,
   readsym
} token_type;

// Lexeme Table Entries - Lexer
typedef struct TableEntry
{
   char word[MAX_LENGTH_IDENTIFIER + 1];
   int ID;
   struct TableEntry *next;
} TableEntry;
TableEntry *tableHead = NULL, *tableTail = NULL;

// Keyword Trie - Lexer
typedef struct TrieNode
{
   struct TrieNode *children[SIZE_ALPHABET];
   int keyValue;
} TrieNode;

// Instruction - VM
typedef struct Instruction
{
   int OP;
   int REG;
   int L;
   int M;
} Instruction;

/* VM Registers & Stack */
Instruction IR [MAX_CODE_LENGTH];   // Instruction Register
int STACK [MAX_STACK_HEIGHT];       // Stack
int REG [MAX_REG];         // Registers 0 - 15 (16)
int SP = 0; // Stack Pointer
int BP = 1; // Base pointer
int PC = 0; // Program Counter
int PPC = 0; // Previous Program Counter (printing purposes)
int Halt = 0; // End Of Program Flag
int numAR = 0; // Number of activation records currently open

//----------------------------------//
//      Function Declaraitons       //
//----------------------------------//

/* Lexer - Function Declarations */
int lexer (char* filename, int printFlag);
int invalidSymbol(char c);
void handleError(int i, FILE *out);
TableEntry *insertTableEntry(TableEntry *tail, char *word, int id);
TrieNode *getTrieNode();
void insertKeyword(TrieNode *head, char *key, int enumValue);
int searchKeyword(TrieNode *head, char *key);
void initKeywords(TrieNode *head);
void wrapUp(TableEntry *head, FILE *src, FILE *out);
void printSource(FILE *src, FILE *out);
void printLexemeTable(TableEntry *head, FILE *out);
void printLexemeList(TableEntry *head, FILE *out);

/* VM - Function Definitions */
void init(char *fileName); // TODO Modify or delete this function
void instDecode(Instruction inst);
void DumpVM();
int base(int l, int base);
void printStack(int sp, int bp, int *stack, int numAR);
char *parseOP(int i);

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


	//call lexer - lexical.c
   lexer(filename, f_l);
	//create symbol table - new
	//call parser - new
	//cal virtual machine - pm0vm.c
}

//---------------------
//
// Lexer code
//
//---------------------

//TODO double check that compatibility changes didnt break the lexer code

/* Main-lexer */
int lexer (char* filename, int printFlag) {
   char token[MAX_LENGTH_IDENTIFIER + 1];
   FILE *readFile, *writeFile;
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
   writeFile = fopen("out.txt", "w+");

   if (DEBUG) 
   {
      printf("Opened Files...\n");
      fflush(stdout);
   }


   if (readFile == NULL)
   {
      fprintf(stdout, "File not found");
      return 1;
   }
   else if (writeFile == NULL)
   {
      fprintf(stdout, "Failed to create out file. Exiting...\n");
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
               token[1] = fgetc(readFile);
               token[2] = '\0';
               extraChar = token[1];
            }
            else if (invalidSymbol(token[0]))
            {
               handleError(27, writeFile);
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
                  handleError(25, writeFile);
                  return -1;
               }
               else if (!(token[i] >= '0' && token[i] <= '9'))
               {
                  if ((token[i] >= 'a' && token[i] <= 'z') || 
                      (token[i] >= 'A' && token[i] <= 'Z'))
                  {
                    handleError(24, writeFile);
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
                  handleError(26, writeFile);
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
                  handleError(27, writeFile);
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
      wrapUp(tableHead, readFile, writeFile);
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
void handleError(int i, FILE *out)
{
   char errBuff[100];
   // Assume I is errror indication
   switch(i)
   {
      case 24:
         strncpy(errBuff, "Error: Variable must begin with a letter.", 100);
         break;
      case 25:
         strncpy(errBuff, "Error: Number too large.", 100);
         break;
      case 26:
         strncpy(errBuff, "Error: Identifier too large.", 100);
         break;
      case 27:
         strncpy(errBuff, "Error: Invalid Symbol.", 100);
         break;
      default: // How ???
         return ;
   }

   fprintf(stdout, "%s\n", errBuff);
   fprintf(out, "%s\n", errBuff);
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
      insertKeyword(head, "begin", beginsym);
      insertKeyword(head, "end", endsym );
      insertKeyword(head, "if", ifsym);
      insertKeyword(head, "then", thensym);
      insertKeyword(head, "while", whilesym);
      insertKeyword(head, "do", dosym);
      insertKeyword(head, "read", readsym);
      insertKeyword(head, "write", writesym);
      insertKeyword(head, "odd", oddsym);
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

   fclose(src);
   fclose(out);
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
  fprintf(stdout,"\n");
  fprintf(out,"\n"); 
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
      fprintf(stdout,"\n");
      fprintf(out,"\n");
   }
}
