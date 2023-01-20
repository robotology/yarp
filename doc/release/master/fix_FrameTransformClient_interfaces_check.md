fix/FrameTransformClient/interfaces_check {#master}
-------------------

### Tools

#### `frameTransformClient`

* Now, if the selected configuration xml file does not include a certain interface, the device is still correctly initialized but that interface won't be available. If a method that involves the missing interface is called, an error will be returned.
