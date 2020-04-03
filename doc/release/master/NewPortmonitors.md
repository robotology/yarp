NewPortmonitors {#master}
------------------

### Carriers

#### `DepthImage2`

* `DepthImage2` transforms a depth image (`VOCAB_PIXEL_FLOAT` format) into a rgb image, using a precomputed colormap.
Example: ```yarp connect /grabber/depth:o /yarpview/img:i tcp+recv.portmonitor+type.dll+file.depthimage2```

#### `SegmentationImage`

* `SegmentationImage` transforms a segmentation image (`VOCAB_PIXEL_MONO` or `VOCAB_PIXEL_MONO16`) into a rgb image, using a precomputed colormap.
Example: ```yarp connect /segmentationimage:o /yarpview/img:i tcp+recv.portmonitor+type.dll+file.segmentationimage```


