Navigation2DClient_thrift {#master}
-----------

Important Changes
-----------------

### Libraries

#### `yarp::dev`
* `INavigation2D` interface now derives from `IMap2D` interface

### devices

*  Added new device `navigation2D_nwc_yarp` which connects to:
   * `map2D_nws_yarp`  (thrift based)
   * `localization2D_nws_yarp` (thrift based)
   * `navigation2DServer` (rpc/vocab based)
* `Navigation2DClient` source code has been split into multiple files, one for each yarp interface. **No thrift is involved here, so it still connects to:**
   * `map2DServer`  (rpc/vocab based) The new methods introduced by the IMap2D interface are intentionally left **not implemented**: they will be used only in the navigation2D_nwc_yarp device.
   * `localization2DServer` (rpc/vocab based)
   * `navigation2DServer` (rpc/vocab based)