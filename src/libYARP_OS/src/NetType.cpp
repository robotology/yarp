#include <yarp/NetType.h>
#include <yarp/ManagedBytes.h>

using namespace yarp;

// slow implementation - only relevant for textmode operation

String NetType::readLine(InputStream& is, int terminal) {
  String buf("");
  bool done = false;
  while (!done) {
    //ACE_OS::printf("preget\n");
    int v = is.read();
    //ACE_OS::printf("got [%d]\n",v);
    char ch = (char)v;
    if (v>=32) {
      buf += ch;
    }
    if (ch==terminal) {
      done = true;
    }
    if (ch<0) { 
      throw IOException("readLine failed");
    }
  }
  return buf;
}    

int NetType::readFull(InputStream& is, const Bytes& b) {
  int off = 0;
  int fullLen = b.length();
  int remLen = fullLen;
  int result = 1;
  while (result>0&&remLen>0) {
    result = is.read(b,off,remLen);
    //printf("read result is %d\n",result);
    if (result>0) {
      remLen -= result;
      off += result;
    }
  }
  return (result<=0)?-1:fullLen;
}

int NetType::readDiscard(InputStream& is, int len) {
  if (len<100) {
    char buf[100];
    Bytes b(buf,len);
    return readFull(is,b);
  } else {
    ManagedBytes b(len);
    return readFull(is,b.bytes());
  }
}


String NetType::toString(int x) {
  char buf[256];
  ACE_OS::sprintf(buf,"%d",x);
  return buf;
}


int NetType::toInt(String x) {
  return ACE_OS::atoi(x.c_str());
}

