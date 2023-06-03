#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>


enum returnEnum {SUCCESS = 0, FAIL = -1, DONE = 2};
enum typesEnum {BOX = 0, ROW = 1, COL = 2}; 
enum statusEnum {VALID = 0, INVALID = 1};
enum tfEnum {TRUE = 1, FALSE = 0};

char typesText[3][10] = {"grid", "row", "column"};
char validityText[2][10] = {"valid", "invalid"};

int earlyTermination = TRUE; //extra credit part
int consoleSummary = TRUE;
int noLog = FALSE;
int k;
int N;
int n;
int** grid; //input 2d array
int status = SUCCESS; //default

int threadLogEntry(int threadid, int tasktype, int workid, int validity)
{
    if(noLog==TRUE) {return 1;}
    char filename [20] = "output.txt";
    FILE* fp = fopen(filename, "a+");
    int tasknum = 1 + workid % N;
    #pragma omp critical 
    {
        fprintf(fp, "Thread %d checks %s %d and is %s.\n", threadid, typesText[tasktype], tasknum, validityText[validity] );
    }
    fclose(fp);
    return 0;
}

int checkRectangle(int x1, int y1, int x2, int y2)
{
    int* count = (int*) calloc(N, sizeof(int));
    int value;
    for(int y=y1; y<=y2; y++)
    {
        for(int x=x1; x<=x2; x++)
        {
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
    if(checkRectangle(c, 0, c, N-1) == FAIL ) {return FAIL;}        //checks row
    else {return SUCCESS;}
}
int checkBox(int b)
{
    int x = n* (b%n);  //takes values over 0, n, 2n ... (n-1)n 
    int y = n* (int) (b/n); //takes values over 0, n, 2n ... (n-1)n
    if(checkRectangle(x, y, x+n-1, y+n-1) == FAIL ) {return FAIL;}  //checks boxes
    else {return SUCCESS;}
}


int solver(int workid)
{
    int x, y;
    int threadid = omp_get_thread_num();

    if (earlyTermination == TRUE && status == FAIL) {return FAIL;} //extra credit part
    if(workid<N)
    { 
        if(checkBox(workid) == FAIL )
        {
            threadLogEntry(threadid +1, BOX, workid, INVALID); //thread 1 corresponds to threadid 0
            status = FAIL;
        }  //checks boxes
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
        }       //checks row
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
        }       //checks col
        else
        {
            threadLogEntry(threadid+1, COL, workid, VALID);
        }
    }

    return DONE;
}


int input()
{
    char filename [20] = "input.txt";
    FILE* fp = fopen(filename, "r");
    fscanf(fp, "%d%d", &k, &N);
    n = (int) sqrt(N);
    if(n*n != N){fclose(fp); return FAIL;}
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

    //clear contents of output file
    char filename [20] = "output.txt";
    FILE* fp = fopen(filename, "w+"); //w to overwrite from start
    fprintf(fp, "Log for k = %d and N = %d:\n\n", k, N);
    fclose(fp);

    omp_set_dynamic(0);     
    omp_set_num_threads(k); 

    double start_time = omp_get_wtime();

    //creating openmp threads
    #pragma omp parallel for schedule(static,1)
    for(int workid=0; workid < 3*N; workid++)
    {
        solver(workid); 
    }

    double end_time = omp_get_wtime();

    char ofilename [20] = "output.txt";
    FILE* ofp = fopen(ofilename, "a+");
    if(status==FAIL){fprintf(ofp, "\nSudoku is invalid.\n");}
    else {fprintf(ofp, "\nSudoku is valid.\n");}
    fprintf(ofp, "The total time taken is %.2lf microseconds.", (end_time- start_time)* 1e6 );
    fclose(ofp);

    if(consoleSummary == TRUE)
    {
        if(status==FAIL){printf("Sudoku is invalid.\n");}
        else {printf("Sudoku is valid.\n");}
        printf("The total time taken is %.2lf microseconds.\n", (end_time- start_time)* 1e6 );
    }  

    for(int y=0; y<N; y++)
    {
        free(grid[y]);
    }
    free(grid);
    if(status == FAIL) {return FAIL;}
    else {return SUCCESS;}
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

