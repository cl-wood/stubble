# Driver to decide how to mutate

import os, shutil
import subprocess
from operator import itemgetter

seed_value = ('0', '0', '0')

def test(inputFile, taintFile, seed = seed_value):

    # First time, don't mutate
    if seed != seed_value:
        mutate(inputFile, seed)
    else:
        print('Initial run')
    
    # command for subprocess, pintool
    cmd =   [   
        '../pin/pin', 
        '-t', 'obj-ia32/MyPinTool.so', 
        #'--', '../vulnerable_programs/trivial/a.out', inputFile
        '--', './a.out', inputFile
        #'--', './simpleCrackme', inputFile
        #'--', './simpleCrackme2', inputFile
        #'--', './simpleCrackme50', inputFile
        #'--', './strcpy', inputFile
        #'--', 'ls'
    ]

    # Which test are we running? 
    testNumber = getTestNumber('./taint/')
    print 'Running test #' + str(testNumber), 'based on mutation seed', seed, '...' 

    # One iteration of pintool, generates new (input.txt, taint.out) pair
    outputFile = open('./file.out', 'w')
    p1 = subprocess.Popen(cmd, stdout=outputFile)
    p1.wait()

    # Find which taint are new
    # input -> input/ and mutated
    # taint -> taint/
    explored        =   open('./taint/explored.0', 'r').read().split()
    exploredFile    =   open('./taint/explored.0', 'a')
    taint           =   open(taintFile, 'r').read().split()

    newTaintDict = {i.split(':')[0]: i.split(':')[1:] for i in taint}
    exploredTaintDict = {i.split(':')[0]: i.split(':')[1:] for i in explored}

    # Set difference
    newTaint = {x: newTaintDict[x] for x in newTaintDict if x not in exploredTaintDict.keys()} 

    # Copy taint and append latest taint to explored
    recordResults(inputFile, outputFile, newTaint, seed, testNumber) 
    exploredFile.write(':'.join(seed) + "\n")

    # file in format pos:op:arg1:arg2
    toExplore = [(r, newTaint[r][0], newTaint[r][1]) for r in newTaint]

    toExplore = sorted(toExplore, key=itemgetter(0))
    return toExplore

# End test

# TODO will need to handle non-character cases for seed[1]
def mutate(inputFile, seed):
    f = list(open(inputFile, 'r').read())

    # Change input value to equal non-tainted immediate
    if (int(seed[1]) < 256):
        try:
            f[int(seed[0])] = chr(int(seed[2]))
        except OverflowError as e:
            print 'Value too large for C type long'
            return

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
    if mutation != seed_value:
        open('./mutations/mutation.' + str(n), 'w').write(':'.join(mutation))
    

    taint = open('./taint/taint.' + str(n), 'w' )
    for i in taintSet:
        taint.write(i + "\n")

# End copyOperations()

def getTestNumber(taintDir):

    taint = os.listdir(taintDir)
    taint.remove('.gitignore')
    if len(taint) == 1:
        return 1

    return max([int(i.split('.')[1]) for i in taint]) + 1

    
# End getTestNumber

if __name__ == '__main__':
    seedFile = './file.txt'
    i = 'inputFile.txt'
    shutil.copy(seedFile, i)
    o = './results.out' # might rename in pintool to taint.out, makes more sense

    # initial run
    toExplore = test(i, o)
    print 'seed run', toExplore

    while len(toExplore) > 1:
        s = toExplore[0]
        toExplore = test(i, o, s)
        print toExplore

# End main

