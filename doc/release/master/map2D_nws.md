map2D_nws {#master}
-------------------------

## Important Changes

### libYARP_dev
* added the following two methods to interface `iMap2D`:
- saveLocationsAndExtras()
- loadLocationsAndExtras()
the methods have been implemented in:
- map2D_nws_yarp
- map2DClient
- map2DStorage

### harness_dev
* Added test `Map2DClientTest`

### Devices

#### bugfix in `map2DServer`
* Fixed issued preventing correct file save/load operations of locations data

#### added new device `map2D_nws_yarp`
* The devices implements a yarp wrapper for a `map2DStorage` device.

#### added new device `map2D_nws_ros`
* The devices implements a ROS wrapper for a `map2DStorage` device.

#### added new device `map2DStorage`
* A device which is able to store navigation data, e.g `MapGrid2D`,`Map2DLocation` etc. It implements all the logic previously located inside device 'Map2DServer'.
