map2D_nws {#master}
-------------------------

## Important Changes

### Devices

#### added new device `map2D_nws_yarp`
* The devices implements a yarp wrapper for a `map2DStorage` device.

#### added new device `map2D_nws_ros`
* The devices implements a ROS wrapper for a `map2DStorage` device.

#### added new device `map2DStorage`
* A device which is able to store navigation data, e.g `MapGrid2D`,`Map2DLocation` etc. It implements all the logic previously located inside device 'Map2DServer'.
