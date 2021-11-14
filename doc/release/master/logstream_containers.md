logstream_containers {#master}
-----------

### YARP_os

* yarp::os::LogStream has learned to chain most standard containers as well as
  arrays and tuples. In addition to the previously supported `std::vector`, it
  is now also possible to call `yInfo() << collection` where the collection is
  a `std::array`, `std::list`, `std::set`, `std::map`, C-array, `std::tuple`...
