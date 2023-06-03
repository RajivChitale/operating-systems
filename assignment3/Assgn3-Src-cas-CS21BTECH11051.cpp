/*
g++ Assgn3-Src-cas-CS21BTECH11051.cpp -lpthread -o cas.out

CS21BTECH11051
RAJIV SHAILESH CHITALE
CAS
*/

#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>
#include <atomic>
#include <unistd.h>
#include <random>
#include <sys/time.h>

using namespace std;

atomic<int> lock;

bool bunchLogs = true; //if true, request, entry and exit times get printed together
int n, k, l1, l2; //global vars for parameters
int maxWaitTime=0;
long totalWaitTime=0;
// n threads, each enters critical section k times. l1 and l2 are average times for exponential distribution

//reads input paramters from ipfile 
int input(string ipfile)
{
    ifstream fin;
    fin.open(ipfile);
	if (!fin)
	{
		cout << "Input could not be read."<< endl; // for example, file does not exist
		return -1;
	}
    fin>>n>>k>>l1>>l2;   //input parameters
    fin.close();
    return 0;
}

//returns string with system time in mm:ss.xxx where x is millisec
//also sets timestamp in microseconds if required
string getSysTime(long long* timestamp)
{
    struct timeval t;
    gettimeofday(&t, NULL);

    if(timestamp != NULL) 
        {*timestamp = t.tv_sec*1e6 + t.tv_usec;} //timestamp in ms

    char rbuf[50];
    sprintf(rbuf, "%03ld", t.tv_usec/1000);

    struct tm *localtm = localtime(&t.tv_sec);
    char lbuf[50];
    strftime(lbuf, 50, "%M:%S.", localtm);

    string str = string(lbuf) + string(rbuf);
    
    return str;
}

//generates sample from exponential distribution with given mean
double genExpVariable(int mean)
{
    double uniformVar;
    do{uniformVar = (double) rand() / RAND_MAX;} //generate uniform random variable
    while(uniformVar == 0); //avoid 0 otherwise time will be infinite
    //cout << -log( uniformVar ) * mean << endl;
    return -log( uniformVar ) * mean;   //convert uniform to exponential random vairable
}

//maintains stats of maximum and total wait time
void logPerformance(long long waitTime)
{
    if(waitTime > maxWaitTime)
        {maxWaitTime = waitTime;}
    totalWaitTime += waitTime;
}

//logs string to output file
void output(string outstr)
{
    ofstream fout;
    fout.open("output-cas.txt", ios_base::app); //open file in append mode
    fout << outstr; 
    fout.close();
}


//enters and exits critical section k times
//simulates critical and remainder section runtimes using exponential distribution
//logs times of request, entry and exit from critical section
void testCS(int id)
{
    string outstr, suffix;
    long long waitStartTime, waitEndTime;
    double t1, t2;
    int zero_const = 0; 

    for(int i=1; i<k+1; i++)
    {
        if(i==1){suffix="st";}  //suffix for output purposes
        else if(i==2){suffix="nd";}
        else if(i==3){suffix="rd";}
        else {suffix="th";}

        t1 = genExpVariable(l1); //generates sample from exponential distribution
        t2 = genExpVariable(l2);

        string reqEnterTime = getSysTime(&waitStartTime); //get request time
        outstr = to_string(i) + suffix + " CS Request at " + reqEnterTime + " by thread " + to_string(id) + "\n";
        if(!bunchLogs) {output(outstr); outstr="";} //prints and empties string if bunching is off

    //Entry Section
        //NOTE: this function is slightly different in it's return values
        //it returns true if the comparision is true, false otherwise
        int k=0;
        while (!lock.compare_exchange_strong(zero_const, 1))
            {zero_const = 0;}
        
        string actEnterTime = getSysTime(&waitEndTime); //get entry time
        outstr += to_string(i) + suffix + " CS Entry at " + actEnterTime + " by thread " + to_string(id) +"\n";
        if(!bunchLogs) {output(outstr); outstr="";}

        logPerformance(waitEndTime - waitStartTime); //calculate waiting time 

    //Simulation of Critical-Section      
        usleep(t1*1000); //millisec to microsec

    //Exit Section
        lock.store(0);

        string exitTime = getSysTime(NULL); //get exit time
        outstr += to_string(i) + suffix + " CS Exit at "+ exitTime + " by thread " + to_string(id) + "\n";
        output(outstr);
        usleep(t2*1000); // Simulation of Remainder Section


    }
}

int main()
{
    srand(time(NULL));
    string filename = "inp-params.txt";
    if(input(filename) == -1) {return -1;} //read file

    ofstream fout;
    fout.open("output-cas.txt"); //clears file
    fout << "CAS ME Output:" <<endl;
    fout.close();

    lock.store(0);
    thread tester[n];
    //create n tester threads
    for(int i=0; i<n; i++)
    {
        tester[i] = thread(testCS, i+1);
    }

    for(int i=0; i<n; i++)
    {
        tester[i].join();
    }

    cout << "Average waiting time: " << (double) totalWaitTime/(n*k*1000) << " ms"<< endl;
    cout << "Maximum waiting time: " << (double) maxWaitTime/1000 << " ms" << endl;
    return 0;
}