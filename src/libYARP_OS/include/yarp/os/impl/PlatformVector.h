/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMVECTOR_H
#define YARP_OS_IMPL_PLATFORMVECTOR_H

#include <yarp/conf/system.h>
#ifndef YARP_USE_STL
#  include <ace/Vector_T.h>
#  define PlatformVector ACE_Vector
#  define PLATFORM_VECTOR_ITERATOR(x) ACE_Vector_Iterator<x >
#else
#  include <vector>
#  define PlatformVector std::vector
#  define PLATFORM_VECTOR_ITERATOR(x) std::vector<x >::iterator
#endif

#endif // YARP_OS_IMPL_PLATFORMVECTOR_H
