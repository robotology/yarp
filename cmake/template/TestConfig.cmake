# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# If we are using ACE on MSVC, the RUN_TESTS target will fail
# unless we add the ACE dlls to our path (or the user does
# that globally themselves).
set(ACE_ACE_LIBRARY_DEBUG "@ACE_ACE_LIBRARY_DEBUG@")
set(ACE_ACE_LIBRARY_RELEASE "@ACE_ACE_LIBRARY_RELEASE@")
if (ACE_ACE_LIBRARY_DEBUG)
  get_filename_component(ACE_DEBUG "${ACE_ACE_LIBRARY_DEBUG}" PATH)
  set(ENV{PATH} "$ENV{PATH};${ACE_DEBUG}")
endif()
if (ACE_ACE_LIBRARY_RELEASE)
  get_filename_component(ACE_RELEASE "${ACE_ACE_LIBRARY_RELEASE}" PATH)
  set(ENV{PATH} "$ENV{PATH};${ACE_RELEASE}")
endif()
