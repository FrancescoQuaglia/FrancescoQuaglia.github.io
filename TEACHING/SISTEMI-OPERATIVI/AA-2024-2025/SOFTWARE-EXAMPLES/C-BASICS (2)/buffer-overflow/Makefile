
# the security problem related to this example can be also tackled by using 
# the"-fstack-protector-all" gcc option for compiling exploit.c as in the 
# 'stack-protect' target of this makefile

all:
	gcc exploit.c -fomit-frame-pointer -no-pie -fno-stack-protector -o exploit
	gcc print-string.c -o print-string

position-independent:
	gcc exploit.c -fomit-frame-pointer -fno-stack-protector -pie -fPIE -o exploit
	gcc print-string.c -o print-string

stack-protect:
	gcc exploit.c -fomit-frame-pointer -fstack-protector-all -o exploit
	gcc print-string.c -o print-string
