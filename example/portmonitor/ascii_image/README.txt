
An example which shows how to use a DLL object to modify incoming data
in an input port (converting a YARP image to Ascii code)
======================================================================


-- Open a terminal and follow the bellow instruction to build the dll
   $ mkdir $YARP_ROOT/examples/portmonitor/ascii_image/build
   $ cd $YARP_ROOT/examples/portmonitor/ascii_image/build
   $ cmake ../; make;

   you should see the `libascii_image.so' after the compilation (the generated dll can have
   different names on windows or macos).

-- Open a terminal and run yarpserver
   $ yarpserver

-- Open another terminal (lets call this the sender terminal) and type
   $ yarpdev --device test_grabber --mode ball

-- In the directory where you you built the dll (lets call this the receiver terminal), type
   $ yarp read /read


-- In another terminal connect the port as follow:
   $  yarp connect /grabber /read tcp+recv.portmonitor+type.dll+file.ascii_image
