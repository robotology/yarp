bindings_addMissing {#master}
-----------------------

### Bindings

* Several previously inaccessible methods from motor interfaces are now correctly wrapped.
  This covers pretty much everything the pair `remote_controlboard`/`controlboardwrapper2`
  wraps and implements, only excluding calibration interfaces. In addition, two more
  interfaces can be accessed through the PolyDriver idiom: `IEncodersTimed` and `IMotor`.
