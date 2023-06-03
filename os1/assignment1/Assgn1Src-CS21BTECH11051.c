// CS21BTECH11051
// Rajiv Shailesh Chitale
// gcc perfect.c -lm -lrt

/*
SUMMARY of functions:

isPerfect - checks whether a number is perfect or not

solve - run by child processes. Receives numbers to chec from parent (via sharedBuffer task)
		Writes outcomes to a log file ("OutFilei") and succesful finds to the sharedBuffer result

manage - run by parent process. Gives child processes numbers to check (via sharedBuffer task)
		Reads from sharedBuffer result corresponding to each child. Writes their combined data to a file OutMain.

main - takes input and creates child processes by forking
*/

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

// #include <time.h>

#define FALSE 0
#define TRUE 1

typedef long long ll;

/*useful paramters for a buffer with LEN elements (long long):
statptr[0] is in
statptr[1] is out
bufptr points to elements in the buffer
shmname is the name of the shared memory
*/
struct sharedBuffer
{
	char shmname[20]; 
	int *statptr;
	ll *bufptr;
	int LEN;
};

// returns true iff the input num is a perfect number
int isPerfect(ll num)
{
	if (num < 4)
	{
		return FALSE;
	} // get rid of small trivial cases whose only factor is 1

	ll sum = 1;						 // one always
	ll factorMax = floor(sqrt(num)); // note: floor

	// loop from 2 to sqrt of num. if a factor i exists, so does num/i. Add both to sum.
	for (ll i = 2; i < factorMax; i++)
	{
		if (num % i == 0)
		{
			sum += i + num / i;
		}
		if (sum > num)
		{
			return FALSE;
		} // exit early if sum exceeds num
	}

	// separate out last case
	if (factorMax * factorMax == num)
	{
		sum += factorMax;
	} // ignore repeated value generated if num/factorMax == factorMax
	else if (num % factorMax == 0)
	{
		sum += factorMax + num / factorMax;
	} // otherwise two factors are generated

	if (sum == num)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// SOLVER CHILD PROCESS: reads numbers from shared tasks-buffer. 
// Performs task on each number and writes outcome to log file
// Writes succesful finds to results-buffer
int solve(ll n, int k, int setid)
{
	//sleep(1); //earlier used to avoid children starting before parent
	char filename[20];
	sprintf(filename, "OutFile%d", setid); //log file name

	struct sharedBuffer tasks;
	struct sharedBuffer results;
	sprintf(tasks.shmname, "perfect_shm_%d", setid); 
	sprintf(results.shmname, "perfect_shm_%d", setid);

	tasks.LEN = 16;	  // upto 15 tasks to solve
	results.LEN = 40; // upto 39 successful results

	// SHARED MEMORY CREATION
	const int SIZE = 64 * sizeof(long long);
	int shm_fd = shm_open(tasks.shmname, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);

	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	while (ptr == MAP_FAILED) //if mem map fails, tries again
	{
		printf("Mmap failed (in child with setid: %d). Trying again in one second.\n", setid);
		sleep(1);
		shm_fd = shm_open(tasks.shmname, O_RDWR, 0666);
		ftruncate(shm_fd, SIZE);
		ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	}


	tasks.statptr = (int *)ptr;
	tasks.bufptr = (ll *)(ptr + 6 * sizeof(int));
	results.statptr = (int *)(ptr + 3 * sizeof(int));
	results.bufptr = (ll *)((void *)tasks.bufptr + tasks.LEN * sizeof(long long));
	ll val;
	ll perfect[40]; // local array to store perfect numbers temporarily

	// shared varibles
	while (tasks.statptr[2] != 0 && tasks.statptr[2] != 1) {;} // buffer values not initialized. halt
	int in = tasks.statptr[0];
	int out = tasks.statptr[1];

	// SOLVING AND LOGGING
	FILE *fp;
	fp = fopen(filename, "w+");

	int count = 0;
	while (tasks.statptr[2] == 0 || in != out) // exits if parent is finished giving tasks, and there is no task in buffer
	{

		do
		{
			in = tasks.statptr[0];
			usleep(100);
		} while (in == out); // wait if empty buffer

		val = tasks.bufptr[out];

		if (isPerfect(val) == TRUE)
		{
			// printf("set:%d, out:%d, %lld\n", setid, in, val); //prints to terminal directly
			fprintf(fp, "%lld: Is a perfect number\n", val);
			perfect[count] = val;
			count++;
		}
		else
		{
			fprintf(fp, "%lld: Not a perfect number\n", val);
		}

		out = (out + 1) % tasks.LEN;
		tasks.statptr[1] = out; //
	}

	fclose(fp);

	// printf("solved setid %d: completeflag %d\n", setid, tasks.statptr[2]); //testing checkpoint

	// reuse in, out for second buffer
	in = 0;
	out = 0;
	// write perfect numbers to results-buffer
	for (int i = 0; i < count; i++)
	{
		do
		{
			out = results.statptr[1];
		} while ((in + 1) % results.LEN == out);

		results.bufptr[in] = perfect[i];
		// printf("%lld ",results.bufptr[in]); //testing check
		in = (in + 1) % results.LEN;
		results.statptr[0] = in;
	}

	results.statptr[2] = 1;
	// printf(" setid %d, count %d: returned\n", setid, count); //testing checkpoint

	return 0;
}

// MAIN PARENT PROCESS: distributes the task/numbers to different child process, waits for them and compiles results
int manage(ll n, int k)
{
	struct sharedBuffer *tasks;
	struct sharedBuffer *results;

	tasks = (struct sharedBuffer *)malloc(k * sizeof(struct sharedBuffer));
	results = (struct sharedBuffer *)malloc(k * sizeof(struct sharedBuffer));

	// SHARED MEMORY CREATION: for each child
	for (int setid = 0; setid < k; setid++)
	{
		tasks[setid].LEN = 16;	 // upto 15 tasks to solve
		results[setid].LEN = 40; // upto 39 successful results

		sprintf(tasks[setid].shmname, "perfect_shm_%d", setid);
		sprintf(results[setid].shmname, "perfect_shm_%d", setid);

		const int SIZE = 64 * sizeof(long long);
		int shm_fd = shm_open(tasks[setid].shmname, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SIZE);

		void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED)
		{
			printf("Map failed!\n");
			return -1;
		}

		tasks[setid].statptr = (int *)ptr;
		tasks[setid].bufptr = (ll *)(ptr + 6 * sizeof(int));
		results[setid].statptr = (int *)(ptr + 3 * sizeof(int));
		results[setid].bufptr = (ll *)((void *)tasks[setid].bufptr + tasks[setid].LEN * sizeof(long long));

		// initialize common variables in shared memory
		tasks[setid].statptr[0] = 0;
		tasks[setid].statptr[1] = 0;
		tasks[setid].statptr[2] = 0;
		results[setid].statptr[0] = 0;
		results[setid].statptr[1] = 0;
		results[setid].statptr[2] = 0;
	}

	int setid = 0;
	int in = 0;
	int out = 0;
	// TASK DISTRIBUTION
	//goes through each set, and writes to buffer wherever not full
	for (int val = 1; val <= n;)
	{
		in = tasks[setid].statptr[0];
		out = tasks[setid].statptr[1];
		if ((in + 1) % tasks[setid].LEN == out)
		{
			continue;
		}

		tasks[setid].bufptr[in] = val;
		// printf("set: %d, in: %d, %lld\n", setid, tasks[setid].statptr[0], tasks[setid].bufptr[in] );	//

		in = (in + 1) % tasks[setid].LEN;
		tasks[setid].statptr[0] = in;
		val++;
		setid = (setid + 1) % k;
	}

	// WAIT
	for (int setid = 0; setid < k; setid++)
	{
		tasks[setid].statptr[2] = 1;
	}
	while (wait(NULL) != -1)
	{
		;
	} // waits for all child processes to complete

	// READ RESULTS AND OUTPUT TO FILE
	char filename[20] = "OutMain";
	FILE *fp;
	fp = fopen(filename, "w+");

	int count = 0;
	ll val;
	for (int setid = 0; setid < k;)
	{
		in = results[setid].statptr[0];
		out = results[setid].statptr[1];

		if (in == out && results[setid].statptr[2] == 1) // finished reading results from a set
		{
			shm_unlink(results[setid].shmname);
			setid++;
			if (count > 0)
				fprintf(fp, "\n");
			count = 0;
			continue;
		};
		// while(in == out){in = results[setid].statptr[0];}
		// wait for empty buffer to refill. not applicable because children have terminated

		if (count == 0)
		{
			fprintf(fp, "P%d:", setid);
		}
		val = results[setid].bufptr[out];
		fprintf(fp, " %lld", val);

		out = (out + 1) % results[setid].LEN;
		results[setid].statptr[1] = out;
		count++;
	}
	fclose(fp);

	// free from heap
	free(tasks);
	free(results);

	printf("Completed!\n");
	return 0;
}

int main(int argc, char *argv[])
{
	//time_t init = time(NULL);

	// INPUT: takes filename, and reads parameters n and k from the file
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
	int k = 4;
	fscanf(fp, "%lld %d", &n, &k);

	// FORKING: to create k child processes
	pid_t pid;

	int setid;
	for (setid = 0; setid < k; setid++)
	{
		pid = fork();
		if (pid == 0)
		{
			break;
		} // child process breaks, to avoid further forking
	}

	if (pid == 0)
	{
		solve(n, k, setid);
	}
	else if (pid > 0)
	{
		manage(n, k);
		/*time_t delta = time(NULL) - init;
		printf("%ld\n",delta);*/
	}
	else{
		printf("Fork fail!\n");
	}

	// single process for testing/comparision purposes
	/*
	for(ll i=1; i<10000; i++)
	{
		if(isPerfect(i) == TRUE){printf("%lld: Is a perfect number\n", i);}
	}
	*/
	return 0;
}

/*
Parts of the code:
perfect number checker
child process creation and load distribution
shared memory and buffers
file read write
*/