clearMapTemporaryFlags {#master}
-----------------------

### yarp::dev

* added method `yarp::dev::Nav2D::MapGrid2D::clearMapTemporaryFlags()`
* added the following methods to `yarp::dev::Nav2D::IMap2D` interface: `clearAllMapsTemporaryFlags()`, `clearMapTemporaryFlags(std::string map_name)`
* the flag `VOCAB_NAV_CLEAR_X`, defined in file `ILocalization2D.h` has been renamed to `VOCAB_NAV_CLEARALL_X` to avoid confusion with flag `VOCAB_NAV_DELETE_X`.
  Indeed `VOCAB_NAV_CLEARALL_X` clears all data belonging to the same category (all maps, all locations, all areas, etc), while `VOCAB_NAV_DELETE_X`, deletes
  a single entity (a map with name x, a location with name x etc).

### devices
* `Navigation2DClient` and `Navigation2DServer` updated accordingly.
