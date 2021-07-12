vocab32_part2 {#master}
-------------

## Libraries

### `os`

* `yarp::os::createVocab()` is deprecated in favour of `yarp::os::createVocab32()`
* `yarp::os::Vocab::encode()` is deprecated in favour of `yarp::os::Vocab32::encode()`
* `yarp::os::Vocab::decode()` is deprecated in favour of `yarp::os::Vocab32::decode()`
* `yarp::os::Value::isVocab()` is deprecated in favour of `yarp::os::Value::isVocab32()`
* `yarp::os::Value::asVocab()` is deprecated in favour of `yarp::os::Value::asVocab32()`
* `yarp::os::Value::makeVocab()` is deprecated in favour of `yarp::os::Value::makeVocab32()`
* `yarp::os::Bottle::addVocab()` is deprecated in favour of `yarp::os::Bottle::addVocab32()`
* `yarp::os::WireReader::getIsVocab()` is deprecated in favour of `yarp::os::WireReader::getIsVocab32()`


#### `Bottle`

* Added `addVocab32()` overloads accepting 4 chars and string


#### `Value`

* Added `makeVocab32()` overload accepting 4 chars


#### `idl/WireWriter`

* Add `addVocab32()` overloads accepting 4 chars and string
