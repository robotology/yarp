Property_refactor {#devel}
-----------------

### os

#### `Property`

* The `yarp::os::Property` constructor using 'hash_size' was deprecated (it was
  already unused since the internal structure was ported to use std::map).
