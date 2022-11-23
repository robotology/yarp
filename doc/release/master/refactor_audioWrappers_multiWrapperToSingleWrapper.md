refactor_audioWrappers_multiWrapperToSingleWrapper {#master}
-------------------

### AudioRecorderWrapper and AudioPlayerWrapper

#### `yarp::dev`

The two wrappers have been modified in order to be compatible with `yarprobotinterface` without losing the
compatibility with `yarpdev` and the `subdevice` option

Now the two objects do not inherit from `yarp::dev::IMultipleWrapper` but from `yarp::dev::WrapperSingle`

This modification has been done in order to allow the two wrappers to implement both the `IWrapper` and the `IMultipleWrapper` interfaces
