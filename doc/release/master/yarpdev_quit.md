yarpdev_quit {#master}
------------

### Tools

#### `yarpdev`

* The terminator port now tries to use the default value for `--name` accepted
  by the device.
  For example `yarpdev --device test_grabber` will open the port `/grabber/quit`
  instead of `/test_grabber/quit`.
