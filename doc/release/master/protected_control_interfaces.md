protected_control_interfaces {#master}
-----------

### `devices`

* `RPCMessagesParser`:  several calls to various motor control interfaces have been protected so that 
  if an interface is not available in the hardware device (i.e. the view() fails),
  the controlboardwrapper/yarprobotinterface does not segfaults.
