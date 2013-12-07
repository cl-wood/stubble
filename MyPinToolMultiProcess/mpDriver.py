# Driver to decide how to mutate

import os, shutil, sys
import subprocess
from operator import itemgetter

# MP version: seed is now a list of inputs
def test(inputFile, taintFile, seeds = [('0', '0', '0')], processes = 1):

    # command for subprocess, pintool
    cmd =   [   
        './multiProc', 
        str(processes)
    ]

    # Which test are we running? 
    testNumber = getTestNumber('./taint/')
    print 'Running test #' + str(testNumber), 'based on mutation seed', seeds, '...' 

    # First time, don't mutate
    if seeds != ('0', '0', '0'):
        mutate(inputFile, seeds, processes, cmd)
    else:
        print('Initial run')


    # One iteration of pintool, generates new (input.txt, taint.out) pair
    outputFile = open('./file.out', 'w')
    print cmd
    p1 = subprocess.Popen(cmd)
    p1.wait()

    # Find which taint are new
    # input -> input/ and mutated
    # taint -> taint/
    explored        =   open('./taint/explored.0', 'r').read().split()
    exploredFile    =   open('./taint/explored.0', 'a')
    #taint           =   open(taintFile, 'r').read().split()
    taintValues = seeds[0:processes]
    print taintValues
    taint = map(lambda x: ':'.join(x), taintValues)

    newTaintDict = {i.split(':')[0]: i.split(':')[1:] for i in taint}
    exploredTaintDict = {i.split(':')[0]: i.split(':')[1:] for i in explored}

    # Set difference
    newTaint = {x: newTaintDict[x] for x in newTaintDict if x not in exploredTaintDict.keys()} 

    # Copy taint and append latest taint to explored
    recordResults(inputFile, outputFile, newTaint, seeds, testNumber, processes) 
    #exploredFile.write(':'.join(seed) + "\n")
    for i in seeds[0:processes]:
        exploredFile.write(':'.join(i) + "\n")

    # file in format pos:op:arg1:arg2
    print 'new taint is:', newTaint
    toExplore = [(r, newTaint[r][0], newTaint[r][1]) for r in newTaint]

    toExplore = sorted(toExplore, key=itemgetter(0))
    return toExplore

# End test

# TODO will need to handle non-character cases for seed[1]
def mutate(inputFile, seeds, mutationNum, cmd):

    # Change input value to equal non-tainted immediate
    for seed in seeds:
        f = list(open(inputFile, 'r').read())
        if (int(seed[1]) < 256):
            try:
                f[int(seed[0])] = chr(int(seed[2]))
            except OverflowError as e:
                print 'Value too large for C type long'
                return

        out = ''.join(f)

        fileName = inputFile + '.' + str(mutationNum)
        open(fileName, 'w').write(out)
        cmd.append(fileName)

# End mutate

def recordResults(inputFile, outputFile, taintSet, mutation, n, processes):
    """ inputFile:  input to instrumented binary
        outputFile: record of STDOUT from instrumented binary
        taintSet:   taint from DTA analysis
        mutation:   latest mutation explored
        n:          test number 
    """ 
    

    # Record input file and STDOUT for test n
    for i in xrange(0, processes):
        shutil.copy(inputFile, './input/input.' + str(n + i) )
        shutil.copy(outputFile.name, './output/output.' + str(n + i) )

    # Record mutation n (TODO and it's metadata or tree structure)
    #if mutation != ('0', '0', '0'):
    #    open('./mutations/mutation.' + str(n), 'w').write(':'.join(mutation))
    

    #taint = open('./taint/taint.' + str(n), 'w' )
    #for i in taintSet:
    #    taint.write(i + "\n")

# End copyOperations()

def getTestNumber(taintDir):

    taint = os.listdir(taintDir)
    if len(taint) == 1:
        return 1

    return max([int(i.split('.')[1]) for i in taint]) + 1

    
# End getTestNumber

if __name__ == '__main__':

    # Concurrency handled in c program, this just drives
    try:
        num_processes = sys.argv[1]
    except IndexError as e:
        num_processes = 1 

    seedFile = './file.txt'
    i = 'inputFile.txt'
    shutil.copy(seedFile, i)
    o = './results.out' # might rename in pintool to taint.out, makes more sense

    # initial run
    #toExplore = test(i, o, seeds = ['0','0','0'] processes = num_processes)
    toExplore = test(i, o, processes = num_processes)
    print 'seed run', toExplore

    while len(toExplore) > 1:
        s = toExplore
        toExplore = test(i, o, s, processes = num_processes)
        print toExplore

# End main

