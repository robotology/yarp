
An example which shows how to use a Lua script to modify incoming image
data in an input port
------------------------------------------------------------------------


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- In another terminal, change to the 'image_modification' directory:
    $ cd $YARP_ROOT/src/carriers/portmonitor_carrier/example/image_modification
    $ yarpmanager --application TestMonitorImage.xml

-- Run and connect the ports; in the yarpmanager's console type:
    >> run
    >> connect

   You should see two instances of yarpview showing the original image from fake test
   grabber and the modified one using Lua script (i.e. the Lua script draws moving circular
   points over the original image)

-- You can also change the foreground and background point's color using yarp port administrator.
   To do this, open another terminal and try the following commands:

    $ yarp admin rpc /modified/yarpview/img:i
    $ set in /grabber (fg 0 255 0)      // this set the foreground color to Green (0 G 0)
    $ set in /grabber (bg 0 0 255)      // this set the foreground color to Blue (0 0 B)
    $ get in /grabber                   // will show the parameter's value


-- To stop the application, in the yarpmanager's console, type:
    >> stop
    >> exit
