addFilesystem  {#devel}
-------------

#### YARP_conf

* Added `yarp::conf::filesystem` utilities(#1855).

#### YARP_os

* Deprecated filesystem utilities of `Network`.
  `getDirectorySeparator` and `getPathSeparator` have been deprecated in
  favour of `yarp::conf::filesystem::preferred_separator`
  and `yarp::conf::filesystem::path_separator`respectively.

