An Implementation of PL/0 (Language/Scanner/Compiler)
=====================================================

Authors
-------
This assignment was authored by the following students:

   - Manuel Govea (ma100985)
   - Christian Whitted (ch279244)

Overview
--------

This projected is divided into 3 distinct portions, each simulating a piece 
of the PL/0 Compiler architecture. Namely:
   - Virtual Machine
   - Lexer 
   - Parser/Code Generator


Compilation Instructions
---------------------------------

This project consists of two files:
      1. hw4compiler.c
      2. pl0.h

To compile this project, use one of the following commands:

     1.     gcc hw4compiler.c
     2.     gcc -o hw4compiler hw4compiler.c

Note: The pl0.h file MUST be in the same directory as the 
      hw4compiler.c file for compilation to be successful. Attempting 
      to compile this project without the pl0.h file WILL result 
      in compilation errors.


Runtime Instructions
---------------------------------

Minimum Runtime Arguments:
      1. ./a.out lexicalInput.txt
      2. ./hw4compiler lexicalInput.txt

      Note: No output will be dislayed unless the program calls 
            the PL0 "write" function.


Example Runtime Arguments:

      1.    ./a.out -l -a -v lexicalInput.txt
      2.    ./hw4compiler -l -a -v lexicalInput.txt

      Note: Output will be displayed according to the Runtime
            Flags below.


Runtime Flags:
   
   Note: Flags may be combined in any order. Note that the arguments
   for -x and -m must come directly after their usage. (see -x / -m below).

      -l |  Prints the Source Code, Lexeme Table, and Lexeme List to stdout
            and the out.txt file (in the local directory).

      -a |  Prints the output of the parser (generated assembly code) to stdout
            and the out.txt file (in the local directory).

      -v |  Prints the Virtual Machine execution trace to stdout and the out.txt
            file (in the local directory).


Debugging Flags:

      -x |  DEBUG FLAG: Explicitly sets the Lexer input file. This flag must 
                        be thrown if the -m flag is thrown. The name of the 
                        lexical input file must follow the -x flag.

            
            Example Call: ./a.out -l -x lexicalInput.txt -a -v 

      
      -m |  DEBUG FLAG: Explicitly sets the VM input file. Used to test the VM
                  functionality independently of the Lexer and Parser. 
                  The name of the vminput file must follow the -m flag.


            Example Call: ./a.out -l -a -v -x lexicalInput.txt -m vmInput.txt 


Addendum
--------
See pl0.h for function definitions, global variable declarations, and documentation.
