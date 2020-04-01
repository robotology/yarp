copyPortable_const {#master}
------------------

### Libraries

#### `os`

##### `Portable`

* The first argument of `copyPortable` is now `const`. The new signature is:
  ```
    bool copyPortable(const PortWriter& writer, PortReader& reader)
  ```
