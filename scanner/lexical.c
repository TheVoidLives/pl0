// Submission Date: 6-15-18
// Authors:
//       manuel govea      | ma100985
//       christian whitted | ch279244
//
// Program Purpose:
//    To implement an iteration of the Lexical Analyzer.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_LENGTH_IDENTIFIER 11
#define MAX_LENGTH_INTEGER 5
#define SIZE_ALPHABET 26
#define DEBUG 0

// Declaration of Token Types
typedef enum
{
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
   callsym,
   constsym,
   varsym,
   procsym,
   writesym,
   readsym,
   elsesym
} token_type;

// Lexeme Table Entries
typedef struct TableEntry
{
   char word[MAX_LENGTH_IDENTIFIER + 1];
   int ID;
   struct TableEntry *next;
} TableEntry;

// Keyword Trie
typedef struct TrieNode
{
   struct TrieNode *children[SIZE_ALPHABET];
   int keyValue;
} TrieNode;

/* Function Declarations */
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

/* Main */
int main(int argc, char **argv)
{
   char token[MAX_LENGTH_IDENTIFIER + 1];
   char *filename = argv[1];
   FILE *readFile, *writeFile;
   TableEntry *tableHead = NULL, *tableTail = NULL;
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
      fprintf(stdout, "Proper Syntax: .\\Lexical <inputFile>\n");
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
            token[i + 1] = '\0';
         }
         else if (tooLongFlag == 1 || commentFlag == 1)
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
            else if ((token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z'))
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
               }
               else if (!(token[i] >= '0' && token[i] <= '9'))
               {
                  if ((token[i] >= 'a' && token[i] <= 'z') ||
                      (token[i] >= 'A' && token[i] <= 'Z'))
                  {
                     extraChar = token[i];
                     while ((extraChar >= 'a' && extraChar <= 'z') ||
                            (extraChar >= 'A' && extraChar <= 'Z'))
                     {
                        extraChar = fgetc(readFile);
                     }
                     flag = 1;
                     badVarNameFlag = 1;
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
               }
            }
         }
      }

      if (tooLongFlag == 1)
      {
         if (wordFlag == 1)
         {
            handleError(26, writeFile);
         }
         else if (integerFlag == 1)
         {
            handleError(25, writeFile);
         }
      }

      if (badVarNameFlag == 1)
      {
         handleError(24, writeFile);
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

   wrapUp(tableHead, readFile, writeFile);
   return 0;
}

int invalidSymbol(char c)
{
   switch (c)
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

void handleError(int i, FILE *out)
{
   char errBuff[100];
   // Assume I is errror indication
   switch (i)
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
      return;
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

   strncpy(tmp->word, word, (MAX_LENGTH_IDENTIFIER + 1));
   tmp->ID = id;
   tmp->next = NULL;
   return tmp;
}

void initKeywords(TrieNode *head)
{
   insertKeyword(head, "const", constsym);
   insertKeyword(head, "var", varsym);
   insertKeyword(head, "procedure", procsym);
   insertKeyword(head, "call", callsym);
   insertKeyword(head, "begin", beginsym);
   insertKeyword(head, "end", endsym);
   insertKeyword(head, "if", ifsym);
   insertKeyword(head, "then", thensym);
   insertKeyword(head, "else", elsesym);
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
   while (*key)
   {
      // If the child doesnt exist add the new node
      if (tmp->children[*key - 'a'] == NULL)
      {
         tmp->children[*key - 'a'] = getTrieNode();
      }

      // Move to next node
      tmp = tmp->children[*key - 'a'];

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

   while (*key)
   {
      // Check for invalid char.
      if (*key - 'a' > 26 || *key - 'a' < 0)
      {
         return 0;
      }

      // Check for Node Existence
      if (tmp->children[*key - 'a'] == NULL)
      {
         return 0;
      }

      tmp = tmp->children[*key - 'a'];
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
   while ((fgets(buff, 500, src)))
   {
      fprintf(stdout, "%s", buff);
      fprintf(out, "%s", buff);
   }
   fprintf(stdout, "\n\n");
   fprintf(out, "\n\n");
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
         fprintf(stdout, "%s %d\n", tmp->word, tmp->ID);
         fprintf(out, "%s %d\n", tmp->word, tmp->ID);
         tmp = tmp->next;
      }
   }
   fprintf(stdout, "\n");
   fprintf(out, "\n");
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
         if (tmp->ID == 2 || tmp->ID == 3)
         {
            fprintf(stdout, "%d %s ", tmp->ID, tmp->word);
            fprintf(out, "%d %s ", tmp->ID, tmp->word);
         }
         else
         {
            fprintf(stdout, "%d ", tmp->ID);
            fprintf(out, "%d ", tmp->ID);
         }
         tmp = tmp->next;
      }
      fprintf(stdout, "\n");
      fprintf(out, "\n");
   }
}
