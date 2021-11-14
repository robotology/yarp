version_compare_macro {#master}
-----------

### YARP_conf

* The `YARP_VERSION_COMPARE` macro has been added to version.h to simplify how
  YARP versions are compared e.g. in `#if` guards. For instance, the condition
  `#if YARP_VERSION_COMPARE(>=, 3, 6, 0)` will yield true if the current YARP
  version is equal to or greater than 3.6.0.
