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
    typedef int NetInt32;
    //typedef int32_t NetInt32;
#else
#  ifdef YARP2_WINDOWS
    typedef __int32 NetInt32;
#  else
#    error "need to define NetInt32 for this OS; see yarp/NetType.h"
#  endif
#endif

  }
}

#endif /* _YARP2_NETINT32_ */



