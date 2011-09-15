Some quick unofficial notes on usage.

yarpmanager (v0.9)
===========

Make sure you have a name server running:
$ yarpserver

And a "run" server (be aware of security implications):
$ yarp run --server /local

Then you can try this test:

$ cd .../src/yarpmanager/tests
$ yarpmanager
>> help
>> list app 
     # you should see EyesViewer, FakeEyes
>> load app EyesViewer
>> run
>> connect
     # you should see moving red bar
>> stop
>> exit

gyarpmanager (v0.9)
============
$ cd .../src/yarpmanager/tests
$ gyarpmanager
+ click the triangle beside Applications, then double-click EyesViewer,
+ select all the module rows, and click the gears button (run).
+ select all the connection rows, and click the plug-in button (connect).
+ select all the module rows, and click the x button (stop).
+ click the circular-arrow button (refresh).


