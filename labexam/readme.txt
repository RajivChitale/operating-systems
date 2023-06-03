EVALUATION OF PAGE POLICIES
This is a c++ code to simulate page faults and replacement policies.

STEPS TO RUN
$ g++ pgpolicy.cpp
$ ./a.out

INPUT
a file "inp.txt" containing the following information,
First line: Number of physical frames
Second line: Page sizze in Bytes
Third line onwards (until end of file): Logical address accessed by the processor.
The value is given in decimal

Example:
10
4096
255
2010
5143
202020
7330
200
4000 

OUTPUT
Outputs the number of page faults from FIFO, LRU and OPT in the following manner:
Number of page faults with FIFO: 19
Number of page faults with LRU: 14
Number of page faults with OPT: 13

To obtain debugging information such as hits, faults and page replacement, set DEBUG_MODE to 1.
To hide this information, set it to 0.
