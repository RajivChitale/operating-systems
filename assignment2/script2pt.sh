#!/bin/bash
# using a for loop

gcc Assgn2Srcpthreadcs21btech11051.c -o pthread.out -lm -lpthread
gcc Assgn2SrcOpenMpcs21btech11051.c -o omp.out -lm -fopenmp


echo "OMP" >> autorun.txt
for k in 1 2 4 8 16 32 64
do
   python3 gen.py $k 25 >> autorun.txt
   for i in {1..6}
   do
    #echo $i >> autorun.txt
    temp=`./omp.out -c`
    echo ${temp##*is} >> autorun.txt
   done
done


echo "PTHREAD" >> autorun.txt
for k in 1 2 4 8 16 32 64
do
   python3 gen.py $k 25 >> autorun.txt
   for i in {1..6}
   do
    #echo $i >> autorun.txt
    temp=`./pthread.out -c`
    echo ${temp##*is} >> autorun.txt
   done
done