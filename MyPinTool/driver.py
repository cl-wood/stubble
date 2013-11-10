# Driver to decide how to mutate

import os, subprocess
from operator import itemgetter

def main():

    cmd =   [   
        '../pin/pin', 
        '-t', 'obj-ia32/MyPinTool.so', 
        '--', '../vulnerable_programs/trivial/a.out', 'file.txt'
    ]

    # One iteration of pintool, generates new (file.txt, results.out) pair
    p1 = subprocess.Popen(cmd)
    p1.wait()

    # Get set diffs to decide which parts of user input to mutate next
    explored_results_set = open('./results/explored.out', 'r').read().split()
    #while set(explored_results_set) - 
    results = open('./results.out', 'r').read().split()
    newResults = set(results) - set(explored_results_set) 

    # file in format pos:op:arg1:arg2
    toExplore = [r.split(':') for r in newResults]

    toExplore = sorted(toExplore, key=itemgetter(0))
    print toExplore

    # explore each 
    #for i in toExplore:
    # End main()

def moveOperations(inputFile, outputFile):
    results = os.listdir('.')

def getMutationNumber(resultsDir):

    results = os.listdir(resultsDir)
    if len(results) == 1:
        return 1

    return max([int(i.split('.')[1]) for i in results]) + 1

    
# End getMutationsNumber




#main()
#moveOperations('file.txt', 'results.out')



