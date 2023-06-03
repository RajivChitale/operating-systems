#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

enum returnEnum {SUCCESS = 0, FAIL = -1, DONE = 2};
enum typesEnum {BOX = 0, ROW = 1, COL = 2}; 
enum statusEnum {VALID = 0, INVALID = 1};
enum tfEnum {TRUE = 1, FALSE = 0};

char typesText[3][10] = {"grid", "row", "column"};
char validityText[2][10] = {"valid", "invalid"};
pthread_mutex_t lock;

//global variables and flags
int earlyTermination = TRUE; //extra credit part
int consoleSummary = TRUE;
int noLog = FALSE;
int k;
int N;
int n;
int** grid;
int status;

//writes an entry to the log file, corresponding to a task
int threadLogEntry(int threadid, int tasktype, int workid, int validity)
{
    if(noLog==TRUE) {return 1;}
    char filename [20] = "output.txt";
    FILE* fp = fopen(filename, "a+");
    int tasknum = 1 + workid % N;
    pthread_mutex_lock(&lock);
    fprintf(fp, "Thread %d checks %s %d and is %s.\n", threadid, typesText[tasktype], tasknum, validityText[validity] );
    pthread_mutex_unlock(&lock);
    //fprintf is thread safe, has lock mechanism
    fclose(fp);
    return 0;
}

//checks uniqueness condition for numbers in a rectangle from (x1,y1) to (x2,y2)
int checkRectangle(int x1, int y1, int x2, int y2)
{
    int* count = (int*) calloc(N, sizeof(int));
    int value;
    for(int y=y1; y<=y2; y++)
    {
        for(int x=x1; x<=x2; x++)
        {
            //value =  grid[x+ N*y]; //old version
            value =  grid[y][x];
            if( value  > 0 && value <= N && count[ value -1 ] == 0 ) 
                { count[ value -1 ]++; }
            else
                {free(count); return FAIL;}
        }
    }
    free(count);
    return SUCCESS;
}

int checkRow(int r)
{
    if(checkRectangle(0, r, N-1, r) == FAIL ) {return FAIL;}
    else {return SUCCESS;}
}
int checkCol(int c)
{
    if(checkRectangle(c, 0, c, N-1) == FAIL ) {return FAIL;}       
    else {return SUCCESS;}
}
int checkBox(int b)
{
    int x = n* (b%n);  //takes values over 0, n, 2n ... (n-1)n 
    int y = n* (int) (b/n); //takes values over 0, n, 2n ... (n-1)n
    if(checkRectangle(x, y, x+n-1, y+n-1) == FAIL ) {return FAIL;}  
    else {return SUCCESS;}
}


void* solver(void* param)
{
    int threadid = (int) (long long) param; //pointer to longlong to int 
    int x, y;

    for(int workid=threadid; workid< 3*N; workid+=k)
    {
        if(earlyTermination == TRUE && status==FAIL) {pthread_exit(NULL);}
        if(workid<N)
        { 
            if(checkBox(workid) == FAIL )
            {
                threadLogEntry(threadid +1, BOX, workid, INVALID); //thread 1 corresponds to threadid 0
                status = FAIL;
            }
            else
            {
                threadLogEntry(threadid +1 , BOX, workid, VALID);
            }
        }
        else if(workid<2*N) 
        { 
            if(checkRow(workid-N) == FAIL ) 
            {
                threadLogEntry(threadid +1, ROW, workid, INVALID);
                status = FAIL;    
            }     
            else
            {           
                threadLogEntry(threadid +1, ROW, workid, VALID);
            }
        }
        else if(workid<3*N)
        { 
            if(checkCol(workid-2*N) == FAIL ) 
            {
                threadLogEntry(threadid+1, COL, workid, INVALID);
                status = FAIL;   
            }      
            else
            {
                threadLogEntry(threadid+1, COL, workid, VALID);
            }
        }
    }
    pthread_exit(NULL);
}

int input()
{
    char filename [20] = "input.txt";
    FILE* fp = fopen(filename, "r");
    fscanf(fp, "%d%d", &k, &N);
    n = (int) sqrt(N);
    if(n*n != N){ fclose(fp); return FAIL;}
    //N has to be perfect square

    grid = (int**) malloc( N*sizeof(int*) );
    for(int y=0; y<N; y++)
    {
        grid[y] = (int*) malloc( N*sizeof(int) );
        for(int x=0; x<N; x++)
        {
            fscanf(fp, "%d", &grid[y][x]);
        }
    }

    fclose(fp);

    return DONE;
}

//calls input function and creates threads for sudoku checking
int sudokuChecker()
{
    if(input() == FAIL) {printf("N needs to be a perfect square.\n"); return FAIL;}

    char filename [20] = "output.txt";
    FILE* fp = fopen(filename, "w+");   //w to overwrite from start
    fprintf(fp, "Log for k = %d and N = %d:\n\n", k, N);
    fclose(fp);

    pthread_t* tidArray = (pthread_t*) malloc(sizeof(pthread_t)*k);
    pthread_attr_t def_attr;
    pthread_attr_init(&def_attr);

	struct timeval start, finish;
	gettimeofday(&start, NULL);

    //pthread creation
    for(int threadid=0; threadid < k; threadid++)
    {
        pthread_create(&tidArray[threadid], &def_attr, solver, (void*)(long long) threadid);
    }

    for(int threadid=0; threadid < k; threadid++)
    {
        pthread_join(tidArray[threadid], NULL);
    }

	gettimeofday(&finish, NULL);
	double timeTaken = (double)(finish.tv_usec - start.tv_usec) + (double)(finish.tv_sec - start.tv_sec)*1e6;
	

    char ofilename [20] = "output.txt";
    FILE* ofp = fopen(ofilename, "a+");
    if(status==FAIL){fprintf(ofp, "\nSudoku is invalid.\n");}
    else {fprintf(ofp, "\nSudoku is valid.\n");}
    fprintf(ofp, "The total time taken is %d microseconds.", (int) timeTaken );
    fclose(ofp);

    if(consoleSummary == TRUE)
    {
        if(status==FAIL){printf("Sudoku is invalid.\n");}
        else {printf("Sudoku is valid.\n");}
        printf("The total time taken is %d microseconds.\n", (int) timeTaken  );
    }  

    free(grid);
    return SUCCESS;
}

//checks flags
int main(int argc, char** argv)
{
    for(int i=1; i< argc; i++)
    {
        if(strcmp(argv[i],"-c") == 0)       //complete: log without early termination
        {
            earlyTermination = FALSE;
        }
        if(strcmp(argv[i],"-s") == 0)       //silent: no output to console
        {
            consoleSummary = FALSE;
        }
        if(strcmp(argv[i],"-nl") == 0)       //silent: no output to console
        {
            noLog = TRUE;
        }
    }
    sudokuChecker();
    return 0;
}


