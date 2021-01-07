compressed_MapGrid2D {#master}
-----------------------

### yarp::dev
* added internal lossless compression for datatype `yarp::dev::Nav2D::MapGrid2D` using Zlib: the methods `yarp::os::Portable::write()`
  and `yarp::os::Portable::read()` handle the compression before sending data to the network.
* the method `MapGrid2D::enable_map_compression_over_network()` enables/disables the data compression over the network (default true).
* if Zlib library is not available, compression will be set to false.

### devices
* added rpc option `enable_maps_compression <0/1>` to `yarp::dev::Map2DServer`
