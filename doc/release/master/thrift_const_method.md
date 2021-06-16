thrift_const_method {#master}
-------------------

## Libraries

### `os`

#### `Wire`

* Added `yarp()` const overload

#### `WireLink`

* The write method is now const. The signature is now:
  ```c++
  bool write(const PortWriter& writer, PortReader& reader) const
  ```

## Tools

### `yarpidl_thrift`

* It is now possible to specify a "const" qualifier to service methods using the
  `yarp.qualifier` annotation. For example, this code:
  ```
  service Foo {
    bool const_method() (yarp.qualifier = "const");
  }
  ```
  will generate a service `Foo` with this method:
  ```c++
  bool const_method() const
  ```
