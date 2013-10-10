
An example which shows how to use a Lua script to modify incoming image
data in an input port 
------------------------------------------------------------------------


-- Compile yarp with lua binding support

-- Set LUA_CPATH to include yarp-lua binding library 
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- In another terminal go to the $YARP_ROOT/src/carrier/portmonitor_carrier/example 
   directory and run yarpmanager (yarpmanager --application TestMonitorImage.xml)

-- Run and connect the ports (type run then connect). you should see two instances of
   yarpview showing the original image from fake test grabber and the modified one using
   Lua script (i.e. the Lua script draws moving circular points over the original image) 

-- You can change the the foreground and background point's color using yarp port 
   administrator as follow: 
  
    $ yarp admin rpc /modified/yarpview/img:i
    $ set in /grabber (fg 0 255 0)      // this set the foreground color to Green (0 G 0)
    $ set in /grabber (bg 0 0 255)      // this set the foreground color to Blue (0 0 B)
    $ get in /grabber                   // will show the parameter's value 
    


