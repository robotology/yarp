deprecated_internal {#master}
-------------------

# Important Changes

## Libraries

### `os`

#### `Bottle`

* `BOTTLE_TAG_INT` is deprecated in favour of `BOTTLE_TAG_INT32`
* `BOTTLE_TAG_DOUBLE` is deprecated in favour of `BOTTLE_TAG_FLOAT64`
* `BOTTLE_TAG_VOCAB` is deprecated in favour of `BOTTLE_TAG_VOCAB32`
* `addInt` is deprecated in favour of `addInt32`
* `addDouble` is deprecated in favour of `addFloat64`

#### `ConnectionReader`

* `expectInt` is deprecated in favour of `expectInt32`
* `expectDouble` is deprecated in favour of `expectFloat64`

#### `ConnectionWriter`

* `appendInt` is deprecated in favour of `appendInt32`
* `appendDouble` is deprecated in favour of `appendFloat64`

#### `Value`

* `isInt` is deprecated in favour of `isInt32`
* `isDouble` is deprecated in favour of `isFloat64`
* `asInt` is deprecated in favour of `asInt32`
* `asDouble` is deprecated in favour of `asFloat64`
* `makeInt` is deprecated in favour of `makeInt32`
* `makeDouble` is deprecated in favour of `makeFloat64`
