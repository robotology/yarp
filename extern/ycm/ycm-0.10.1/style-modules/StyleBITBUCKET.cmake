#.rst:
# StyleBITBUCKET
# --------------

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


# TYPE HG STYLE BITBUCKET
set(YCM_HG_BITBUCKET_USERNAME "" CACHE STRING "Name and email to use for Bitbucket hg repositories")
set(YCM_HG_BITBUCKET_COMMIT_USERNAME "" CACHE STRING "Name to use for git commits for Butbucket hg repositories (if empty will use YCM_GIT_COMMIT_NAME <YCM_GIT_COMMIT_EMAIL>)")
set(YCM_HG_BITBUCKET_BASE_ADDRESS "https://bitbucket.org/" CACHE STRING "Address to use for Bitbucket mercurial repositories")
set_property(CACHE YCM_HG_BITBUCKET_BASE_ADDRESS PROPERTY STRINGS "https://bitbucket.org/"
                                                                  "ssh://hg@bitbucket.org/")
mark_as_advanced(YCM_HG_BITBUCKET_USERNAME
                 YCM_HG_BITBUCKET_COMMIT_USERNAME
                 YCM_HG_BITBUCKET_BASE_ADDRESS)
