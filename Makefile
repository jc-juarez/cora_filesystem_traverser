# *************************************
# Cora Filesystem Traverser
# Module Initialization
# Environment: Build process
# 'Makefile'
# Author: jcjuarez
# *************************************

obj-m += main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
