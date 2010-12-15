#!/bin/bash

tranducer='T_swap.fst'

i=0
while [ $i -le 10 ]
do
    count="tmp_count$i.txt"
    ./T_count_swap.py $tranducer $count
    tranducer="tmp_T$i.fst"
    ./T_newbasedcount.py $count `basename $tranducer .fst`
    i=$((i+1))
done
