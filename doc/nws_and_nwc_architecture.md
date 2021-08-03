Network Wrapper Server and Network Wrapper Client Architecture  {#nws_and_nwc_architecture}
=======

[TOC]

## Architecture Description

A device driver should derive from the abstract class [`yarp::dev::DeviceDriver`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1DeviceDriver.html). This interface contains methods common to all device drivers, such as open and close. Additionally a device driver should implement a selection of other interfaces that capture what it shares with other families of devices. For example a camera might implement [`yarp:dev::IFrameGrabber`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1IFrameGrabber.html) (a "raw" interface to the raw data) and/or [`yarp::dev::IFrameGrabberImage`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1IFrameGrabberImage.html) (a higher-level source of images).

Even if a device is normally opened as a *plugin* through the [`PolyDriver`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1PolyDriver.html) class, and the process instantiating the device driver *locally* does not know exactly what interfaces are implemented by the device, the process is still able to access to these interfaces by calling the [`view<T>()`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1DeviceDriver.html#ad2a1b10de5dda3160d687ebb5f20577c) template method.

YARP Network Wrapper Server/Client architecture extends this functionalities to a *remote* device.

A "*Network Wrapper Client*" (**NWC**) is a device driver (i.e. derived from the [`yarp::dev::DeviceDriver`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1DeviceDriver.html) class), and implements the same interfaces that a normal device would implement, but instead of being connected to a physical device, it is connected to a "*Network Wrapper Server*" (**NWS**), or equivalent for different middlewares.
A "*Network Wrapper Server*" (**NWS**) is a very thin layer that is attached (using the [`IWrapper::attach()`](http://www.yarp.it/git-master/classyarp_1_1dev_1_1IWrapper.html#ad7637d057fd26a32eb27f87a046fe095) method), and that should not have any logic inside, besides forwarding the interfaces to the client.
In this way, it is possible to develop an application that can work indifferently with a local or with a remote device, without any need to change the code of the application, but just adjusting a few parameters.

NWC and NWS are tightly coupled, and communicate using some protocol (which does not necessarily imply that YARP is used underneath, in the future there might be NWC and NWS couples using ROS or some other protocol to communicate).
NWS and NWC can be replaced by something else that uses the exact same protocol.

## Historical Notes and Rationale

The following sections are supposed to give a rationale to the changes that are currently happening in the backstage.

### YARP and ROS wrappers

Before YARP 3 and earlier releases, network wrappers were for YARP only.
Recently, with the introduction of YARP-ROS compatibility layer, wrappers started implementing also the compatibility with the ROS middleware, with tons of if and switch cases inside the code. Obviously, this doesn't scale to other middlewares.

There is no limit to how many wrappers can be attached to a device. We are therefore currently working to split the wrappers in two separate wrappers (one for YARP and one for ROS) that can be attached to the same device. Unfortunately this has the limitation that only one of them can be launched using `yarpdev`, and in order to use both of them contemporarily, it is necessary to use `yarprobotinterface`. We are currently considering whether to modify `yarpdev` to support attaching multiple wrappers.

### Naming

Before YARP 3.5, NWS have often been called including one or more of these terms:

* Wrapper
* Network Wrapper
* Server

NWC have been called including one or more of these terms in their names:

* Client
* Remote

The server and client nomenclature was not always the same, therefore it was not always immediate to understand which client should be connected to a server.

Moreover the name "server" is used also for servers that are not network wrappers, and the name "wrapper" is used also for devices that do not implement a client/server architecture (e.g. calibrators).

After several discussions we came out with this new nomenclature, that should avoid any ambiguity

* *Network Wrapper Client* (**NWC**)
* *Network Wrapper Server* (**NWS**)

We also decided that the couples of NWC and NWS should share a part of the name, so that it is easy to understand which devices can be connected.

Devices that are actually servers (i.e. servers that are devices, so that they can be started from `yarprobotinterface`) will not change their name.

We also decided to include in the name the middleware used underneath (even for YARP).


### Logic inside the wrappers

Before YARP 3.5, a few wrappers had "logic" inside. This means that using a device locally it is different, compared to using it remotely. This also means that a few wrappers, in order to avoid this differences, had to implement the same interfaces of the device wrapped.
In order to make things as small and clear as possible, network wrappers should not have any logic inside, and instead a separate wrapper (note: not a **network** wrapper here) should be created, and the NWS should be attached to this device instead of to the actual device.
NWS and NWC using a transport that is different from YARP, can require some conversion in order to match with the standards used by the middleware underneath. This kind of conversion is acceptable

### Definition of the protocol

Historically, the protocol used in YARP NWC and NWS was defined inside the code. This is not recommended, since it requires for that the server and client are kept in sync. For more recent devices, the definition of the protocol was moved to a thrift file, allowing to modify it in just one place.


## Recommendation

### Naming convention

The name of the device consists of 3 parts, separated by a `_` character:
* A "significative" common part shared by both devices (camelCase, with the first word lowercase)
* `nwc` or `nws`.
* The name of the middleware (lowercase) (`yarp`, `ros`, etc.)

For example:

| NWS | NWC |
|----|----|
| `controlBoard_nws_yarp` | `controlBoard_nwc_yarp` |
| `multipleAnalogSensor_nws_yarp` | `multipleAnalogSensor_nwc_yarp`

### Directories

NWC, NWS and protocol should be in the same directory

### Definition of the protocol

Using Thrift is currently the recommended way to do define the protocol between NWC and NWS

### Logic inside

NWC and NWS should not have any logic inside, besides the one required to replicate the interfaces remotely. Any logic should be moved to a different device.
Logic in NWS and NWC using a transport that is different from YARP is acceptable in order to match the data with what is expected from the middleware used underneath. This means that any conversion that is performed by the NVS must be matched by an inverse conversion on the NVC.

### Base class for NWS

The base class for all NWS should be yarp::dev::WrapperSingle because NWS are supposed to attach always to one and just one device.
