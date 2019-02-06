# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

### A set of desirable yarp options.
## Assuming your build directory is in yarp/build, use as:
## cd build
## cmake -C ../cmake/full-options.cmake ../

#libYARP_math
option(CREATE_LIB_MATH "Math library" TRUE)


# GUIS
option(CREATE_GUIS "Do you want to compile GUIs" ON)
