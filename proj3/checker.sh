#!/bin/bash
echo [*] Building sequential output...
cat tests/small | /usr/bin/time ./seq > /dev/null

# OPENMP
for ((i = 2; i < 30; i=i+2))
do
    echo [*] Running openmp with $i threads...
    cat tests/small | /usr/bin/time ./openmp $i > /dev/null
    mv openmp.output openmp.output.$i
done


for ((i = 2; i < 30; i=i+2))
do
    echo [*] Comparing proj3.output with each openmp output
    cmp proj3.output openmp.output.$i
done

# PTHREADS
for ((i = 2; i < 30; i=i+2))
do
    echo [*] Running pthreads with $i threads...
    cat tests/small | /usr/bin/time ./pthreads $i > /dev/null
    mv pthreads.output pthreads.output.$i
done


for ((i = 2; i < 30; i=i+2))
do
    echo [*] Comparing proj3.output with each pthread output
    cmp proj3.output pthreads.output.$i
done

# MULTIPROC


