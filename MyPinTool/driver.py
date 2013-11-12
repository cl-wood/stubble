# Driver to decide how to mutate

import os, shutil
import subprocess
from operator import itemgetter


def test(inputFile, taintFile, seed = ('0', '0', '0') ):

    # First time, don't mutate
    if seed != ('0', '0', '0'):
        mutate(inputFile, seed)
    else:
        print('Initial run')
    
    # command for subprocess, pintool
    cmd =   [   
        '../pin/pin', 
        '-t', 'obj-ia32/MyPinTool.so', 
        '--', '../vulnerable_programs/trivial/a.out', inputFile
    ]

    # Which test are we running? 
    testNumber = getTestNumber('./taint/')
    print 'Running test #' + str(testNumber), 'based on mutation seed', seed, '...' 

    # One iteration of pintool, generates new (input.txt, taint.out) pair
    outputFile = open('./file.out', 'w')
    p1 = subprocess.Popen(cmd, stdout=outputFile)
    p1.wait()

    # Find which taint are new
    # input  -> input/ and mutated
    # taint -> taint/
    explored        =   open('./taint/explored.0', 'r').read().split()
    exploredFile    =   open('./taint/explored.0', 'a')
    taint           =   open(taintFile, 'r').read().split()

    # Set difference
    # HACK for now, only mutate each byte found in initial run once
    newTaint = set(taint) - set(explored) 

    # Copy taint and append latest taint to explored
    recordResults(inputFile, outputFile, newTaint, seed, testNumber) 
    exploredFile.write(':'.join(seed) + "\n")

    # file in format pos:op:arg1:arg2
    toExplore = [r.split(':') for r in newTaint]

    toExplore = sorted(toExplore, key=itemgetter(0))
    return toExplore

    # explore each 
    #for i in toExplore:

# End main()

# TODO will need to handle non-character cases for seed[1]
def mutate(inputFile, seed):
    f = list(open(inputFile, 'r').read())

    # Change input value to equal non-tainted immediate
    if (int(seed[1]) < 256):
        f[int(seed[0])] = chr(int(seed[2]))

    out = ''.join(f)

    open(inputFile, 'w').write(out)

# End mutate

def recordResults(inputFile, outputFile, taintSet, mutation, n):
    """ inputFile:  input to instrumented binary
        outputFile: record of STDOUT from instrumented binary
        taintSet:   taint from DTA analysis
        mutation:   latest mutation explored
        n:          test number 
    """ 
    

    # Record input file and STDOUT for test n
    shutil.copy(inputFile, './input/input.' + str(n) )
    shutil.copy(outputFile.name, './output/output.' + str(n) )

    # Record mutation n (TODO and it's metadata or tree structure)
    if mutation != ('0', '0', '0'):
        open('./mutations/mutation.' + str(n), 'w').write(':'.join(mutation))
    

    taint = open('./taint/taint.' + str(n), 'w' )
    for i in taintSet:
        taint.write(i + "\n")

# End copyOperations()

def getTestNumber(taintDir):

    taint = os.listdir(taintDir)
    if len(taint) == 1:
        return 1

    return max([int(i.split('.')[1]) for i in taint]) + 1

    
# End getTestNumber


seedFile = './file.txt'

i = 'inputFile.txt'
shutil.copy(seedFile, i)
o = './results.out' # might rename in pintool to taint.out, makes more sense

# initial run
toExplore = test(i, o)

for s in toExplore:
    # Re-seed in loop
    shutil.copy(seedFile, i)
    test(i, o, s)

#while set(explored_taint_set) - 

