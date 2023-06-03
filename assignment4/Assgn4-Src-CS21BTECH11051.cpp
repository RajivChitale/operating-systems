/*
OS2 Coding Assignment 4
Jurassic Park Problem
CS21BTECH11051 / Rajiv Shailesh Chitale

g++ Assgn4-Src-CS21BTECH11051.cpp -lpthread -o Assgn4.out
./Assgn4.out
*/


#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <thread>
#include <random>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

// semaphores and global variables
sem_t availableCar;             // signals availibility of car
sem_t availablePassenger;       // signals availibility of passenger
sem_t cmutex;                   // mutex for car
sem_t pmutex;                   // mutex for passenger
sem_t* rideEnd;                 // pointer to array used to signal end of rides

int nextpid;                    //id of next available passenger
int nextcid;                    //id of next available car
long long *rideTime;            //pointer to array for loggin ride time
long long *tourTime;            //pointer to array for logging tour time

bool toursComplete = false;     //flag for completion

// input parameters
class InputParams
{
    public:
    int P;              //number of passengers
    int C;              //number of cars
    double lambdaP;     //mean wait time at museum
    double lambdaC;     //mean wait time of safari
    int k;              //number of safari rounds per passenger
};

//reads input paramters from ipfile 
int input(InputParams &params, string ipfile)
{
    ifstream fin;
    fin.open(ipfile);
    if(!fin) {
		cout << "Input could not be read."<< endl; // for example, file does not exist
		return -1;  
    }
    fin >> params.P >> params.C >> params.lambdaP >> params.lambdaC >> params.k ; 
    fin.close();
    return 0;
}

//appends string to output file (for logging)
void output(string outstr)
{
    ofstream fout;
    fout.open("output.txt", ios_base::app); //open file in append mode
    fout << outstr;  //
    fout.close();
}

//generates sample from exponential distribution with given mean
double genExpVariable(int mean, unsigned int* seed)
{ 
    double uniformVar = ((double) rand_r(seed) + 1) / ( (double)(RAND_MAX) + 2); //generate uniform random variable in range (0,1) 
    //cout << -log( 1-uniformVar ) * mean << endl;
    return -log( 1-uniformVar ) * mean;   //convert uniform to exponential random vairable
}

//returns string with system time in mm:ss.xxx where x is millisec
//also sets timestamp in microseconds if required
string getSysTime(long long* timestamp)
{
    struct timeval t;
    gettimeofday(&t, NULL);

    if(timestamp != NULL) 
        {*timestamp = t.tv_sec*1e6 + t.tv_usec;} //timestamp in microsec

    char rbuf[50];
    sprintf(rbuf, "%03ld", t.tv_usec/1000);     //for millisec

    struct tm localtm;
    localtime_r(&t.tv_sec, &localtm);
    char lbuf[50];
    strftime(lbuf, 50, "%M:%S.", &localtm);       //for mm:ss

    string str = string(lbuf) + string(rbuf);
    return str;
}

void carThread(int cid, double lambdaC)
{
    int pid;
    long long startTime, endTime;
    rideTime[cid] = 0;
    double tC;
    string outstr;
    unsigned int seed = time(0) + (unsigned int) cid;
    while(!toursComplete)
    {
        //serve passenger
        sem_wait(&cmutex);
            sem_wait(&availablePassenger);
            if(toursComplete) {sem_post(&cmutex); break;} //break if tours are completed
            pid = nextpid;                  //read pid

            outstr = "Car " + to_string(cid+1) + " accepts Passenger " +to_string(pid+1) + "'s request\n";
            output(outstr);

            nextcid = cid;                  //write cid
            sem_post(&availableCar);
        sem_post(&cmutex);

        getSysTime(&startTime);

        //sleep at safari based on lambdaC
        tC = genExpVariable(lambdaC, &seed); // tC is in millisec
        usleep(1);  //waits for 1us so that passenger's message is before car's (to match example output)
        outstr = "Car " + to_string(cid+1) + " is riding Passenger " +to_string(pid+1) + "\n";
        output(outstr);
        usleep(tC*1000-1); 

        //signal end
        getSysTime(&endTime);
        outstr = "Car " + to_string(cid+1) + " finished Passenger " +to_string(pid+1) + "'s tour\n";
        output(outstr);
        sem_post(&rideEnd[cid]);

        rideTime[cid] += endTime - startTime;
    }
}


void passengerThread(int pid, double lambdaP, int k)
{
    int cid;
    long long startTime, endTime;
    double tP;
    string outstr;
    unsigned int seed = time(0) + (unsigned int) pid + 1000;

    outstr = "Passenger " + to_string(pid+1) + " enters the museum at " + getSysTime(&startTime) +"\n";
    output(outstr);

    for(int rides=0; rides<k; rides++)
    {
        //sleep at gallery based on lambdaP
        tP = genExpVariable(lambdaP, &seed);
        usleep(tP*1000); // in millisec

        outstr = "Passenger " + to_string(pid+1) + " made a ride request at " + getSysTime(NULL) +"\n";
        output(outstr);
        //request for safari
        sem_wait(&pmutex);
            nextpid = pid;                  //write pid
            sem_post(&availablePassenger);

            sem_wait(&availableCar);
            cid = nextcid;                  //read cid
        sem_post(&pmutex);

        outstr = "Passenger " + to_string(pid+1) + " started riding at " + getSysTime(NULL) +"\n";
        output(outstr);

        //look at dinosaurs 

        //wait for ride end
        sem_wait(&rideEnd[cid]);
        outstr = "Passenger " + to_string(pid+1) + " finished riding at " + getSysTime(NULL) +"\n";
        output(outstr);
    }

    outstr = "Passenger " + to_string(pid+1) + " exits the museum at " + getSysTime(&endTime) +"\n";
    output(outstr);

    tourTime[pid] = endTime - startTime;
}


/*
Skeleton pseudocode

CAR:
cmutex{
    wait for available_passenger          //note: follow order of passenger before car
    get pid
    write cid
    signal available_car
}
sleep based on lambdaC
rideEnd[cid].signal

PASSENGER:
sleep based on lambdaP
pmutex{
    write pid
    signal available_passenger
    wait for available_car
    get cid
}
rideEnd[cid].wait
*/


int main()
{
    string outstr;
    long long totalRideTime = 0;    //cumulative ride time, in microsec
    long long totalTourTime = 0;    //cumulative time of passengers in the park, from entry to exit

    InputParams params;
    if( input(params, "inp-params.txt") == -1) {return -1;}
    ofstream fout;
    fout.open("output.txt", ios::out); 
    fout << "";     //clear file content
    fout.close();

    rideTime = new long long[params.C];
    tourTime = new long long[params.P];

    //cout << params.P << endl << params.k;
    thread passenger[params.P];
    thread car[params.C];

    //initialize semaphores
    sem_init(&availableCar, 0, 0);
    sem_init(&availablePassenger, 0, 0);
    sem_init(&cmutex, 0, 1);
    sem_init(&pmutex, 0, 1);

    rideEnd = new sem_t[params.C];
    
    // create car threads
    for(int i=0; i < params.C; i++)
    {
        sem_init(&rideEnd[i], 0, 0);
        car[i] = thread(carThread, i, params.lambdaC);
    }

    // create passenger threads
    for(int i=0; i < params.P; i++)
    {
        passenger[i] = thread(passengerThread, i, params.lambdaP, params.k);
    }

    // join passenger threads
    for(int i=0; i<params.P; i++)
    {
        passenger[i].join();
        totalTourTime += tourTime[i];
    }

    toursComplete = true;
    //fake passengers at the end. So cars can escape the wait to break out of the loop 
    for(int i=0; i < params.C; i++)
    {
        sem_post(&availablePassenger); 
    }

    // join car threads
    for(int i=0; i<params.C; i++)
    {
        sem_post(&availablePassenger); 
        car[i].join();
        sem_destroy(&rideEnd[i]);
        totalRideTime += rideTime[i];
    }

    //print average times
    cout << "Average tour time = " << (double) totalTourTime/(params.P*1000) << " ms"<< endl ;
    cout << "Average car ride time = " << (double) totalRideTime/(params.C*1000) << " ms"<< endl ;

    //destroy semaphores
    sem_destroy(&availableCar);
    sem_destroy(&availablePassenger);
    sem_destroy(&cmutex);
    sem_destroy(&pmutex);
    delete [] rideEnd;
    delete [] rideTime;
    delete [] tourTime;

    return 0;

}
