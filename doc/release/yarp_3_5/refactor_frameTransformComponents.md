refactor_frameTransformComponents {#yarp-3.5}
-------------------

### FrameTransform architecture components

#### `yarp::dev`

The following components have been refactored in order to easily manage ports names and avoid errors in doing so:
* frameTransformSet_nwc_yarp
* frameTransformSet_nws_yarp
* frameTransformGet_nwc_yarp
* frameTransformSet_nws_yarp

The xml files containing the configurations for the `frameTransformClient` and the `frameTransformServer` have been updated
accordingly.
