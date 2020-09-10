debian_fixes {#yarp_3_4}
------------

Bug Fixes
---------

* The license check is now disabled for non-git checkouts.
* The check on the bindings folder in `YARPConfig.cmake` was removed.
* Fixed `yarprobotinterface` build on i386.
* Fixed several spelling errors around the code.
* User defined build flags are no longer overridden in subfolders.
* The `-f(debug|macro|file)-prefix-map` compile options are no longer
  exported in the `YARPConfig.cmake` file.
* Fixed build when the `-fsanitize` compile options is passed by the
  user.
