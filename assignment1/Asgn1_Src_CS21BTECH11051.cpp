/*
CS21BTECH11051  
Rajiv Shailesh Chitale

OS2 Assignment 1
Estimating pi by parallel monte carlo method, using pthreads for multithreading.

Compilation:
g++ Asgn1_Src_CS21BTECH11051.cpp -lpthread -o pigen.out
./pigen.out

Input: it is from a file "inp.txt" containing
n k
where n is total number of points and k is number of threads

To read input from another file, pass its name as the first argument.
For no logs use -nl as flag at the end: (required for large n such as 1 billion)
./pigen.out sample.txt -nl
*/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <pthread.h>
#include <ctime>
#include <cmath>
#include <chrono>

typedef long long ll;
using namespace std;
using namespace chrono;

//coordinates of a point
typedef struct
{
    double x;
    double y;
} Point;

//details of a thread
typedef struct 
{
	pthread_t tid;		 // id of pthread
	pthread_attr_t attr; // attributes of a pthread
} RunnerThread;

// communication via global variables
ll k = 4;   //number of threads
ll n = 100000;  //combined number of points
ll* inCircleCount;    // global variable with count of points inside circle, for each thread
ll* inSquareCount;    // global variable with count of points inside square, for each thread
Point** circlePoints; // array of k arrays: used for log of points inside circle
Point** squarePoints; // array of k arrays: used for log of points inside square

bool createLog = true; //generates logs if true


//1. function run by each thread to generate points in a 2x2 square
void* runner(void* param)
{
    ll tindex = (ll) param;
    unsigned int seed = time(0) + (unsigned int) tindex; //seed for random number generation 

    // Suppose n points have labels p from [0, n-1]
    // each thread covers the set {p | p%k = tindex}
    // this achieves a balanced load of nearly n/k while summing to exactly n points
    ll total = 1 + floor((n - 1 - tindex) / k);

    if(n<k){total = tindex < n ? 1 : 0 ;} //edge case: n<k
    inSquareCount[tindex] = total;

    if(createLog)
    {
        circlePoints[tindex] = new Point[total];
        squarePoints[tindex] = new Point[total];
    }

    ll in = 0;
    double x = 0, y = 0;

    for (ll i = 0; i < total; i++)
    {
        x = 2 * (double)rand_r(&seed) / (double)RAND_MAX - 1;
        y = 2 * (double)rand_r(&seed) / (double)RAND_MAX - 1;

        if(!createLog)
        {
            if (x*x + y*y < 1){in++;} //if no logs
            continue;
        }

        if (x*x + y*y < 1)
        {
            circlePoints[tindex][in].x = x; //save points inside circle
            circlePoints[tindex][in].y = y;
            in++;
        }
        squarePoints[tindex][i].x = x;  //save points inside square
        squarePoints[tindex][i].y = y;
    }

    inCircleCount[tindex] = in;
    return (void*) in; //return value not used later. (Global variables are used as per question)
}

//2. reads input n and k from ipfile
int input(string ipfile)
{
    ifstream fin;
    fin.open(ipfile);
	if (!fin)
	{
		cout << "Input could not be read."<< endl; // for example, file does not exist
		return -1;
	}

    fin>>n>>k;      //inputs
    fin.close();
    return 0;
}

//3. produces output including: time taken, value of pi, and log of points 
void output(double pi, ll microsecs)
{
    string outfile = "output.txt";
    ofstream fout;
    fout.open(outfile);

    fout << "Time: " << microsecs << " µs" << endl;
    fout << endl<< "Value Computed: " << pi << endl ;

    if(!createLog) {fout.close(); return;} //if no logging
    fout << endl << "Log:"<< endl;

    for (ll tindex = 0; tindex < k; tindex++)
    {
        fout << endl<< "Thread" << tindex+1 << ": ";
        fout << inSquareCount[tindex] << " points inside the square, " << inCircleCount[tindex] << " points inside the circle" <<endl;

        fout << "Points inside the square: ";
        for(ll i=0; i<inSquareCount[tindex]; i++)
        {
            if(i!=0){fout<<", ";}
            fout<< "(" << squarePoints[tindex][i].x << ", " << squarePoints[tindex][i].y <<  ")";
        }

        fout << endl<< "Points inside the circle: ";
        for(ll i=0; i< inCircleCount[tindex]; i++)
        {
            if(i!=0){fout<<", ";}
            fout<< "(" << circlePoints[tindex][i].x << ", " << circlePoints[tindex][i].y <<  ")";
        }
        fout<< endl;
        
        if(createLog)
        {
            delete [] circlePoints[tindex];
            delete [] squarePoints[tindex];
        }
    }

    fout.close();
}

//4. creates and joins threads. Then it determines pi
void piFinder() 
{
    auto startTime = high_resolution_clock::now();      //start measuring time

    //initialize the size of global arrays based on number of threads
    inCircleCount = new ll[k];
    inSquareCount = new ll[k];
    circlePoints = new Point*[k];
    squarePoints = new Point*[k];

    RunnerThread worker[k]; //local array with thread details

    //create threads
    for (ll tindex = 0; tindex < k; tindex++)
    {
        ll param = tindex;     //later typecasted to void*
		pthread_attr_init(&worker[tindex].attr);
	    pthread_create(&worker[tindex].tid, &worker[tindex].attr, runner, (void*) param); 
    }

    //join threads
    for (ll tindex = 0; tindex < k; tindex++)
    {
		pthread_join(worker[tindex].tid, NULL);  //here, receiving nothing as return. 
    }

    //generate result
     ll sum = 0;
    for (ll tindex = 0; tindex < k; tindex++)
    {
        sum += inCircleCount[tindex];
    }

    double pi = 4 * (double)sum / (double)n;

    /// end calculations and proceed to output/logging
    auto endTime = high_resolution_clock::now();
    auto elapsedTime = duration_cast<microseconds>(endTime - startTime);
    ll microsecs = (ll) elapsedTime.count();

    output( pi, microsecs );

    delete [] inCircleCount; 
    delete [] inSquareCount; 
    delete [] circlePoints; 
    delete [] squarePoints;
}


int main(int argc, char* argv[])
{
    string ipfile = "inp.txt"; //default input file
	if (argc >= 2)
	{
        string lastarg(argv[argc-1]);
        if(lastarg=="-nl"){createLog = false;} //flag for no-logs 

        string temp(argv[1]);
        if(temp!="-nl"){ipfile = temp;} // command line argument if any
	}

    if( input(ipfile) == -1){return -1;}

    piFinder();
    cout<< "Output written to output.txt"<<endl;

    return 0;
}
