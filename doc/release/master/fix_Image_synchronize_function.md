fix_Image_synchronize_function {#master}
--------------------

### Libraries

#### `sig`

##### `Image`

* Fixed the `yarp::sig::Image::synchronize` method that missed the pixel type and the orientation settings needed to properly use the `move assignment operator`
  * A `getTypeId` method has been added to `yarp::sig::ImageStore` to be able to read the <i>protected</i> value `type_id`
