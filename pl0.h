// The following header serves as a reference doc. for all functionality 
// within the implementation of a PL0 compiler found within. 
//
// Authors: 
//    Manuel Govea 
//    Christian Whitted
//
// Course:
//    COP3402C - Summer 2018
//    Prof. Montagne
//

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

/* Parser Constants*/
#define MAX_SL_LENGTH 500

#define OUTPUT_FILE "out.txt"

/*Debug Flag*/
#define DEBUG 0

// COMPILER OUTPUT FILE 
FILE *output;


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
//          Parser globals          //
//----------------------------------//

// Struct representation of a Symbol within the Symbol Table
typedef struct Symbol
{
   int kind;          // const = 1, var = 2
   char name [11];    // Name of var/const (length 10. name[11] = \0)
   int value;         // Value of var/const
   int level;         // Lexicograpical level
   int address;       // Offset from base address
   int mark;          // is it accesible? 0 = yes, 1 = no
} Symbol;

// Some more global variable cus why not
TableEntry *token = NULL;     // Current Token to be Parsed
Symbol toBeInserted;          // Symbol to be inserted (SELF DOCUMENTATION FTW!)
Symbol *badSearch = NULL;     // check to determine if you found the symbol you where looking for
Symbol symbolTable[MAX_SL_LENGTH];   // Symbol Table
int lastIndexOfST = 0;     // Keeps track of the last element in the symbol table
int errHandle = 0;         // Simple Err Handle Value. 
int currLexical = 0;       // Lexical Level of the current Program.
int varLexical = 0;        // Lexical Level of the Variable we are retrieving from the symbol table
int searchLexical = 0;     // |varLexical - currlexical| --> Lexical Level to store or load a word into stack
int curInsertionOffset = 4;// M - from VM. Used to Store things into the symbol table
int currRegPos = 0;        // Current Registry. Load and store into this current position.
int currPC = 0;            // Current index of IR[] for code generation

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

// TODO: Add Function Descriptions

int parser(int printFlag);

//
int program();

//
int block();

//
int statement();

//
int condition();

// 
int expression();

//
int term();

//
int factor();

//
int addToTable(Symbol simbol);

//
Symbol *lookUp(char *symbol);

//
int gen(int OP, int REG, int L, int M);

// 
void printIR();

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
