# Cora Filesystem Traverser 🪃

Cora is a simple and lightweight filesystem traversal kernel module compiled against the linux kernel v6.5.0.

How to Run Cora
==========

Run the Makefile included in the repository ensuring you have at least gcc-12 installed:
```shell
make
```

Then simply insert the module:
```shell
sudo insmod main.ko
```

Kernel ring buffer logs can be observed through the following command:
```shell
sudo dmesg | tail -n 50
```

Finally, unload the module:
```shell
sudo rmmod main
```