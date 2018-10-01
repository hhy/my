#!/usr/bin/env bash

for n in 2; do
    ./pi_sequential.out -n $n > out.sequential_$n.txt
    ./pi_parallel.out -n $n > out.parallel_$n.txt
    ./pi_spawn.out -n $n > out.spawn_$n.txt
done
    
	 
