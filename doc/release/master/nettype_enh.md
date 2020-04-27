nettype_enh {#master}
-----------

### Libraries

#### `os`

##### `NetType`

* The following methods were added:
    * `static std::string toString(yarp::conf::float32_t)`
    * `static std::string toString(yarp::conf::float64_t)`
    * `static yarp::conf::float32_t toFloat32(const std::string&)`
    * `static yarp::conf::float64_t toFloat64(const std::string&)`
    * `static yarp::conf::float32_t toFloat32(std::string&&)`
    * `static yarp::conf::float64_t toFloat64(std::string&&)`
    * `static std::string toHexString(long)`
    * `static std::string toHexString(unsigned int)`
