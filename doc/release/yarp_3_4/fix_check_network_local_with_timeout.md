fix_check_network_local_with_timeout {#yarp_3_4}
-------------------------------

### yarp::os

#### `NameSpace`

* Fix `checkNetwork` timeout variant not working as `checkNetwork`
  when `NetworkBase::setLocalMode(true)` is called.