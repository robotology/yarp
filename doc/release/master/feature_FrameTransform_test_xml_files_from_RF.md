feature_FrameTransform_test_xml_files_from_RF {#master}
---------------

### Devices

#### `frameTransformServer` + `frameTransformClient`

* Now both `frameTransformClient` and `frameTransformServer` will search for "test" configurations not relying on an absolute path passed as a parameter but will take advantage of `yarp::os::ResourceFinder` using a `file name` and a `context` passed to them in this way:
```
> yarpdev --device frameTransformClient --testxml_from testconfig.xml --testxml_context test_folder
```
