deprecate_Runnable {#devel}
------------------

* Deprecate `os/{Runnable,RateThreadWrapper}`, `dev/Data{Source,Writer}{,2}`.
  Note: Unfortunately `ServerFrameGrabber` still uses them, at the moment
  deprecation warnings are disabled for this device.
