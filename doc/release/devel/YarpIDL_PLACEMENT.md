YarpIDL_PLACEMENT {#devel}
-----------------

### CMake

* The `yarp_idl_to_dir` command now accepts the `PLACEMENT` option:
   * `MERGED`:        headers and sources in `<OUTPUT_DIR>/<namespace>`
   * `SEPARATE`:      headers in `<OUTPUT_DIR>/include/<namespace>` sources in `<OUTPUT_DIR>/src/<namespace>`
   * `SEPARATE_EVEN`: alias for `SEPARATE`
   * `SEPARATE_ODD`:  headers in `<OUTPUT_DIR>/include/<namespace>` sources in `<OUTPUT_DIR>/src`
  For backwards compatibility the default value is `SEPARATE_ODD`.

* After calling the `yarp_add_idl` method, the `YARP_ADD_IDL_INCLUDE_DIR`
  variable contains the include directory for using the header files

### Tools

#### `yarpidl_trift`

* yarpidl_thrift now places the source files according to the "separate" layout.
