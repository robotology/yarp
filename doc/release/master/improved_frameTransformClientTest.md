improved_frameTransformClientTest {#master}
-------------------

### Tests

* improved frameTransformClientTest for better understanding issues in CI: now using new param FrameTransform_verbose_debug=1

### Devices

#### FrameTransformClient

* added optional parameter --FrameTransform_verbose_debug

#### `FrameTransformStorage` `FrameTransformContainer`

* FrameTransformStorage, FrameTransformContainer now show advanced debug info when parameter FrameTransform_verbose_debug=1
* all xml files now expose the parameter FrameTransform_verbose_debug (default = 0)

