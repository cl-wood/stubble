# Driver to decide how to mutate

import os, subprocess
from operator import itemgetter

cmd =   [   
            '../pin/pin', 
            '-t', 'obj-ia32/MyPinTool.so', 
            '--', '../vulnerable_programs/trivial/a.out', 'file.txt'
        ]

p1 = subprocess.Popen(cmd)
p1.wait()

print('done')

# Get set diffs
master_results_set = open('./results/master', 'r').read().split()
#while set(master_results_set) - 
results = open('./results/results.2').read().split()
newResults = set(results) - set(master_results_set) 

todo = []
for r in newResults:
    todo.append(map(lambda x: int(x), r.split(':') ) )

todo = sorted(todo, key=itemgetter(0))





