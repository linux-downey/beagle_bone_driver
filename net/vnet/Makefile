KERN_DIR="/home/book/src/100ask_imx6ull-sdk/Linux-4.9.88"

obj-m+=vnet.o
all:
	make -C $(KERN_DIR) M=$(PWD) modules
clean:
	make -C $(KERN_DIR) M=$(PWD) clean
