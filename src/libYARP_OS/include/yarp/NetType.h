#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/InputStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>

#include <ace/OS_NS_stdlib.h>

namespace yarp {
  class NetType;
}

/**
 * Various utilities related to types and formats.
 */
class yarp::NetType {
public:

  static int netInt(const Bytes& code) {
    YARP_ASSERT(code.length()==sizeof(NetType::NetInt32));
    NetType::NetInt32& i = *((NetType::NetInt32*)(code.get()));
    return i;
  }

  static void netInt(int data, const Bytes& code) {
    NetType::NetInt32 i = data;
    Bytes b((char*)(&i),sizeof(i));
    if (code.length()!=sizeof(i)) {
      throw IOException("not enough room for integer");
    }
    ACE_OS::memcpy(code.get(),b.get(),code.length());
  }

  static String readLine(InputStream& is, int terminal = '\n');

  static int readFull(InputStream& is, const Bytes& b);

  static int readDiscard(InputStream& is, int len);

  static String toString(int x);

  static int NetType::toInt(String x);

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
  typedef int32_t NetInt32;
#else
#  ifdef YARP2_WINDOWS
     typedef __int32 NetInt32;
#  else
#    error "need to define NetInt32 for this OS; see yarp/NetType.h"
#  endif
#endif

};

#endif
