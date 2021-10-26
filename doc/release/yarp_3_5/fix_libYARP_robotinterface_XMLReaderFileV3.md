fix_libYARP_robotinterface_XMLReaderFileV3 {#yarp_3_5}
-------------------

## Libraries

### `robotinterface`

#### `XMLReadefFileV3`


* Fixed bug in `yarp::robotinterface::impl::XMLReaderFileV3::Private::readParamTag`.
* The value passed to the `yarprobotinterface` executable using the correspondent `extern-name` (and stored in a `yarp::os::Value`) where
converted to `std::string` by the aforementioned function using the `yarp::os::Value::asString` instead of `yarp::os::Value::toString`.  
This resulted in empty strings every time a value with a type different from `std::string` was converted.
