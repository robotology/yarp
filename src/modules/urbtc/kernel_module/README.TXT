# Copyright (C) 2007 iXs Research Corp
# CopyPolicy: Released under the terms of the GNU GPL version 2

Here are the sources for the urbtc kernel module located.
 
Compile it with the makefile included and load the module urbtc.ko
$>insmod ./urbtc.ko

plug the usb devices

/dev/urbtc0...2 sould be created.

you can use 'usbview' to check the devices

or use dmesg | tail to see wether attached correctly

if problem try to unload usbhid (hid) module.

it should be possible to plug/unplug the usb cable without any crash. Kernel detaches/attaches the device nodes accordingly.
