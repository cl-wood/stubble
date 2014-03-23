from subprocess import Popen, PIPE
import signal

# Fork and wait for process
proc = Popen('./a.out', shell=True, stdout=PIPE, stderr=PIPE)
proc.wait()

# Check return code of process
returncode = proc.returncode - 128


if returncode == signal.SIGSEGV:
    print 'Segmentation Fault!'


