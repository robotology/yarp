
An example which shows how to use a Lua script to create a new data
type on the sender port
======================================================================


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- Open another terminal (lets call this the sender terminal) and type
   $ yarp write /write

-- Open another terminal (lets call this the receiver terminal) )and change
   to the 'simple_modification' directory:
   $ cd $YARP_ROOT/src/carrier/portmonitor_carrier/example/simple_modification
   $ yarp read /read

-- In another terminal connect the port as follow:
   $ yarp connect /write /read tcp+send.portmonitor+script.lua+file.type_modifier

Now if you write something  in the 'sender' terminal, you will see the text is totaly substituted by a vector.
For example:

[sender terminal]
 Hello

[receiver terminal]
 1.5 2.0 3.0
