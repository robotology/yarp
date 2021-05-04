thrift_enhancement {#master}
------------------

## Libraries

### `os`

#### `WireReader`

* Vocabs are now accepted in readI32


## Tools

### `yarpidl_thrift`

* Types annotated with "yarp.type" are no longer serialized as nested.
  This improves the compatibility with existing services returning Bottle
  or other YARP types, and makes it possible to write a thrift file for
  existing protocols.
  On the other hand, this breaks compatibility with thrift services
  returning Bottles or other annotated types, generated before this
  commit. This is a breaking change, but should impact only a very
  limited number of cases, and it can be easily fixed by regenerating the
  files.
* Added support for vocabs in structs (#2476).
  Vocabs can be defined in this way:
  ```cpp
  typedef i32 ( yarp.type = "yarp::conf::vocab32_t" ) vocab

  struct FooStruct
  {
      1: vocab foo;
  }
  ```
* Added "yarp.nested" annotation for struct fields
  When defined = "true", this serialize a struct as a bottle instead of as a
  flat structure.
  For example, given these 2 structs:
  ```cpp
  struct Foo
  {
      1: double foo1;
      2: double foo2;
  }

  struct Bar
  {
      1: double bar1;
      2: Foo ( yarp.nested = "true" );
  }
  ```
  The `Bar` struct will be serialized as `bar1 (foo1 foo2)`, instead of as a
  flat struct `bar1 foo1 foo2`.
