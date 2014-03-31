Be sure to compile with:
	gcc -fno-stack-protector
And make sure /proc/sys/kernel/randomize_va_space is 0
Also using tcc is useful

Make sure it works for:
    loops
    funcs
    strcpy
