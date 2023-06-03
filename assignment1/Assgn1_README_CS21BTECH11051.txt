//OS2 Assignment 1 README 
//CS21BTECH11051 Rajiv Shailesh Chitale

DESCRIPTION:
This is a C++ code to estimate pi using the Parallel Monte Carlo Method.
It creates k threads and generates n random points.

COMPILATION AND EXECUTION:
$ g++ Asgn1_Src_CS21BTECH11051.cpp -lpthread -o pigen.out
$ ./pigen.out 

INPUT:
Input is read from a file (default is "inp.txt") containing the parameters:
n k
where n is number of random points and k is number of threads

OPTIONAL ARGUMENTS:
There are two optional arguments:
ipfile : file to be used as input instead of "inp.txt"
-nl : no logs flag. Use this when n is very large such as 10^9

$ ./pigen.out [ipfile] [-nl]

OUTPUT:
The results are written to a file "output.txt" include:
-Elapsed time in microseconds
-Computed value of pi
-Logs for each thread 1 to k containing
    count and list of points inside the square 
    count and list of points inside the circle 

The terminal will display "Output written to output.txt" when the file is ready to open.

The following headers are used by the code:
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <pthread.h>
#include <ctime>
#include <cmath>
#include <chrono>
