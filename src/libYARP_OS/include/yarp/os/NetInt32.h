#ifndef _YARP2_NETINT32_
#define _YARP2_NETINT32_

namespace yarp {
  namespace os {

/**
 * Definition of the NetInt32 type
 */
#ifdef WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef _WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOW2
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WIN__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WINDOWS__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOWS
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __LINUX__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif

#ifdef __linux__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif



#ifdef YARP2_LINUX
    typedef int NetInt32;
    //typedef int32_t NetInt32;
#else
#  ifdef YARP2_WINDOWS
    typedef __int32 NetInt32;
#  else
    // now we have to assume we are using autoconf etc
#undef  PACKAGE_BUGREPORT
#undef  PACKAGE_NAME
#undef  PACKAGE_STRING
#undef  PACKAGE_TARNAME
#undef  PACKAGE_VERSION
#    include <yarp/conf/yarp-config.h>
#    if YARP_HAS_INT32_T && !(WORDS_BIGENDIAN)
#include <stdint.h>
       typedef int32_t NetInt32;
#    else
#      error "need to define NetInt32 for this OS; see yarp/os/NetInt32.h"
#    endif
#  endif
#endif

  }
}

#endif /* _YARP2_NETINT32_ */



