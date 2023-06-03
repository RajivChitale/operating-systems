#!/bin/bash
# using a for loop

g++ Assgn3-Src-tas-CS21BTECH11051.cpp -lpthread -o tas.out
g++ Assgn3-Src-cas-CS21BTECH11051.cpp -lpthread -o cas.out
g++ Assgn3-Src-cas-bounded-CS21BTECH11051.cpp -lpthread -o cas-bounded.out


echo "tas" > autorun.txt
for n in 10 20 30 40 50
do
   echo "$n" >> autorun.txt
   echo "$n 10 5 20" > inp-params.txt
   for i in {1..5}
   do
    #echo $i >> autorun.txt
    temp=`./tas.out`
    echo ${temp} >> autorun.txt
   done
done

echo "cas" >> autorun.txt
for n in 10 20 30 40 50
do
   echo "$n" >> autorun.txt
   echo "$n 10 5 20" > inp-params.txt
   for i in {1..5}
   do
    #echo $i >> autorun.txt
    temp=`./cas.out`
    echo ${temp} >> autorun.txt
   done
done

echo "cas-bounded" >> autorun.txt
for n in 10 20 30 40 50
do
   echo "$n" >> autorun.txt
   echo "$n 10 5 20" > inp-params.txt
   for i in {1..5}
   do
    #echo $i >> autorun.txt
    temp=`./cas-bounded.out`
    echo ${temp} >> autorun.txt
   done
done