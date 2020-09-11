Zlib_depth {#master}
-----------------------

### CMake

* added optional `zlib` dependency

### yarp::sig

#### ImageFile.cpp

* added enum::image_fileformat::FORMAT_NUMERIC_COMPRESSED
* added methods: ImageReadFloat_CompressedHeaderless()
* added methods: ImageReadFloat_PlainHeaderless()
* fixed bug in: SavePNG() the output file was corrupted

### yarp guis

#### yarpdataplayer

* `yarpdataplayer` is now able to reproduce depth and depth_compressed data types

### yarp command line tools

#### yarpdatadumper

* `yarpdatadumper` now supports the following data outputs for parameter --type bottle(default), image, image_jpg, image_png, video, depth, depth_compressed


