# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_install(FILES yarp
             DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
             COMPONENT yarp-data)
