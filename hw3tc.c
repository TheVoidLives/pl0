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
int VM(char *filename, int printFlag);
void initializeVM(char *fileName); // Replaced init()
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
         lex_filename = argv[i+1];
      }
      else if (strcmp(argv[i], "-m") == 0) 
      {
         // NOTE: Remove. Only for debugging. -m means "next arg is pm0_file name"
         pm0_filename = argv[i+1];
      }
      else // PL0 Program Filename
      {
         // TODO: Uncomment for RELEASE functionality.
         // TODO: Remove -x and -m flags
         // lex_file = argv[i];
      }
   }

   if (lex_filename == NULL)
   {
      fprintf(stderr, "no code file found");
   }

   // Invoke Lexical Analyzer
   errCheck = lexer(lex_filename, f_l);

   if (errCheck == -1)
   {
      return -1;
   }

   // TODO: Gen. (Build) Symbol table - new
   // TODO: Call (Build) Parser
   // TODO: Build IR Register
   
   
   // Invoke Virtual Machine
   errCheck = VM(pm0_filename, f_v);
   if (errCheck == -1)
   {
      return -1;
   }
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
      fprintf(stdout, "File not found\n");
      return -1;
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
      DumpVM(PPC);
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
      fprintf(stderr, "PM0VM Warning: File not found. Assuming IR is populated...");
      return;
   }

   fprintf(stderr, "PM0VM Warning: File found. Attempting to Populate IR From File...");

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
   printf("%-4s%3d%3d%3d[%3d%3d%3d] ", parseOP(IR[i].OP), IR[i].REG, IR[i].L, IR[i].M, PC, BP, SP);

   // The STACK section (Use the other function)
   printStack(SP, BP, STACK, numAR);
   printf("\n");

   // The REGISTERS
   printf("\tRegisters:[%3d%3d%3d%3d%3d%3d%3d%3d]\n", REG[0], REG[1], REG[2], REG[3], REG[4], REG[5], REG[6], REG[7]);
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
         printf("|");
      }
   }
   else {
      //Print the lesser lexical level
      printStack(bp-1, stack[bp + 2], stack, numAR-1);
      printf("|");
   }
   //Print the stack contents - at the current level
   for (i = bp; i <= sp; i++) {
      printf("%3d ", stack[i]);
   }
}
