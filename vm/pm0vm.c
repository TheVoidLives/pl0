// Submission Date: 6-4-18
// Authors:
//       manuel govea      | ma100985   
//       christian whitted | ch279244
//
// Program Purpose: 
//    To implement an iteration of the PM/0 Virtual Machine.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Constant Declaration */
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3
#define MAX_REG 16
#define DEBUG 0

/* Instruction Declaration */ 
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

// Function Definitions
void init(char *fileName);
void instDecode(Instruction inst);
void DumpVM();
int base(int l, int base);
void printStack(int sp, int bp, int *stack, int numAR);
char *parseOP(int i);
// End

int main (int argc, char **argv) 
{
   init(argv[1]);

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
      //
      instDecode(IR[PC]);
      DumpVM(PPC);
   }
} 

/*Initialize PM/0*/
void init(char *fileName)
{
   int i = 0;
   FILE *fp;

   // (1) Initialize instruction set
   if (fileName == NULL) 
   {
      printf("Incorrect Syntax: ./pm0vm <fileName>");
   }
   else 
   {
      if (DEBUG) 
      {
         printf("Initializing...\n");
      }
   }

   fp = fopen(fileName, "r");

   if (fp == NULL)
   {
      // Please for the love of god never run 
      fprintf(stdout, "File Not Found. Please try Again.\n");
   }

   if (DEBUG)
      printf("Reading File: \"%s\"...\n", fileName);

   // Read the Instructions into Memory aka the IR
   // Array. These Instructions will later be fetched 
   // from IR in the Main function.
   for (i = 0; !feof(fp); i++)
   {
      fscanf(fp, "%d %d %d %d", &IR[i].OP, &IR[i].REG, &IR[i].L, &IR[i].M);
   }

   if (DEBUG)
   {
      printf("File Read Complete.\n");
      printf("Initializing Registers....\n");
   }


   // Initialize Registers to 0
   for (i = 0; i < MAX_REG; i++)
   {
      REG[i] = 0;
   }

   if (DEBUG) 
   {
      printf("Registers Initialized.\n");
   }

   //The first line header for all the INSTRUCTION and PC, BP & SP
   printf("\n OP   Rg Lx Vl[ PC BP SP]\n");
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
