fake_test_failure {#master}
-----------------------

### devices
* added option `test_open_failure` to all fake devices, in order to simulate a failure during the opening of the device.
  Useful for debug purposes, especially when testing if the wrappers/nws behaves correctly if the attached device fails to open.
