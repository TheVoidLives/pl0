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
void init(char *fileName);
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
	//create symbol table - new
	//call parser - new
	//cal virtual machine - pm0vm.c
}
