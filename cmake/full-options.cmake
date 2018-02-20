# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

# robot interface
option(CREATE_YARPROBOTINTERFACE "Do you want to compile yarprobotinterface?" ON)

# useful carriers
option(CREATE_OPTIONAL_CARRIERS "Compile some optional carriers" TRUE)
option(ENABLE_yarpcar_tcpros_carrier "tcpros carrier" TRUE)
option(ENABLE_yarpcar_rossrv_carrier "rossrv carrier" TRUE)
option(ENABLE_yarpcar_xmlrpc_carrier "xmlrpc carrier" TRUE)
option(ENABLE_yarpcar_portmonitor_carrier "portmonitor carrier" TRUE)
option(ENABLE_yarpcar_priority_carrier "priority carrier" TRUE)
option(ENABLE_yarpcar_human_carrier "human carrier" TRUE)



