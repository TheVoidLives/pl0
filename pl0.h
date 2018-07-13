// The following header serves as a reference doc. for all functionality 
// within the implementation of a PL0 compiler found within. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef PL0

/* VM Constants */
#define MAX_STACK_HEIGHT 2000 // Maxumum number of ARs (Activation Records) in STACK
#define MAX_CODE_LENGTH 500   // Maximum number of Instructions held by the IR Register
#define MAX_LEXI_LEVEL 3      // Number of Lexical Levels
#define MAX_REG 16            // Max

/*Lexer Constants */
#define MAX_LENGTH_IDENTIFIER 11 // Maximum length of an ID in PL0
#define MAX_LENGTH_INTEGER 5     // Maximum length of an integer in PL0
#define SIZE_ALPHABET 26         // Size of Keyword Trie (Num. allowable values)

/*Debug Flag*/
#define DEBUG 0

//----------------------------------//
//           Lexer Globals          //
//----------------------------------//

/* Enum Declaration - Lexer 
 *    Symbolic representation of the Token types identified by the 
 *    lexical analyser.
 * */
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

// Lexeme Table - Linked List IMplementation
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


//----------------------------------//
//          parser globals          //
//----------------------------------//

// TODO: Create Parser Globals 

//----------------------------------//
//         pm/0 (vm) globals        //
//----------------------------------//

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
int REG [MAX_REG];   // Registers 0 - 15 (16)
int SP = 0;    // Stack Pointer
int BP = 1;    // Base pointer
int PC = 0;    // Program Counter
int PPC = 0;   // Previous Program Counter (printing purposes)
int Halt = 0;  // End Of Program Flag
int numAR = 0; // Number of activation records currently open


//----------------------------------//
//    lexer function declarations   //
//----------------------------------//

// Invokes the Lexical Analyzer for the passed file.
int lexer (char* filename, int printFlag);

// Returns 1 for invalid symbols. 0 otherwise.
int invalidSymbol(char c);

// Error Handler. Prints error to screen and exits PL0 Compiler.
void handleError(int i, FILE *out);

// Helper Function: Inserts a Lexeme (Word and Identifier) into the Lexeme Table
TableEntry *insertTableEntry(TableEntry *tail, char *word, int id);

// Helper Function: Returns a head node for the Keyword Trie.
TrieNode *getTrieNode();

// Helper Function: Inserts a Keyword into the Static keyword trie.
void insertKeyword(TrieNode *head, char *key, int enumValue);

// Returns the (int) token-type of a given "word" if applicable.
int searchKeyword(TrieNode *head, char *key);

// Helper Function: Initializes the Keyword Trie
void initKeywords(TrieNode *head);

// Heler Function: Calls printSource, printLexemeTable, and printList. Closes open FILE(s).
void wrapUp(TableEntry *head, FILE *src, FILE *out);

// Prints the Source Program to stdout
void printSource(FILE *src, FILE *out);

// Prints the Lexeme Table to stdout
void printLexemeTable(TableEntry *head, FILE *out);

// Prints the (Raw) Lexeme List to stdout
void printLexemeList(TableEntry *head, FILE *out);

//----------------------------------//
//   Parser Function Declarations   //
//----------------------------------//

// TODO: Create Parser Functions

//----------------------------------//
//      VM Function Declarations    //
//----------------------------------//

/* VM - Function Definitions */

// Invoke the PM/0 (PL/0) Virtual Machine
int VM(char *filename, int printFlag);

// Helper Function: Initilializes the PM/0 Virtual Machine.
//       -Initializes STACK and REG to 0
//       -Read Instructs from file if filename is not null
void initializeVM(char *fileName);

// Parses and Executes the given INSTRUCTION
void instDecode(Instruction inst);

// Prints (dumps) the current state of the VM to stdout
void DumpVM();

// Helper Function: Provided by Professor MOtagne - Base Incrementer Func.
int base(int l, int base);

// Helper Function: Given by Professor MOtagne - Prints the current state of the stack 
void printStack(int sp, int bp, int *stack, int numAR);

// Helper Function: Returns the word (string literal) given an applicalbe OP-CODE (integer)
char *parseOP(int i);

#define PL0
#endif
