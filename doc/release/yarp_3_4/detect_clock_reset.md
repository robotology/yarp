detect_clock_reset {#yarp_3_4}
-----------------------

### Libraries

#### `os`

* If in a `yarp::os::NetworkClock` a clock reset is detected, fill the gap between the waiter and the time published by the network clock port. A network clock reset is defined as a jump in the past of the time published by the network clock port. This fix avoids that all the threads that are waiting a `yarp::os::NetworkClock::delay` call on that network clock remain blocked when a time reset occurs.
