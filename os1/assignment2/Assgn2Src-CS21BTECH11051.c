// OS Assgn2
// CS21BTECH11051	Rajiv Shailesh Chitale
// gcc Assgn2Src-CS21BTECH11051.c -pthread -lm

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h> //only for stopwatch

#define FALSE 0
#define TRUE 1

typedef long long ll;

// contains details of a particular pthread, pointers to parameters and return values.
struct SolverThread
{
	pthread_t tid;		 // id of pthread
	pthread_attr_t attr; // attributes of a pthread
	void *param;		 // pointer to array on heap which stores parameters
	void *retval;		 // pointer to array on heap which stores return values
};

// returns TRUE iff the input parameter num is a perfect number
int isPerfect(ll num)
{
	if (num < 4){return FALSE;} // get rid of small trivial cases whose only smaller factor is 1

	ll sum = 1;						 // one always
	ll factorMax = floor(sqrt(num)); // note: floor

	// loop from 2 to sqrt of num. if a factor i exists, so does num/i. Add both to sum.
	for (ll i = 2; i < factorMax; i++)
	{
		if (num % i == 0){sum += i + num/i;}
		if (sum > num){return FALSE;} // exit early if sum exceeds num
	}

	// separate out last case
	if (factorMax * factorMax == num){sum += factorMax;} // ignore repeated value generated if num/factorMax == factorMax
	else if (num % factorMax == 0){sum += factorMax + num/factorMax;} // otherwise two factors are generated

	if (sum == num){return TRUE;}
	else {return FALSE;}
}

// SEARCH: this function checks a set for perfect numbers, and writes outcomes to a log file.
// Returns pointer to array with perfect numbers (on heap)
void *perfectFinder(void *parameters)
{
	// takes in a pointer to various parameters: n, k and setid
	ll *param = (ll *)parameters;
	ll n = param[0];
	int k = (int)param[1];
	int setid = (int)param[2]; // set with setid+1 == m will cover numbers such as m, m+k, m+2k, m+3k...
	ll perfect[50];			   // local array to store perfect numbers temporarily
	ll *retval;

	char filename[20];
	sprintf(filename, "OutFile%d", setid); // log file name

	FILE *fp;
	fp = fopen(filename, "w+");

	ll count = 0;
	for (ll val = setid + 1; val <= n; val += k) // thus a set will have numbers satisfying val%k -1 = setid
	{
		if (isPerfect(val) == TRUE)
		{
			// printf("set:%d, %lld\n", setid, val); //prints to terminal directly
			fprintf(fp, "%lld: Is a perfect number\n", val); // writing to log file
			perfect[count] = val;							 // storing in local array
			count++;
		}
		else
		{
			fprintf(fp, "%lld: Not a perfect number\n", val);
		}
	}
	fclose(fp);

	retval = (ll *)malloc((count + 1) * sizeof(ll)); // allocating array on heap for returning results
	retval[0] = count;
	for (int index = 1; index <= count; index++)
	{
		retval[index] = perfect[index - 1]; // move local data onto heap for sharing
	}

	pthread_exit((void *)retval);
}

int main(int argc, char *argv[])
{

	struct timeval start, finish;
	gettimeofday(&start, NULL);

	// INPUT: takes filename as argument or input, and reads parameters n and k from the file
	FILE *fp;
	if (argc == 1)
	{
		char ipfile[50];
		printf("Enter the name of the input file: "); // command line input
		scanf("%s", ipfile);
		fp = fopen(ipfile, "r");
	}
	else
	{
		fp = fopen(argv[1], "r"); // command line argument
	}

	if (fp == NULL)
	{
		printf("Input could not be read.\n"); // for example, file does not exist
		return -1;
	}

	ll n = 5000;
	int k = 4; // defaults
	fscanf(fp, "%lld %d", &n, &k);

	if(n<1 || k< 1) 
	{
		"Invalid input\n"; 
		return -1;
	}

	// THREAD CREATION: k threads are created and assigned a set to search for perfect numbers
	struct SolverThread *solver = (struct SolverThread *)malloc(sizeof(struct SolverThread) * k);
	ll *param;

	for (int setid = 0; setid < k; setid++)
	{
		solver[setid].param = malloc(3 * sizeof(ll));
		param = (ll *)solver[setid].param;
		param[0] = n;
		param[1] = (ll)k; // storing as long long for homogeneity
		param[2] = (ll)setid;
		pthread_attr_init(&solver[setid].attr);
		pthread_create(&solver[setid].tid, &solver[setid].attr, perfectFinder, solver[setid].param);
		// create thread running perfectFinder with a pointer to paramters n, k, setid
	}

	char outfile[20] = "OutMain";
	fp = fopen(outfile, "w+");

	ll *retval;
	int count = 0;
	// JOINING THREADS AND RESULTS
	for (int setid = 0; setid < k; setid++)
	{
		pthread_join(solver[setid].tid, &solver[setid].retval);
		retval = (ll *)solver[setid].retval;
		count = retval[0];

		if (count > 0){fprintf(fp, "Thread%d:", setid);}
		for (int index = 1; index <= count; index++)
		{
			fprintf(fp, " %lld", retval[index]);
			// writes perfect numbers to the file OutMain, grouped by thread which found them
		}
		if (count > 0){fprintf(fp, "\n");}

		free(retval);
		free(solver[setid].param); // free heap space used by parameters and return values
	}
	fclose(fp);
	free(solver);

	gettimeofday(&finish, NULL);
	double timeTaken = (double)(finish.tv_usec - start.tv_usec)/1000000 + (double)(finish.tv_sec - start.tv_sec);
	printf("Completed in %lf seconds!\n", timeTaken);
 
	return 0;
}
