nws_yarp_consistency {#master}
-------------------

### Devices

#### `controlBoard_nws_yarp`

* A consistency checker prevented from broadcasting any motor state if, among
  the latest retrieved encoder timestamps, there were two whose difference was
  greater than one second. This helped to obtain a sensible average timestamp
  for consumers (such as remotecontrolboard), but was too conservative if state
  was expected to be published anyway as long as there is at least one subdevice
  still reporting valid data. This has been changed to compare against the
  current timestamp instead, and thus support the latter scenario.

