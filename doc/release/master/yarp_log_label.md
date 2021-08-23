yarp_log_label {#master}
-----------

Important Changes
-----------------

### YARP_os

* yarp::os::LogForwarder now checks the environment variable `YARP_LOG_PROCESS_LABEL`. 
  If found, the label is appended to the executable name in the log port name. e.g. /log/hostname/yarpdev[mylabel]/123123

### YARP_run

* yarprun now checks the environment variable `YARP_LOG_PROCESS_LABEL`. 
  If found, the label is appended to the executable name in the log port name. e.g. /log/yaprunname/yarpdev[mylabel]/123123
