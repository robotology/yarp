YarpPlugin_absolute {#master}
-------------------

### CMake

#### `yarp_prepare_plugin`

* Including current directory (either explicitly, or using
  `CMAKE_INCLUDE_CURRENT_DIR`) is no longer required by the generated files.
* `INCLUDE` must now be an existing file, either using a path relative to
  current directory, or an absolute path. Using a path relative to one of the
  include directories is deprecated.
