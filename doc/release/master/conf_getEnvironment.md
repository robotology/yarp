conf_getEnvironment {#master}
-------------------

### Libraries

#### `conf`

* Added the `yarp/conf/environment.h` header.
* Added the following functions:
  * `yarp::conf::environment::getEnvironment()`
  * `yarp::conf::environment::setEnvironment()`
  * `yarp::conf::environment::unsetEnvironment()`

#### `os`

* The method `yarp::os::getenv()` is now deprecated in favour of `std::getenv()`

##### `Network`

* `getEnvironment()` is now deprecated in favour of `yarp::conf::environment::getEnvironment()`
* `setEnvironment()` is now deprecated in favour of `yarp::conf::environment::setEnvironment()`
* `unsetEnvironment()` is now deprecated in favour of `yarp::conf::environment::unsetEnvironment()`
