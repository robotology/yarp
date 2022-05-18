fix_yarpmotorgui_setPointBox {#yarp_3_6}
-------------------

### Tools

#### `yarpmotorgui`

* The target box for the joints sliders now lands where expected (see [#issue2824](https://github.com/robotology/yarp/issues/2824) for further details).
  The the `target` value for the slider was not being multiplied by the `sliderStep` value when computing the new X value for the `target box` rectangle.

