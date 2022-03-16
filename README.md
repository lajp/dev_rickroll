# dev_rickroll

A kernel module that creates `/dev/rickroll`

## Installation
```sh
make
insmod rickroll.ko # Has to be run as root
```

## Usage
After installation the device should be created.
It can be accessed for example with aplay:
```sh
aplay /dev/rickroll
```
