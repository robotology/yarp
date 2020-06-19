fix_qos_invalid_dscp {#master}
--------------------

### Libraries

#### `os`

##### `Port`

* Passing an invalid string when setting the QoS by DSCP no longer sets it to 0.
