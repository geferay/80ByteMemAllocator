OVERVIEW:
The 80ByteMemAllocator is a simple demo program for allocating and doing basic
memory management.
There are just a few basic command.
All commands are 1 character, and all "program" names are 1 character.


COMMANDS:
* S: Show memory
* C: Compact memory
* F p: Free memory of program p
* A p n a: Allocate memory for program p with n bytes, using algorithm a
  n is an integer.
  Algorithms are:
  - F: First fit
  - B: Best fit
  - W: Worst fit
* R <fileName>: Read a file with name fileName.
  You can nest file reads inside files, too.
  The file is just a list of the same commands as from standard input.


EXAMPLES:
A M 20 F - This will allocate 20 bytes for program M, using best fit algorithm.
S        - This will show the memory.
C        - This will compact the memory, if there are gaps.
F M      - This will free ALL memory allocations for program M.
R testScript.txt - This will run the commands from the file testScript.txt.


BUGS:
There is at least 1 bug. The worst fit memory alloction algorithm will not
allocate the last space in the memory space, at least for size 1 allocations.
This is likely true for larger allocations at the end. And, the other memory
allocation algorithms likely have similar bugs.
There are also probably other edge-cases that are unhandled.
