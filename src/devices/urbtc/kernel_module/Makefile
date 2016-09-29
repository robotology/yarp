obj-m := urbtc.o

KREV  := $(shell uname -r)
KDIR  := /lib/modules/$(KREV)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
