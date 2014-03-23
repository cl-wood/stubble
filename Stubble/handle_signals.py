from subprocess import Popen, PIPE
import signal

# doesn't work, use Pin
# Fork and wait for process
#proc = Popen('./a.out', shell=True, stdout=PIPE, stderr=PIPE)
proc = Popen(['../pin/pin', '-t', 'obj-ia32/MyPinTool.so', '--', './a.out'], shell=True, stdout=PIPE, stderr=PIPE)
proc.wait()

# Check return code of process
returncode = proc.returncode - 128


if returncode == signal.SIGSEGV:
    print 'Segmentation Fault!'


