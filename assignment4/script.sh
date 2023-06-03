#!/bin/bash
# using a for loop
g++ Assgn4-Src-CS21BTECH11051.cpp -lpthread -o Assgn4.out
lp=60
lc=110

echo "" > autorun.txt

for i in {1..5}
do
   echo "PLOT1 Average Tour Time" >> autorun.txt
   for P in {10..50..5}
   do
      #echo "$P" >> autorun.txt
      echo "$P 25 $lp $lc 5" > inp-params.txt
      #echo $i >> autorun.txt
      runCode=`./Assgn4.out`
      echo ${runCode} >> autorun.txt
   done

   echo "PLOT2 Average Ride Time" >> autorun.txt
   for C in {5..25..5}
   do
      #echo "$C" >> autorun.txt
      echo "50 $C $lp $lc 3" > inp-params.txt
      #echo $i >> autorun.txt
      runCode=`./Assgn4.out`
      echo ${runCode} >> autorun.txt
   done
   echo "END OF ITERATION" >> autorun.txt
done