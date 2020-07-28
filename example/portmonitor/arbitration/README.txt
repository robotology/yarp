Using port monitors as port arbitrators
=======================================

This example demonstrates how to use port monitor to coordinate different module
by arbitrating their connections in the input port.
Each port monitor can be set with a selection rule (constraint). The rule is simply
written in first order logic (boolean expression) based on some symbolic name. We
call these symbols as events. Port monitors from peer connections (the connections to
the same input port) can set (add) or unset (remove) events in a shared event record.
If an event is set, it has a true value in the corresponding boolean expression; otherwise
it is false. When data arrives to the port from a connection, the corresponding port
monitor evaluate the boolean expression and if it is true (the constraint is satisfied),
the data can be delivered to the port; otherwise it is discarded.

A port monitor can set the selection rule (for its own connection) using PortMonitor.setConstraint().
For example:

    PortMonitor.setConstraint("e_line_selected and not e_ball_selected")

Or to set or unset an event:

    PortMonitor.setEvent("e_line_selected")
    PortMonitor.unsetEvent("e_line_selected")

The following keywords are reserved and cannot be used for event's name:

    { true, false, not, and, or }



This simple example shows the arbitration of two different connections to the same port
of "yarpview" using Port Monitor object.

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



The connection from the third "fakeFrameGrabber" module (/Selector/grabber) is used as an
auxiliary connection to choose between "/Ball/grabber" and "/Line/grabber". To achieve
this, its selection rule in the 'selector_monitor.lua' is simply set to 'false'. The
module runs in low frequency (every 3 seconds) and plays as a selector between two other
modules by setting/unsetting some events which are used as constraint in 'ball_monitor.lua'
and 'line_monitor.lua'. As the result, yarpview switches between different images coming
from "/Ball/grabber" and "/Line/grabber" periodically.


To run the arbitration example:

    1) open a terminal and run yarpserver

    2) open another terminal and type:
    $ cd $YARP_ROOT/src/carriers/portmonitor_carrier/example/arbitration
    $ yarpmanager --application TestArbitratorImage.xml

    3) run and connect the ports, in the yarpmanager's console type:
    >> run
    >> connect

To stop the application, type:
    >> stop
    >> exit
