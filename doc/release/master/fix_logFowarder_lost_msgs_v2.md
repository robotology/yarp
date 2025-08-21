fix_logForwarder_lost_msgs_v2 {#master}
-----------------------

## Libraries

### `os`

#### `Log`

* LogForwarder: now using a separated thread (`class ThreadedPort`) to prevent the loss of log messages during stress condition