conf_environment {#master}
----------------

## Libraries

### `conf`

* Added the following functions in `<yarp/conf/numeric.h>:
  * `yarp::conf::numeric::from_string`
  * `yarp::conf::numeric::to_string`
  * `yarp::conf::numeric::to_hex_string`
* Added the header `<yarp/conf/string.h>` containing the following functions:
  * `yarp::conf::string::split`
  * `yarp::conf::string::join`
* Added `environment::path_separator`
* Added the following functions in `<yarp/conf/environment.h>`:
  * `yarp::conf::environment::get_string`
  * `yarp::conf::environment::set_string`
  * `yarp::conf::environment::get_bool`
  * `yarp::conf::environment::set_bool`
  * `yarp::conf::environment::get_numeric`
  * `yarp::conf::environment::set_numeric`
  * `yarp::conf::environment::split_path`
  * `yarp::conf::environment::join_path`
  * `yarp::conf::environment::get_path`
  * `yarp::conf::environment::set_path`
  * `yarp::conf::environment::unset`
  * `yarp::conf::environment::is_set`
* Deprecated `filesystem::path_separator` in favour of
  `environment::path_separator`
* Deprecated the following methods:
  * `yarp::conf::environment::getEnvironment` in favour of
    `yarp::conf::environment::get_string`
  * `yarp::conf::environment::setEnvironment` in favour of
    `yarp::conf::environment::set_string`
  * `yarp::conf::environment::unsetEnvironment` in favour of
    `yarp::conf::environment::unset`

### `os`

#### `NetType`

* Deprecated the following functions:
  * `toHexString` in favour of `yarp::conf::numeric::to_hex_string`
  * `toString` in favour of `yarp::conf::numeric::to_string`
  * `toInt`, `toFloat32` and `toFloat64` in favour of
    `yarp::conf::numeric::from_string`
