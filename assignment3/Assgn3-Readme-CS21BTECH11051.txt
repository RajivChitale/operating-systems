//OS2 Coding Assignment 3 README 
//CS21BTECH11051 Rajiv Shailesh Chitale

DESCRIPTION:
These are C++ codes to implement and compare TAS, CAS and bounded waiting with CAS.

COMPILATION AND EXECUTION:

g++ Assgn3-Src-tas-CS21BTECH11051.cpp -lpthread -o tas.out
./tas.out

g++ Assgn3-Src-cas-CS21BTECH11051.cpp -lpthread -o cas.out
./cas.out

g++ Assgn3-Src-cas-bounded-CS21BTECH11051.cpp -lpthread -o cas-bounded.out
./cas-bounded.out

INPUT:
Input is read from a file "inp-params.txt" containing the parameters:
n k l1 l2
Where n is number of threads, each entering critical section k times. 
l1 and l2 are average times spent in critical and remainder section used to generate exponential distribution.

OUTPUT:
The logs are written to the files "output-tas.txt", "output-cas.txt" and "output-cas-bounded.txt".
These include the time of request, entry and exit for each thread and iteration.
On completion, the average waiting time and total waiting time are printed to the terminal.


The following headers are required in the code:
#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>
#include <atomic>
#include <unistd.h>
#include <random>
#include <sys/time.h>
