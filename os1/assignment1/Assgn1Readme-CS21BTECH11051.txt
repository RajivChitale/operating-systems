//OS Assignment 1 Readme 
//CS21BTECH11051 Rajiv Shailesh Chitale

DESCRIPTION:
This is a code written in C to find perfect numbers from 1 to N. 
It creates K processes and uses shared memory buffers for interprocess communication.

COMPILATION:
$ gcc Assgn1Src-CS21BTECH11051.c -lm -lrt -o perfect.out

ARGUMENT:
$ ./perfect.out ipfilename.txt
Where ipfilename.txt is a text file containing the parameters N, K in the format below,
N K

INPUT:
Alternatively, if no command line argument is given, it will ask for input.
$ ./perfect.out
Enter the name of the input file: ipfilename.txt


OUTPUT:
Each child process generates a log file, OutFileX. 
It contains the results of numbers checked by the process, in the following format.
1: Not a perfect number
6: Is a perfect number

The main process generates a file, OutMain.
It contains the perfect numbers grouped by the process that found them. It has the following format,
P0: 6 28
P3: 496
(Note that the i th process is labelled as Pi and i has 0 indexing) 

The terminal will display "Completed!" when the task is completed by the program.

The following headers are required in the code:
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
