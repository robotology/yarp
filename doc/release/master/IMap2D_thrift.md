IMap2D_thrift {#master}
-----------

Important Changes
-----------------

### Libraries

#### `yarp::dev`
* Added the following three methods to `yarp::dev::Nav2D::IMap2D`
   getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations)
   getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas)
   getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths)

### devices

*  Added new thrift interface `IMap2DMsgs`.
*  Added new device `map2D_nwc_yarp` which can connect only to `map2D_nws_yarp` (and not `Map2DServer`).
   This because the communication between `map2D_nwc_yarp` and `map2D_nws_yarp` uses the new thrift
   interface `IMap2DMsgs`, while `Map2DClient` and `Map2DServer` communicate through an hand-written protocol
   based on yarp vocabs.
*  Added a warning to device `map2DClient`. The device will be deprecated in the next release. 
