// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PLATFORMSTDLIB_
#define _YARP2_PLATFORMSTDLIB_

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_stdlib.h>
#  include <ace/OS_NS_string.h>
#  include <ace/OS_NS_sys_stat.h>
#  include <ace/OS_NS_dirent.h>
#  include <ace/OS_NS_unistd.h>
#else
#  include <cstring>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ifndef ACE_OS
#    define ACE_OS
#  endif
#  ifndef ACE_stat
#    define ACE_stat struct stat
#  endif
#  ifndef ACE_DIRENT
#    define ACE_DIRENT dirent
#  endif
#  ifndef ACE_DIR
#    define ACE_DIR DIR
#  endif
#endif

#endif
