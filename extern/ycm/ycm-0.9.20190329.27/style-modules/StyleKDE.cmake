#.rst:
# StyleKDE
# --------

#=============================================================================
# Copyright 2013-2017 Istituto Italiano di Tecnologia (IIT)
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


# TYPE GIT STYLE KDE
set(YCM_GIT_KDE_USERNAME "" CACHE STRING "Username to use for kde git repositories")
set(YCM_GIT_KDE_COMMIT_NAME "" CACHE STRING "Name to use for git commits for kde git repositories (if empty will use YCM_GIT_COMMIT_NAME)")
set(YCM_GIT_KDE_COMMIT_EMAIL "" CACHE STRING "Email address to use for git commits for kde git repositories (if empty will use YCM_GIT_COMMIT_EMAIL)")
set(YCM_GIT_KDE_BASE_ADDRESS "git://anongit.kde.org/" CACHE STRING "Address to use for kde git repositories")
set_property(CACHE YCM_GIT_KDE_BASE_ADDRESS PROPERTY STRINGS "git://anongit.kde.org/"
                                                             "ssh://git@git.kde.org/"
                                                             "git@git.kde.org:"
                                                             "kde:")
mark_as_advanced(YCM_GIT_KDE_USERNAME
                 YCM_GIT_KDE_COMMIT_NAME
                 YCM_GIT_KDE_COMMIT_EMAIL
                 YCM_GIT_KDE_BASE_ADDRESS)
