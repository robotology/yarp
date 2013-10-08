
-- Compile yarp with lua binding support

-- set LUA_CPATH to include yarp-lua binding library 
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- go to the portmonitor_carrier/example directory and run gyarpmanager 
   (gyarpmanager --application TestMonitorImage.xml)

-- run and connect the ports. you should see two instances of yarpview showing 
   the original image from fake test grabber and the modified one using lua script
   (i.e. the lua script draw a moving circular point over the original image) 

-- you can change the the forground and background points color using yarp port 
   administrator as follow: 
  
    $ yarp admin rpc /view
    $ set in /grabber (fg 0 255 0)      // this set the forground color to Green (0 G 0)
    $ set in /grabber (bg 0 0 255)      // this set the forground color to Blue (0 0 B)
    $ get in /grabber                   // will get the values 
    


