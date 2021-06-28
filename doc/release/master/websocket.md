new carrier added {#master}
-----------------


### Carriers

#### WebSocket

* added support for websocket carrier, now it can be reached at port address. 
  It works only on the receiving side, and it works on an http request
  that starts with `"GET /?ws"` (for example 'http://127.0.0.1:10002/?ws'
  or 'ws://127.0.0.1:10002/?ws' if a read port is opened by yarp).

* The websocket, on the browser side, needs to communicate using the YARP
  protocol. A library will be added in another repository.
