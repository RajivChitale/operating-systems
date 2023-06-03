#!/bin/bash
# using a for loop

gcc Assgn2Srcpthreadcs21btech11051.c -o pthread.out -lm -lpthread
gcc Assgn2SrcOpenMpcs21btech11051.c -o omp.out -lm -fopenmp


echo "OMP" >> autorun.txt
for N in 9 16 25 36 49 64 81 100
do
   python3 gen.py 16 $N >> autorun.txt
   for i in {1..6}
   do
    #echo $i >> autorun.txt
    temp=`./omp.out -c`
    echo ${temp##*is} >> autorun.txt
   done
done


echo "PTHREAD" >> autorun.txt
for N in 9 16 25 36 49 64 81 100
do
   python3 gen.py 16 $N >> autorun.txt
   for i in {1..6}
   do
    #echo $i >> autorun.txt
    temp=`./pthread.out -c`
    echo ${temp##*is} >> autorun.txt
   done
done