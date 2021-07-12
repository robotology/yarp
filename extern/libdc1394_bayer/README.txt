libdc1394_bayer
---------------

This is a two-file fragment from libdc1394.  Read conversions.h to see
adaptation to MSVC.  It is used by the optional bayer carrier.  If a system
version is found, it will be preferred over this. you can use the
YARP_USE_SYSTEM_LIBDC1394 flag if you still want to use this version.

Original is here:
  http://libdc1394.git.sourceforge.net/git/gitweb.cgi?p=libdc1394/libdc1394;a=blob_plain;f=libdc1394/dc1394/bayer.c;hb=HEAD

Licence: LGPL-2.1-or-later
