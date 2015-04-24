# Copyright: (C) 2014 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# If we are using ACE on MSVC, the RUN_TESTS target will fail
# unless we add the ACE dlls to our path (or the user does
# that globally themselves).
set(ACE_ACE_LIBRARY_DEBUG @ACE_ACE_LIBRARY_DEBUG@)
set(ACE_ACE_LIBRARY_RELEASE @ACE_ACE_LIBRARY_RELEASE@)
if (ACE_ACE_LIBRARY_DEBUG)
  get_filename_component(ACE_DEBUG ${ACE_ACE_LIBRARY_DEBUG} PATH)
  set(ENV{PATH} "$ENV{PATH};${ACE_DEBUG}")
endif()
if (ACE_ACE_LIBRARY_RELEASE)
  get_filename_component(ACE_RELEASE ${ACE_ACE_LIBRARY_RELEASE} PATH)
  set(ENV{PATH} "$ENV{PATH};${ACE_RELEASE}")
endif()
