Clark Wood
COP 5570 Proj3
28 Oct 2013

Testing:
    cat TEST | /usr/bin/time ./exec

    Results for sequential jacobi:

        3.30user 0.01system 0:03.35elapsed 99%CPU (0avgtext+0avgdata 51184maxresident)k
        0inputs+0outputs (0major+3221minor)pagefaults 0swaps

Able to beat under what circumstances:
    Use test ./tests/large

    SEQUENTIAL 
    23.42user 0.05system 0:23.53elapsed 99%CPU
    OPENMP   (16 threads)
    32.23user 0.24system 0:05.91elapsed 549%CPU
    PTHREADS (16 threads)
    25.32user 0.16system 0:05.90elapsed 431%CPU


How to compile:

Known Bugs:


