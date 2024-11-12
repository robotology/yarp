YARP <yarp-3.9> (UNRELEASED)                                         {#yarp_3_9}
============================

[TOC]

YARP <yarp-3.9> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.9%22).


Fixes
-----

### `bindings`

* Avoid to include unsupported characters like `~dev` in pip version when YARP_PYTHON_PIP_METADATA_INSTALL CMake option is enabled.

### Devices

* Fix NWS extra config wrapper parameter.

### Libraries

#### `libYARP_os`

* Allow expressions in logging component parameters, e.g. `yCInfo(flag ? COMP1 : COMP2) << "message";`.

#### `yarpidl_thrift`

* Add support for `yarpidl_thrift` to manipulate paths longer than 260 characters on Windows.

### GUIs

#### `yarpviz`

* Support compilation with Graphviz >= 9.

