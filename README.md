     __  __ ___  ____   ____
     \ \/ //   ||  _ \ |  _ \
      \  // /| || |/ / | |/ /
      / // ___ ||  _ \ |  _/
     /_//_/  |_||_| \_\|_|
     ========================
    Yet Another Robot Platform


YARP
====

[![YARP Homepage](https://img.shields.io/badge/YARP-Yet_Another_Robot_Platform-orange.svg)](http://www.yarp.it/)
[![Latest Release](https://img.shields.io/github/release/robotology/yarp.svg?label=Latest%20Release)](https://github.com/robotology/yarp/releases)

YARP is a library and toolkit for communication and device interfaces,
used on everything from humanoids to embedded devices.

Installation
------------

See full instructions at http://www.yarp.it/install.html

On macOS:

    brew install robotology/formulae/yarp

On Linux:

    sudo apt-get install cmake libace-dev
    git clone https://github.com/robotology/yarp
    cd yarp && mkdir build && cd build && cmake .. && make
    sudo make install  # Optional

On Windows:
 * Binaries at https://github.com/robotology/yarp/releases

Regular YARP builds use the ACE library.  On Linux and macOS,
YARP can be compiled without ACE by adding "-DSKIP_ACE=TRUE" 
when running cmake.


Tutorials
---------

There's a comprehensive list of tutorials here:

 * http://www.yarp.it/tutorials.html

License
-------

Material included in YARP is Copyright of Istituto Italiano di Tecnologia (IIT),
RobotCub Consortium and other contributors.

 * Most YARP components are released under the terms of the BSD-3-Clause.
 * Some components are released under the BSD-3-Clause, but use LGPL 3rd party
   libraries, therefore they have to be considered LGPL if these libraries are
   linked statically.
 * Some components are released under the terms of the LGPL license, version 2.1
   or later.
 * A few optional parts of YARP are released under the terms of the GPL license,
   version 2 or later:
   + libYARP_math uses the GNU Scientific Library, under the GPL.
   + devices urbtc, vfw, dimax_u2c have components that are under the GPL.

See the file COPYING and LICENSE files for details.

Status
------

[![Build Status (Linux/macOS)](https://travis-ci.org/robotology/yarp.svg?branch=master)](https://travis-ci.org/robotology/yarp)
[![Build status (Windows)](https://ci.appveyor.com/api/projects/status/github/robotology/yarp?branch=master&svg=true)](https://ci.appveyor.com/project/robotology/yarp)
and [more](http://dashboard.icub.org/index.php?project=YARP)

[![Coverage Status](https://img.shields.io/coveralls/robotology/yarp.svg?label=Coverage)](https://coveralls.io/r/robotology/yarp)

[![Issues](https://img.shields.io/github/issues/robotology/yarp.svg?label=Issues)](https://github.com/robotology/yarp/issues)
