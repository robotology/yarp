    __  __ ___     ____   ____ 
    \ \/ //   |   / __ \ / __ \
     \  // /| |  / /_/ // /_/ /
     / // ___ | / _, _// ____/
    /_//_/  |_|/_/ |_|/_/        Yet Another Robot Platform

YARP
====

[![YARP Homepage](https://img.shields.io/badge/YARP-Yet_Another_Robot_Platform-orange.svg)](http://wiki.icub.org/yarpdoc/)
[![Latest Release](https://img.shields.io/github/tag/robotology/yarp.svg?label=Latest Release)](https://github.com/robotology/yarp/tags)

YARP is a library and toolkit for communication and device interfaces,
used on everything from humanoids to embedded devices.

Installation
------------

See full instructions at http://wiki.icub.org/yarpdoc/install.html

On OSX:

    brew install yarp

On Linux:

    sudo apt-get install cmake libace-dev
    git clone https://github.com/robotology/yarp
    cd yarp && mkdir build && cd build && cmake .. && make
    sudo make install  # Optional

On Windows:
 * Binaries at http://sourceforge.net/projects/yarp0/files/yarp2/

Regular YARP builds use the ACE library.  On Linux and OSX,
YARP can be compiled without ACE by adding "-DSKIP_ACE=TRUE" 
when running cmake.


Tutorials
---------

There's a comprehensive list of tutorials here:

 * http://wiki.icub.org/yarpdoc/tutorials.html

License
-------

Material included in YARP is Copyright of Istituto Italiano di Tecnologia and
RobotCub Consortium. YARP is released under the terms of the LGPL v2.1 or
later. See the file COPYING for details.

Certain optional parts of YARP have dependencies that have more 
requirements than the LGPL:
 + libYARP_math uses the GNU Scientific Library, under the GPL.
 + devices urbtc, vfw, dimax_u2c have components that are under the GPL.

Status
------

[![Build Status (Linux/OSX)](https://img.shields.io/travis/robotology/yarp/master.svg?label=Build Status (Linux/OSX))](https://travis-ci.org/robotology/yarp)
[![Build status (Windows)](https://img.shields.io/appveyor/ci/paulfitz/yarp/master.svg?label=Build Status (Windows))](https://ci.appveyor.com/project/paulfitz/yarp)
and [more](http://dashboard.icub.org/index.php?project=YARP)

[![Coverage Status](https://img.shields.io/coveralls/robotology/yarp.svg?label=Coverage)](https://coveralls.io/r/robotology/yarp)

[![Issues](https://img.shields.io/github/issues/robotology/yarp.svg?label=Issues)](https://github.com/robotology/yarp/issues)
