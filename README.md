# Example Linux module illustrating the use of Keyboard LEDs

This code was taken from [The Linux Kernel Module Programming Guide](https://www.tldp.org/LDP/lkmpg/2.6/html/index.html) (which targets Kernel 2.6) and ported to work with Kernel >= 4.15 using the newer timer API.

## Installing

```
$ git clone https://github.com/AndreiZiureaev/kbleds.git
$ make
$ sudo insmod kbleds.ko
```

## Uninstalling
```
$ make clean
$ sudo rmmod kbleds
```
