yarpidl_thrift_list_serialization {#master}
---------------------------------

New Features
------------

### Libraries

#### `os`

##### `idl/WireReader`

* Added `readBlock` method

##### `idl/WireWriter`

* Added `writeBlock` method
* Added `skip_tag` flag to all write methods.


### Tools

#### `yarpidl_thrift`

* Improved serialization for lists of basic types.
  When possible, instead of being serialized element by element, lists of basic
  types are now serialized as blocks.
