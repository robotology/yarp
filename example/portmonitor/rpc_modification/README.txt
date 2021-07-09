
An example which shows how to use a Lua script to modify incoming data
in an input port
======================================================================


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- Open another terminal (lets call this the server terminal) and type
   $ yarp rpcserver /server --echo

-- Open another terminal (lets call this the client terminal) and change
   to the 'rpc_modification' directory:
   $ cd $YARP_ROOT/src/carriers/portmonitor_carrier/examples/rpc_modification
   $ ./client.lua


The client.lua sends the "Hello" rpc command to the server and the server returns it back to the client.
You can see how the command and reply are changed by the portmonitor ("Ciao" is added to the command
and "Mondo" is added to the reply) by looking at the client and server terminal.

[client terminal]
$ ./client.lua
in create!
yarp: Sending output from /client to /server using tcp+send.portmonitor+file.rpc_modifier
command:    Hello
reply:      Hello "(Ciao)" "(Mondo)"
command:    Hello
reply:      Hello "(Ciao)" "(Mondo)"
...

[server terminal]
$ yarp rpcserver --echo /server
Waiting for a message...
yarp: Receiving input from /client to /server using tcp+send.portmonitor+file.rpc_modifier
Message: Hello "(Ciao)"
Reply: Waiting for a message...
Message: Hello "(Ciao)"
Reply: Waiting for a message...
...
