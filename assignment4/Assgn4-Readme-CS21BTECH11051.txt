//OS2 Coding Assignment 4 README 
//CS21BTECH11051 Rajiv Shailesh Chitale

DESCRIPTION:
This is a C++ code to synchronize between P passenger threads and C car threads in the jurassic park problem.

COMPILATION AND EXECUTION:

g++ Assgn4-Src-CS21BTECH11051.cpp -lpthread -o Assgn4.out
./Assgn4.out

INPUT:
Input is read from a file "inp-params.txt" containing the parameters:
P C lambdaP lambdaC k

Where,
P is number of passenger threads
C is the number of car threads
lambdaP is average time between rides (used to generate exponential distribution)
lambdaC is the average time taken in a car ride (used to generate exponential distribution)
k is the number of rides a passenger takes

OUTPUT:
The logs are written to the files "output.txt". These include:
Time of entry and exit of passenger into museum.
Time of a ride being requested, started and finished for each passenger.
Car accepting request, running and finishing ride.

On completion the following are printed to the terminal:
average tour time of passengers 
average ride time of cars

The following headers are required in the code:
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <thread>
#include <random>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
