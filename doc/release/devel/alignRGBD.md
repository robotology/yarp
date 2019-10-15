alignRGBD {#devel}
---------

### devices

#### realsense2

* Deprecated the `needAlignment` parameter in favour of `alignmentFrame`.
  The default behaviour has been maintaned, if not specified `alignmentFrame`
  is `RGB`. The allowed values are `RGB`, `Depth` and `None`.
