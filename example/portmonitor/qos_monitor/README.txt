
An example which shows how to use a Lua script to modify incoming data
in an input port
======================================================================


-- Compile YARP with Lua binding support

-- Set LUA_CPATH to include YARP-Lua binding library
    (e.g., export LUA_CPATH=";;;$YARP_ROOT/bindings/build-lua/?.so")

-- Open a terminal and run yarpserver

-- Open another terminal (lets call this the grabber terminal) and type
   $ yarpdev --device fakeFrameGrabber --freq 30

-- Open another terminal (lets call this the viewer terminal) and change
   to the 'qos_monitor' directory and run an instance of the 'yarpview'
   $ cd $YARP_ROOT/example/portmonitor/qos_monitor
   $ yarpview --name /view

-- In another terminal connect the port as follow:
   $ yarp connect /grabber /view tcp+recv.portmonitor+file.qos_monitor


The 'fakeFrameGrabber' is supposed to transmit the image data to the 'yarpview'
in every 30ms (30Hrz). This is monitored in the 'qos_monitor.lua' and when
the real frequency goes below the desired frequency, corresponding message
will be printed out in the viewer terminal.

[viewer terminal]
 [qos_monitor] Qos is not okay. (data frequency < 30Hz)
 [qos_monitor] Qos is not okay. (data frequency < 30Hz)
 ...
