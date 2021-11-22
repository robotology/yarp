fix_logForwarder_lost_msgs {#master}
-----------------------

## Libraries

### `os`

#### `Log`

* LogForwarder: now using yarp::os::PortWriterBuffer to prevent message loss