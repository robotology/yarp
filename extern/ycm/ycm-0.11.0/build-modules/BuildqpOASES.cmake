#.rst:
# BuildECM
# --------
#
# qpOASES

#=============================================================================
# Copyright 2014 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

# TYPE SVN STYLE COIN_OR
set(YCM_SVN_COIN_OR_USERNAME "" CACHE STRING "Username to use for coin-or svn repositories")
set(YCM_SVN_COIN_OR_PASSWORD "" CACHE STRING "Password to use for coin-or svn repositories")
set(YCM_SVN_COIN_OR_BASE_ADDRESS "https://projects.coin-or.org/svn/" CACHE INTERNAL "Address to use for coin-or svn repositories")
mark_as_advanced(YCM_SVN_COIN_OR_USERNAME YCM_SVN_COIN_OR_PASSWORD)

# qpOASES
include(YCMEPHelper)

ycm_ep_helper(qpOASES TYPE SVN
                      STYLE COIN_OR
                      REPOSITORY qpOASES/stable/3.0
                      TRUST_CERT 1
                      COMPONENT external
                      INSTALL_COMMAND "")
