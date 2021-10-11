fix_controlBoardRemapper {#yarp_3_5}
-------------------

### Devices

#### `ControlBoardRemapper`

* fixed segfault which may occur if some methods (e.g. `ControlBoardRemapper::getControlModes(int *modes)`) are called
  and inside `fillCompleteJointVectorFromSubControlBoardBuffers` the joints arrays `m_bufferForSubControlBoard` are not yet allocated.

