KERN_DIR = /home/book/Desktop/linux-2.6.22.6

#M is a var in makefile,`pwd`=$(shell pwd)
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m:= second_drv.o
