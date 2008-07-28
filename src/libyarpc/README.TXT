This is a start at a C interface to YARP.

It is a step towards enabling a more stable ABI.

It will also make more systematic SWIG wrappers easier (multiple inheritance hard to 
translate to a lot of languages).

For now, it will be just a wrapper around the C++ implementation.  If it turns out to be 
useful, the implementation layering could be reversed - the C++ classes could be a 
wrapper around the C implementation.

This library is not integrated with main project build yet, you have to compile it 
separately.
  --paulfitz
