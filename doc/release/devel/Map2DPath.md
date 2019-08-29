Map2DPath {#devel}
-----------------------------

#### `libYARP_dev`
* yarp::dev::Map2DLocation refactored (thrifted)
* new class: yarp::dev::Map2DPath (thrifted)
* Added new Path methods to IMap2D interface, e.g. storePath, getPath, deletePath, renamePath...
* Added tests

#### `devices`
* yarp::dev::map2DClient, yarp::dev::map2DServer modified to handle yarp::dev::Map2DPath
