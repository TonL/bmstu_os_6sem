CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
obj-m := md.o md1.o md2.o md3.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.o .*.cmd .*.flags *.mod.c *.order
	rm -f .*.*.cmd *~ *.*~ TODO.*
	rm -fR .tmp*
	rm -rf .tmp_versions
	rm .*.d *.ko *.mod Module.symvers

disclean: clean
	@rm *.ko *.symvers