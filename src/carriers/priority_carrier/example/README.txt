
A simple example shows the arbitration of two different modules connected to
the same port of "yarpview"
****************************************************************************

 __________________
|                  | (/Ball/grabber)                          __________
| fakeFrameGrabber  >----------------------------------->|   |          |
|__________________|                 ------------------->|---> yarpview |
                                     -           .......>|   |__________|
 __________________                  -           .
|                  | (/Line/grabber) -           .
| fakeFrameGrabber  >-----------------           .
|__________________|                             .
                                                 .
                                                 . (an auxiliary connection)
                                                 .
 __________________                              .
|                  | (/Selector/grabber)         .
| fakeFrameGrabber  >.............................
|__________________|


The connection from the third "fakeFrameGrabber" module (/Selector/grabber)
is used as an auxiliary connection to activate "/Ball/grabber" and deactivate
 "/Line/grabber". The module runs in low frequency (every 3 seconds) and plays
as a selector between two other modules. As the result, yarpview switches
between different images coming from "/Ball/grabber" and "/Line/grabber"
periodically.

To run the application, open a terminal and type :

$ cd $YARP_ROOT/src/carriers/priority_carrier/example
$ yarpmanager --application TestArbitrator.xml --run --connect

To stop the application, in the yarpmanager's console type:

>> stop
>> exit

You are also able to see the run-time stimulation levels using "yarpscope".
