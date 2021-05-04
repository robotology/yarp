conf_dirs {#master}
---------


Deprecation and Behaviour Changes
---------------------------------

* A few default values for the environment variables used by yarp are now
  different:
  * Linux:
    * `YARP_CONFIG_DIRS` defaults to `/etc/xdg/yarp` to be compliant with the
      [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
  * Windows:
    * `YARP_DATA_DIRS` defaults to `%%ALLUSERSPROFILE%%\yarp`
    * `YARP_CONFIG_DIRS` defaults to `%%ALLUSERSPROFILE%%\yarp\config`


New Features
------------

## Libraries

### `conf`

* Added the new `dirs.h` file with methods to retrieve the important folders
* Added the following methods:
   * `std::string yarp::conf::dirs::home()`
   * `std::string yarp::conf::dirs::tempdir()`
   * `std::string yarp::conf::dirs::datahome()`
   * `std::vector<std::string> yarp::conf::dirs::datadirs()`
   * `std::string yarp::conf::dirs::confighome()`
   * `std::vector<std::string> yarp::conf::dirs::configdirs()`
   * `std::string yarp::conf::dirs::cachehome()`
   * `std::string yarp::conf::dirs::runtimedir()`
   * `std::string yarp::conf::dirs::yarpdatahome()`
   * `std::vector<std::string> yarp::conf::dirs::yarpdatadirs()`
   * `std::string yarp::conf::dirs::yarpconfighome()`
   * `std::vector<std::string> yarp::conf::dirs::yarpconfigdirs()`
   * `std::string yarp::conf::dirs::yarpcachehome()`
   * `std::string yarp::conf::dirs::yarpruntimedir()`

### `os`

#### `ResourceFinder`

* Deprecated methods with alternatives in `yarp::conf::dirs`.
  The following methods are now deprecated:
    * `yarp::os::ResourceFinder::getDataHome()`
    * `yarp::os::ResourceFinder::getDataHomeNoCreate()`
    * `yarp::os::ResourceFinder::getConfigHome()`
    * `yarp::os::ResourceFinder::getConfigHomeNoCreate()`
    * `yarp::os::ResourceFinder::getDataDirs()`
    * `yarp::os::ResourceFinder::getConfigDirs()`
  in favour of:
    * `yarp::conf::dirs::yarpdatahome()`
    * `yarp::conf::dirs::yarpconfighome()`
    * `yarp::conf::dirs::yarpdatadirs()`
  Warnings:
    * The return value of `yarpdatadirs()` is different
      (`std::vector<std::string>` instead of `std::string`).
    * The `yarpdatahome()` and `yarpconfighome()` do not create the directory,
      it must be created manually (for example with `yarp::os::mkdir_p()`) if
      required.
