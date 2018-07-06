# PM0VM.c : A P-Machine(PL/0) Implementation

======================================
 Overview
======================================

The enclosed pm0vm.c file is an implementation of a generic P-Machine 
as written by Manuel Govea and Christian Whitted. 

---------------------------------
 Authors:
---------------------------------

   - Manuel Govea      | ma100985   
   - Christian Whitted | ch279244

---------------------------------
 Compilation Instructions
---------------------------------

To compile the P-Machine, utilize the following gcc command in your 
friendly neighborhood terminal:

      gcc -o pm0vm pm0vm.c 

---------------------------------
 Running Instructions 
---------------------------------

The pm0vm implementation of the P-Machine can be invoked 
using the following command in a bash, zsh, or other
generic sh capable terminal:

   ./pm0vom <fileName>

Note: The file passed to the P-Machine is expected to be a
.txt file formatted in the following way:

   01 00 00 05

As specified in the HW1 specifications.

-------------------------------------
 Extraneous Variables & Functions
-------------------------------------
   * PPC:
      -PPC is a control variable used to print the previously 
       executed instruction. It is othewrwise unused.

   * NumAR
      -NumAR is a control variable that keeps track of how 
       many open activation records are currently on the stack. 
       Like PPC, this variable is used to print the state of 
       the virtual machine. It is otherwised unused. 

   * DumpVM(PPC) 
      -DumpVM is a debug function that serves to print the state of
       the current IR and STACK. 
