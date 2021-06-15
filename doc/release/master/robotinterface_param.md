robotinterface_param {#master}
--------------------

## Libraries

### `robotinterface`

#### `XMLReader`

* The `getRobotFromFile` and `getRobotFromString` now accept a `Searchable` that
  is used to replace params with the attribute `extern-name`.
  The signatures are now:
  ```
      XMLReaderResult getRobotFromFile(const std::string& filename,
                                       const yarp::os::Searchable& config = yarp::os::Property());
      XMLReaderResult getRobotFromString(const std::string& filename,
                                         const yarp::os::Searchable& config = yarp::os::Property());
  ```

* DTD version is now 3.1.

## Tools

### `yarprobotinterface`

* Arguments taken from the command line are now handled and used to replace
  parameters marked with the attribute `extern-name`.
