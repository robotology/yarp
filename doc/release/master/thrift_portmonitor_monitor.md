thrift_portmonitor_monitor {#master}
--------------------------

New Features
------------

### Libraries

#### `os`

* Added `CommandBottle` class.
  This is a class that contains both the command and the reply.
  The only advantage of this class compared to using two bottles is that
  portmonitors are able to know, in the `updateReply` method, what was the
  request for the reply received.

##### `ConnectionWriter`

* Replaced `getBuffer` with const/non-const versions

##### `ConnectionReader`

* `flushWriter` is now an interface method.


##### `idl/WireReader`

* Added `getReader()` method.

##### `idl/WireWriter`

* Flush the connection when the object is destroyed.


### Tools

#### `yarpidl_thrift`

* The return variable is again a class member.
  The updateReply method of a portmonitor is not called on the same thread
  that reads from the connection, therefore the return variable, as a
  thread_local variable, causes it to be empty.
  The flush performed in the WireWriter dtor, ensures that any
  appendExternalBlock contained in helper serializationis actually written
  before the helper is destroyed.

* When rpc command is incomplete or contains extra args, the argument fails.

* It is no longer possible to have a service function that starts with the same
  name as a different function (e.g. `get` and `get_all`).

* Enum handling was heavily refactored.
  It is now possible to use `yarp.name` and `yarp.includefile` also for enums.

* The 'yarp.nested' annotation was fixed. It is also now possible to annotate a
  struct with the `yarp.bottlesize` annotation, when the struct is not
  serialized as a bottle but as a fixed size bottle-like structure (for example
  other yarpidl_thrift generated structs).

* The editor for structs is now disabled by default. The `yarp.editor`
  annotation should be passed to the struct in order to enable it.

* Added generation of portmonitor for services.
  See [documentation](\ref thrift_monitor) for details.
