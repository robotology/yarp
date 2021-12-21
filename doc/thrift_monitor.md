Thrift IDL in YARP: monitoring a connection                    {#thrift_monitor}
===========================================

It is possible to monitor messages sent to a thrift service by using
the [portmonitor carrier](\ref portmonitor).


## Using thrift client

With YARP, it is possible to generate a `Monitor` class that can be used to
inspect which messages are sent to the service, by passing the
`yarp.monitor = "true"` annotation.
For example, suppose we have the following structure in `service.thrift`

\snippet example/idl/thriftMonitor/Demo.thrift Demo.thrift Old

that is used to produce a static library

\snippet example/idl/thriftMonitor/CMakeLists.txt CMakeLists.txt Protocol Library

a server

\snippet example/idl/thriftMonitor/DemoServer.cpp DemoServer.cpp
\snippet example/idl/thriftMonitor/CMakeLists.txt CMakeLists.txt Server

and a client

\snippet example/idl/thriftMonitor/DemoClient.cpp DemoClient.cpp Old
\snippet example/idl/thriftMonitor/CMakeLists.txt CMakeLists.txt Client


It is possible to enable the generation of the `FakeService::Monitor` class in
this way:

\snippet example/idl/thriftMonitor/Demo.thrift Demo.thrift New
\snippet example/idl/thriftMonitor/CMakeLists.txt CMakeLists.txt Monitor

It is then possible to modify the connection in the client to use the new
portmonitor

\snippet example/idl/thriftMonitor/DemoClient.cpp DemoClient.cpp New

In this way, the client will send a message to the `/monitor` port for every
command sent or reply received. The name of the monitor port can be changed by
passing `+monitor.<monitor port>` option to the carrier, for example

\snippet example/idl/thriftMonitor/DemoClient.cpp DemoClient.cpp New2


## Using yarp rpc or custom clients

The same thing can be done also running `yarp rpc` in client mode. This is
usually useless, but it can be helpful for debugging.
Also the same method can be used if the client is written manually, and not
using thrift,

```bash
yarp rpc --client /demo/yarprpc
```

and connecting it in a different terminal, using the right connection argument:

```bash
yarp connect /demo/yarprpc /demo/server tcp+send.portmonitor+type.dll+file.demo_monitor
```

If enabled, in order to monitor a connection using `yarp rpc`, it is also
possible to use the generic version of the portmonitor (this does not work with
thrift clients, but only with `yarp rpc` or other clients using
yarp::os::Bottle and yarp::os::CommandBottle):

```bash
yarp connect /demo/yarprpc /demo/server tcp+send.portmonitor+type.dll+file.rpc_monitor
```
