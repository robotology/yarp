fix_frameTransformClient_part2 {#yarp_3_5}
-------------------

### Devices

#### `FrameTransformClient`

* timestamp is now set also for static_transforms

#### `FrameTransformSetMultiplexer`

* fixed race condition issue in `deleteTransform()` method.

#### `FrameTransformStorage`

* fixed race condition issue: added extra mutex to protect the periodicThread from set/get/delete operations
* the periodic thread can be now stopped/started on request by
  methods `IFrameTransformStorageUtils::stopStorageThread` and `IFrameTransformStorageUtils::startStorageThread`.

#### `FrameTransformContainer`

* added new iterator class. It iterates only on valid transforms.
* when a transform is deleted, it is marked as invalid. The transform is then removed
  during the next iteration of the checkAndRemoveExpired()

### lib_yarpDev

* added methods `startStorageThread()` and `stopStorageThread()` to interface `yarp::dev::IFrameTransformStorageUtils`

### lib_yarpMath

* added method `yarp::math::Quaternion::isValid()`
* added method `yarp::math::FrameTransform::isValid()`

### tests

* Test `FrameTransformClientTest` has been improved.

