Property_refactor {#devel}
-----------------

### os

#### `Searchable`

* Added move constructor and assignment operator.


#### `Property`

* Added move constructor and assignment operator.
* The `yarp::os::Property` constructor using 'hash_size' was deprecated (it was
  already unused since the internal structure was ported to use std::map).
