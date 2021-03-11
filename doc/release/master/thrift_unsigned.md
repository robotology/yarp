thrift_enhancement {#master}
------------------

## Libraries

### `os`

#### `Bottle`

 * Added `BOTTLE_TAG_VOCAB32` tag. This is the same as `BOTTLE_TAG_VOCAB`.
 * The usage of `BOTTLE_TAG_VOCAB` is not recommended in new code, use
   `BOTTLE_TAG_VOCAB32` instead.

#### `WireReader`

 * Added the following methods:
   * `readVocab32`
   * `readUI8`
   * `readUI16`
   * `readUI32`
   * `readUI64`
   * `readSizeT`
 * `readVocab` is now deprecated in favour of `readVocab32`

#### `WireWriter`

 * Added the following methods:
   * `writeVocab32`
   * `writeUI8`
   * `writeUI16`
   * `writeUI32`
   * `writeUI64`
   * `writeSizeT`
 * `writeVocab` is now deprecated in favour of `writeVocab32`


## Tools

### `yarpidl_thrift`

 * Thrift was updated to version 0.14.1.
 * Added support for `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `size_t`,
   `float32_t` and `float64_t` using annotated types.
   It is now possible to use these types in thrift by defining them using a
   `typedef`, for example:
   ```
   typedef i32 ( yarp.type = "yarp::conf::vocab32_t" ) vocab
   typedef i8 ( yarp.type = "std::uint8_t" ) ui8
   typedef i16 ( yarp.type = "std::uint16_t" ) ui16
   typedef i32 ( yarp.type = "std::uint32_t" ) ui32
   typedef i64 ( yarp.type = "std::uint64_t" ) ui64
   typedef i32 ( yarp.type = "size_t" ) size_t
   typedef double ( yarp.type = "yarp::conf::float32_t" ) float32
   typedef double ( yarp.type = "yarp::conf::float32_t" ) float64

   struct TestAnnotatedTypes
   {
     1: vocab a_vocab,
     2: ui8 a_ui8,
     3: ui16 a_ui16,
     4: ui32 a_ui32,
     5: ui64 a_ui64,
     6: float32 a_float32,
     7: float64 a_float64,
     8: size_t a_size;
   }
   ```
