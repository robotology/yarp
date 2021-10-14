Navigation2D_nws_yarp {#master}
-----------

Important Changes
-----------------

### devices

* Added new device `navigation2D_nws_yarp`
* added deprecation warning to `navigation2DServer`
* added new thrift interface `INavigation2DMsgs`
* `navigation2D_nwc_yarp` now communicates with `navigation2D_nws_yarp` using the thrift interface.
* Removed any previous support is given to hybrid configurations (i.e. nwc->NAvigationserver or nws->Navigationclient).
