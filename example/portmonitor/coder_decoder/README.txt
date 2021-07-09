An example which shows how to use the portmonitor object at both side of
a connection to encode and decode the data
======================================================================


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- Open another terminal (lets call this the sender terminal) )and change
   to the 'coder_decoder' directory:
   $ cd $YARP_ROOT/example/portmonitor/coder_decoder
   $ yarp write /write

-- Open another terminal (lets call this the receiver terminal) )and change
   to the 'coder_decoder' directory:
   $ cd $YARP_ROOT/example/portmonitor/coder_decoder
   $ yarp read /read

-- In another terminal connect the port as follow:
   $ yarp connect /write /read tcp+send.portmonitor+type.lua+file.codec

Now if you write something in the 'sender' terminal, you will see the original text
gets encoded with a simple base64 encoder and transmited to the receiver. For example:

[sender terminal]
 Hello

[receiver terminal]
 "SGVsbG8="


-- Now try to plug the `codec.lua' to the receiver side too. The you will see the data
   gets decoded and the original text will be shown in the receiver terminal:
   $ yarp connect /write /read tcp+send.portmonitor+file.codec+recv.portmonitor+file.codec

[sender terminal]
 Hello

[receiver terminal]
 "Hello"

Notice that codec.lua acts as coder or decoder depending to which side of the connection
it is attached. This is checked in the PortMonitor.create(options) callback.
