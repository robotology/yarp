
An example which shows how to use a Lua script to modify incoming data
in an input port
======================================================================


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- Open another terminal (lets call this the sender terminal) and type
   $ yarp write /write

-- Open another terminal (lets call this the receiver terminal) )and change
   to the 'simple_modification' directory:
   $ cd $YARP_ROOT/src/carriers/portmonitor_carrier/examples/simple_modification
   $ yarp read /read

-- In another terminal connect the port as follow:
   $ yarp connect /write /read tcp+recv.portmonitor+type.lua+file.bot_modifier

Now if you write something  in the 'sender' terminal, you will see the text
"modified from Lua" will be added to the original message. For example:

[sender terminal]
 Hello

[receiver terminal]
 Hello "modified from Lua"


As it is constrained in this PortMonitor.accept() method, If you type "ignore",
the word will never be delivered to the input port.
