os_Header {#master}
---------

## Libraries

### yarp::os

* Introduced the `yarp::os::Header` class, intended to gradually replace the
  `yarp::os::Stamp` class to handle envelopes, maintaining backwards
  compatibility.
  The differences with the `Stamp` class:
  * `Header` has an extra field to handle the frame id, making it comparable
    with ROS [`std_msgs/Header`](http://docs.ros.org/en/api/std_msgs/html/msg/Header.html)
    message.
  * The counter uses unsigned int instead of int.
  * `Stamp::getMaxCount()` is replaced with `Header::npos`
  * `Stamp::getCount()` is replaced with `Header::count()`
  * `Stamp::getTime()` is replaced with `Header::timeStamp()`
