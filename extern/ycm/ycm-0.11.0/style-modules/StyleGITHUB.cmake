#.rst:
# StyleGITHUB
# -----------

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


# TYPE GIT STYLE GITHUB
set(YCM_GIT_GITHUB_USERNAME "" CACHE STRING "Username to use for github git repositories")
set(YCM_GIT_GITHUB_COMMIT_NAME "" CACHE STRING "Name to use for git commits for github git repositories (if empty will use YCM_GIT_COMMIT_NAME)")
set(YCM_GIT_GITHUB_COMMIT_EMAIL "" CACHE STRING "Email address to use for git commits for github git repositories (if empty will use YCM_GIT_COMMIT_EMAIL)")
set(YCM_GIT_GITHUB_BASE_ADDRESS "https://github.com/" CACHE STRING "Address to use for github git repositories")
set_property(CACHE YCM_GIT_GITHUB_BASE_ADDRESS PROPERTY STRINGS "https://github.com/"
                                                                "git://github.com/"
                                                                "ssh://git@github.com/"
                                                                "git@github.com:"
                                                                "github:")
mark_as_advanced(YCM_GIT_GITHUB_USERNAME
                 YCM_GIT_GITHUB_COMMIT_NAME
                 YCM_GIT_GITHUB_COMMIT_EMAIL
                 YCM_GIT_GITHUB_BASE_ADDRESS)
