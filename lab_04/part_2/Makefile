KVERSION = $(shell uname -r)
KDIR = /lib/modules/$(KVERSION)/build
PWD = $(shell pwd)
TARGET1 = fortune
obj-m := $(TARGET1).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

disclean: clean
	@rm -f *.ko *.symvers *.mod