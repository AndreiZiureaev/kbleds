obj-m += kbleds.o
all:
# make -C /lib/modules/4.15.0-88-generic/build M=$(PWD) modules
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
# make -C /lib/modules/4.15.0-88-generic/build M=$(PWD) clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
