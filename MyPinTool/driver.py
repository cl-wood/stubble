# Driver to decide how to mutate

import os, shutil
import subprocess
from operator import itemgetter


def test(inputFile, outputFile, seed = ('0', '0', '0') ):

    mutate(inputFile, seed)

    cmd =   [   
        '../pin/pin', 
        '-t', 'obj-ia32/MyPinTool.so', 
        '--', '../vulnerable_programs/trivial/a.out', inputFile
    ]

    testNumber = getTestNumber('./results/')
    print('Running test number', testNumber, 'based on seed', seed, '...')

    # One iteration of pintool, generates new (input.txt, results.out) pair
    p1 = subprocess.Popen(cmd)
    p1.wait()

    # Find which results are new
    # input is copied to input/ and mutated
    # results is copied to results/
    explored = open('./results/explored.0', 'r').read().split()
    exploredFile = open('./results/explored.0', 'a')
    results = open(outputFile, 'r').read().split()
    newResults = set(results) - set(explored) 

    # Copy results and append latest results to explored
    copyResults(inputFile, newResults, testNumber) 
    exploredFile.write(':'.join(seed) + "\n")

    # file in format pos:op:arg1:arg2
    toExplore = [r.split(':') for r in newResults]

    toExplore = sorted(toExplore, key=itemgetter(0))
    return toExplore[0]

    # explore each 
    #for i in toExplore:

# End main()

# TODO will need to handle non-character cases for seed[1]
def mutate(inputFile, seed):
    f = list(open(inputFile, 'r').read())

    if (int(seed[1]) < 256):
        f[int(seed[0])] = chr(int(seed[1]))

    out = ''.join(f)

    open(inputFile, 'w').write(out)

# End mutate

def copyResults(inputFile, outputSet, testNumber):

    shutil.copy(inputFile, './input/input.' + str(testNumber) )
    #shutil.copy(outputFile, './results/results.' + str(testNumber) )
    results = open('./results/results.' + str(testNumber), 'w' )
    for i in outputSet:
        results.write(i + "\n")

# End copyOperations()

def getTestNumber(resultsDir):

    results = os.listdir(resultsDir)
    if len(results) == 1:
        return 1

    return max([int(i.split('.')[1]) for i in results]) + 1

    
# End getTestNumber


i = './file.txt'
o = './results.out'

newSeed = test(i, o)
win = ('300', '88', '88')
test(i, o, win)

#while set(explored_results_set) - 



