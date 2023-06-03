OS2 - Assignment 2
CS21BTECH11051 - Rajiv Shailesh Chitale

COMPILATION AND EXECUTION:

gcc Assgn2Srcpthread-cs21btech11051.c -o pthread.out -lm -lpthread
./pthread.out 

gcc Assgn2SrcOpenMp-cs21btech11051.c -o omp.out -lm -fopenmp
./omp.out 


OPTIONAL FLAGS:

-c : complete checking without early termination

-s : silent mode - does not print anything to console

-nl : no logs for each thread. Only overall result is stored to output.txt


INPUT:
Takes a file input.txt containing:
 
K N
a11 a12 a13 ... a1N
a21	a22	a23		a2N
 .	 .	 .		 .
 .	 .	 .		 .
aN1 aN2 aN3 ... aNN

Where, aij are numbers on the input sudoku grid 
K = number of threads
N = width of sudoku grid, 

OUTPUT:
Logs and results(valid or invalid, time taken) are generated in output.txt
Result is also presented to console

