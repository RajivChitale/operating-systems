// RAJIV SHAILESH CHITALE
// CS21BTECH11051
// LAB EXAM Q2

#include<iostream>
#include<fstream>
#include<vector>
#include<deque>

#define DEBUG_MODE 0

using namespace std;


// first in first out replacement
int fifo(int frames, int pgsize, vector<int> addr){

    int faults = 0 ;
    deque<int> queue;
    bool hit;

    for(int a=0; a< addr.size(); a++)
    {
        hit = false;
        //check if hit:
        for(int i=0; i< queue.size(); i++)
        {
            if(queue.at(i) == (int) addr.at(a) / pgsize ) 
            {
                hit = true;
                if(DEBUG_MODE) cout << "Hit: page " << addr.at(a)/pgsize << " for address " << addr.at(a) << endl; 
                break;
            }
        }

        if(hit) {continue;}

        if(DEBUG_MODE) cout << "Fault: page " << addr.at(a)/pgsize << " needed by address " << addr.at(a) << endl; 

        if(queue.size() >= frames)       // evict
        {
            if(DEBUG_MODE) cout << "Evicted page " << queue.back() << endl;
            queue.pop_back();                           // pop in fifo
        }   

        queue.push_front((int) addr.at(a) / pgsize);   // push to front 
        faults++;
        
    }
    /*
    keep adding to queue until frames limit, then dequeue
    */
    cout << "Number of page faults with FIFO: " << faults << endl;
    if(DEBUG_MODE) cout << endl;

    return faults;
}

// replace least recently used page 
int lru(int frames, int pgsize, vector<int> addr){
    int faults = 0 ;
    deque<int> queue;
    bool hit;
    int evicted;

    for(int a=0; a< addr.size(); a++)       // check for hit
    {
        hit = false;
        //check if hit:
        for(int i=0; i< queue.size(); i++)
        {
            if(queue.at(i) == (int) addr.at(a) / pgsize ) 
            {
                hit = true;
                if(DEBUG_MODE) cout << "Hit: page " << addr.at(a)/pgsize << " for address " << addr.at(a) << endl;
                int temp =queue.at(i) ;
                queue.erase(queue.begin() + i);
                queue.push_front(temp);   // make most recently used
                break;
            }
        }

        if(hit) {continue;}

        if(DEBUG_MODE) cout << "Fault: page " << addr.at(a)/pgsize << " for address " << addr.at(a) << endl; 


        if(queue.size() >= frames)       // evict
        {
            if(DEBUG_MODE) cout << "Evicted page " << queue.back() << endl;
        
            queue.pop_back();
        }

        faults++;
        queue.push_front( (int) addr.at(a)/ pgsize);     //push
    }
    cout << "Number of page faults with LRU: " << faults << endl;
    if(DEBUG_MODE) cout << endl;

    return faults;
}


// optimal replacement - replace page which will not be used for longest time
int opt(int frames, int pgsize, vector<int> addr){
    int faults = 0 ;
    vector<int> table;
    int opt, max=0;
    bool hit;
    int evicted;

    for(int a=0; a< addr.size(); a++)      
    {
        hit = false;
        for(int i=0; i< table.size(); i++)  // scan for hits
        {
            if(table.at(i) == (int) addr.at(a) / pgsize )   // check for hit
            {
                hit = true;
                if(DEBUG_MODE) cout << "Hit: page " << addr.at(a)/pgsize << " for address " << addr.at(a) << endl; 
                break;
            }
        }

        if(hit) {continue;}

        if(DEBUG_MODE) cout << "Fault: page " << addr.at(a)/pgsize << " for address " << addr.at(a) << endl; 


        faults++;
        table.push_back( (int) addr.at(a)/ pgsize);     //push new frame

        if(table.size() > frames)       // evict the one not used for most time
        {
            opt = 0;
            max = 0;
            for(int i=0; i< table.size(); i++)  // check each page's next use
            {
                for(int next = a+1; next < addr.size(); next++) // scan forward for repetition
                {
                    if( (int) addr.at(next)/pgsize == table.at(i)) break; // found next occurrence

                    if(next > max) 
                    {
                        max = next;
                        opt = i;    
                    }
                    //cout << (int) addr.at(next)/pgsize << " " << table.at(i) <<" temp \n";

                }

            }

            if(DEBUG_MODE) cout << "Evicted page " << opt << endl;
        
            table.erase(table.begin() + opt);
        }


    }
    cout << "Number of page faults with OPT: " << faults << endl;
    if(DEBUG_MODE) cout << endl;


    return faults;
}


int main()
{

    int frames = -1; 
    int pgsize = -1; //temp
    int tempaddr = -1;
    int result = -1;
    vector<int> addr;

    ifstream fin;
    fin.open("inp.txt");
	if (!fin)
	{
		cout << "Input could not be read."<< endl; // for example, file does not exist
		return -1;
	}
    //inputs
    fin>> frames;  
    if(frames==-1) {cout << "Enter number of frames" << endl; return 0; }
    fin >> pgsize;
    if(pgsize==-1) {cout << "Enter page size (in bytes)" << endl; return 0; }


    while(!fin.eof())
    {
        tempaddr = -1;
        fin >> tempaddr;
        if(tempaddr == -1)  break;      // dealing with empty lines
        addr.push_back(tempaddr);
    }
    fin.close();

    fifo(frames, pgsize, addr);
    lru(frames, pgsize, addr);
    opt(frames, pgsize, addr);


    return 0;
}