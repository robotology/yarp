Map2DArea_description {#master}
-----------

Important Changes
-----------------

### YARP_dev

* `Map2DArea` has been refactored: it now inherits from the thrift defined type Map2DAreaData.thrift
* `Map2DArea`, `Map2DLocation`, `Map2DPath` now include a `description` field which can be filled by the user for generic purposes
* The documentation of the thrift types: `Map2DAreaData`, `Map2DLocationData`, `Map2DPathData` has been updated.

### devices

*  Updated `map2DStorage` device to support the new `v3` file format, which includes the description for Map2DLocation, Map2DArea and Map2DPath
