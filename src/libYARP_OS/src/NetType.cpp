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


/*
  PNG's nice and simple CRC code 
  (from http://www.w3.org/TR/PNG-CRCAppendix.html)
 */

/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
static int crc_table_computed = 0;
   
/* Make the table for a fast CRC. */
static void make_crc_table(void) {
  unsigned long c;
     int n, k;
   
     for (n = 0; n < 256; n++) {
       c = (unsigned long) n;
       for (k = 0; k < 8; k++) {
         if (c & 1)
           c = 0xedb88320L ^ (c >> 1);
         else
           c = c >> 1;
       }
       crc_table[n] = c;
     }
     crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */

static unsigned long update_crc(unsigned long crc, unsigned char *buf,
			 int len) {
  unsigned long c = crc;
  int n;
  
  if (!crc_table_computed)
    make_crc_table();
  for (n = 0; n < len; n++) {
    c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  }
  return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long NetType::getCrc(char *buf, int len) {
  return update_crc(0xffffffffL, (unsigned char *)buf, len) ^ 0xffffffffL;
}
