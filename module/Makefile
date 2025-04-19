

PWD := $(shell pwd)
KERNEL_DIR ?=

obj-m += monitor_blocked_times.o

all: monitor_blocked_times.c
	make -C $(KERNEL_DIR) M=$(PWD) modules
	

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
