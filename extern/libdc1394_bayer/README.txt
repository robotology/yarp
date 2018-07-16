libdc1394_bayer
---------------

This is a two-file fragment from libdc1394.  Read conversions.h to see
adaptation to MSVC.  It is used by the optional bayer carrier.  If a system
version is found, it will be preferred over this. you can use the
YARP_USE_SYSTEM_LIBDC1394 flag if you still want to use this version.

Original is here:
  http://libdc1394.git.sourceforge.net/git/gitweb.cgi?p=libdc1394/libdc1394;a=blob_plain;f=libdc1394/dc1394/bayer.c;hb=HEAD

Licence text:

 * 1394-Based Digital Camera Control Library
 *
 * Bayer pattern decoding functions
 *
 * Written by Damien Douxchamps and Frederic Devernay
 * The original VNG and AHD Bayer decoding are from Dave Coffin's DCRAW.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
