yAssert_ndebug {#devel}
--------------

### os

* `yAssert` is now a proper assertion macro (removed when `NDEBUG`, i.e.
  `Release builds). If the expression tested has side-effects, program behavior
  will be different depending on whether NDEBUG is defined. This may
  create Heisenbugs which go away when debugging is turned on.
