
An example which shows how to use a DLL object to modify incoming data
in an input port
======================================================================


-- Open a terminal and follow the bellow instruction to build the dll
   $ mkdir $YARP_ROOT/examples/portmonitor/simple_dll/build
   $ cd $YARP_ROOT/examples/portmonitor/simple_dll/build
   $ cmake ../; make;

   you should see the `libsimple_monitor.so' after the compilation (the generated dll can have
   different names on windows or macos).

-- Open a terminal and run yarpserver
   $ yarpserver

-- Open another terminal (lets call this the sender terminal) and type
   $ yarp write /write

-- In the directory where you you built the dll (lets call this the receiver terminal), type
   $ yarp read /read


-- In another terminal connect the port as follow:
   $ yarp connect /write /read tcp+recv.portmonitor+type.dll+file.simple_monitor

Now if you write something  in the 'sender' terminal, you will see the text
"Modified in DLL" will be added to the original message. For example:

[sender terminal]
 Hello

[receiver terminal]
 Hello "modified in DLL"


As it is constrained in `SimpleMonitorObject::accept()' method from `Simple.cpp', if you type "ignore",
the message will be ignored by the portmonitor and it never be delivered to the input port.
