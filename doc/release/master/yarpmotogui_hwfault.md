yarpmotorgui_hwfault {#master}
-----------------------

### libyarp_dev

* Added new interface IJointFault with method: IJointFault::getLastJointFault(int j, int& fault, std::string& message)

### devices


#### RemoteControlBoard

* RemoteControlBoard now implements IJointFault

#### ControlBoard_nws_yarp

* ControlBoard_nws_yarp now implements IJointFault

#### ControlBoardRemapper

* ControlBoardRemapper now implements IJointFault

#### fakeMotionControl

* fakeMotionControl now simulates an hardware fault when it receives a TorqueCommand > 1Nm

### GUIs

#### yarpmotorgui

* Modified yarpmotorgui to support a new page "Hardware Fault" which displays the internal hardware error





