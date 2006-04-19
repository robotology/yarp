
#include <yarp/DgramTwoWayStream.h>

#include <yarp/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/NetType.h>

#include <ace/SOCK_Dgram_Mcast.h>

using namespace yarp;

#define CRC_SIZE 8
#define READ_SIZE (65536-CRC_SIZE)
#define WRITE_SIZE (65500-CRC_SIZE)



static bool checkCrc(char *buf, int length, int crcLength, int pct) {
  unsigned long alt = NetType::getCrc(buf+crcLength,length-crcLength);
  Bytes b(buf,4);
  Bytes b2(buf+4,4);
  unsigned long curr = (unsigned long)NetType::netInt(b);
  int altPct = NetType::netInt(b2);
  bool ok = (alt == curr && pct==altPct);
  if (!ok) {
    if (alt!=curr) {
      YARP_DEBUG(Logger::get(), "crc mismatch");
    }
    if (pct!=altPct) {
      YARP_DEBUG(Logger::get(), "packet code broken");
    }
    //YARP_ERROR(Logger::get(), String("crc read as ") + NetType::toString(curr));
    //YARP_ERROR(Logger::get(), String("crc count is ") + NetType::toString(altPct));
    //YARP_ERROR(Logger::get(), String("local count ") + NetType::toString(pct));
  }
  return ok;
}


static void addCrc(char *buf, int length, int crcLength, int pct) {
  unsigned long alt = NetType::getCrc(buf+crcLength,length-crcLength);
  Bytes b(buf,4);
  Bytes b2(buf+4,4);
  NetType::netInt((NetType::NetInt32)alt,b);
  NetType::netInt((NetType::NetInt32)pct,b2);
  //YARP_ERROR(Logger::get(), String("msg len ") + NetType::toString(length));
  //YARP_ERROR(Logger::get(), String("crc set to ") + NetType::toString(alt));
  //YARP_ERROR(Logger::get(), String("crc ct to ") + NetType::toString(pct));
}


void DgramTwoWayStream::open(const Address& remote) {
  Address local;
  ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
  local = Address(anywhere.get_host_addr(),
		  anywhere.get_port_number());
  open(local,remote);
}

void DgramTwoWayStream::open(const Address& local, const Address& remote) {
  localAddress = local;
  remoteAddress = remote;

  localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),(ACE_UINT32)INADDR_ANY);
  if (remote.isValid()) {
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());
  }
  dgram = new ACE_SOCK_Dgram;
  YARP_ASSERT(dgram!=NULL);
  int result = dgram->open(localHandle);
  if (result!=0) {
    throw IOException("could not open datagram socket");
  }
  dgram->get_local_addr(localHandle);
  YARP_DEBUG(Logger::get(),String("starting DGRAM entity on port number ") + NetType::toString(localHandle.get_port_number()));
  localAddress = Address("127.0.0.1",
			 localHandle.get_port_number());
  YARP_DEBUG(Logger::get(),String("Update: DGRAM from ") + 
	     localAddress.toString() + 
	     " to " + remote.toString());


  allocate();
}

void DgramTwoWayStream::allocate() {
  readBuffer.allocate(READ_SIZE+CRC_SIZE);
  writeBuffer.allocate(WRITE_SIZE+CRC_SIZE);
  readAt = 0;
  readAvail = 0;
  writeAvail = CRC_SIZE;
  happy = true;
  pct = 0;
}


void DgramTwoWayStream::join(const Address& group, bool sender) {

  if (sender) {
    // just use udp as normal
    open(group);
    return;
  }

  ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;

  //possible flags: ((ACE_SOCK_Dgram_Mcast::options)(ACE_SOCK_Dgram_Mcast::OPT_NULLIFACE_ALL | ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_YES));

  dgram = dmcast;
  YARP_ASSERT(dgram!=NULL);
  YARP_DEBUG(Logger::get(),String("subscribing to mcast address ") + 
	     group.toString());
  ACE_INET_Addr addr(group.getPort(),group.getName().c_str());
  int result = dmcast->join(addr,1);
  if (result!=0) {
    throw IOException("cannot connect to multi-cast address");
  }
  localAddress = group;
  remoteAddress = group;
  localHandle.set(localAddress.getPort(),localAddress.getName().c_str());
  remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());

  allocate();
}

DgramTwoWayStream::~DgramTwoWayStream() {
  close();
}

void DgramTwoWayStream::interrupt() {
  if (!closed) {
    closed = true;
    if (reader) {
      YARP_DEBUG(Logger::get(),"dgram interrupt");
      try {
	DgramTwoWayStream tmp;
	tmp.open(Address(localAddress.getName(),0),localAddress);
	ManagedBytes empty(10);
	for (int i=0; i<empty.length(); i++) {
	  empty.get()[i] = 0;
	}
	tmp.write(empty.bytes());
	tmp.flush();
	tmp.close();
      } catch (IOException e) {
	YARP_DEBUG(Logger::get(),e.toString() + " <<< closer dgram exception");
      }
    YARP_DEBUG(Logger::get(),"finished dgram interrupt");
    }
  }
  happy = false;
}

void DgramTwoWayStream::close() {
  if (dgram!=NULL) {
    interrupt();
    if (dgram!=NULL) {
      dgram->close();
      delete dgram;
      dgram = NULL;
    }
  }
  happy = false;
}

int DgramTwoWayStream::read(const Bytes& b) {
  reader = true;

  if (closed) { 
    happy = false;
    return -1; 
  }

  // if nothing is available, try to grab stuff
  if (readAvail==0) {
    readAt = 0;
    ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
    YARP_ASSERT(dgram!=NULL);
    //YARP_DEBUG(Logger::get(),"DGRAM Waiting for something!");
    int result =
      dgram->recv(readBuffer.get(),readBuffer.length(),dummy);
    YARP_DEBUG(Logger::get(),
	       String("DGRAM Got ") + NetType::toString(result) +
	       " bytes");
    if (closed||result<0) {
      happy = false;
      return result;
    }
    readAvail = result;

    // deal with CRC
    bool crcOk = checkCrc(readBuffer.get(),readAvail,CRC_SIZE,pct);
    pct++;
    if (!crcOk) {
      YARP_DEBUG(Logger::get(),"CRC failure");
      readAt = 0;
      readAvail = 0;
      throw IOException("CRC failure");
    } else {
      readAt += CRC_SIZE;
      readAvail -= CRC_SIZE;
    }
  }

  // if stuff is available, take it
  if (readAvail>0) {
    int take = readAvail;
    if (take>b.length()) {
      take = b.length();
    }
    ACE_OS::memcpy(b.get(),readBuffer.get()+readAt,take);
    readAt += take;
    readAvail -= take;
    return take;
  }

  return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
  //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
  //ACE_OS::printf("DGRAM write %d bytes\n",b.length());

  if (reader) {
    return;
  }

  Bytes local = b;
  while (local.length()>0) {
    //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
    int rem = local.length();
    int space = writeBuffer.length()-writeAvail;
    bool shouldFlush = false;
    if (rem>=space) {
      rem = space;
      shouldFlush = true;
    }
    memcpy(writeBuffer.get()+writeAvail, local.get(), rem);
    writeAvail+=rem;
    local = Bytes(local.get()+rem,local.length()-rem);
    if (shouldFlush) {
      flush();
    }
  }
}


void DgramTwoWayStream::flush() {
  // should set CRC
  if (writeAvail<=CRC_SIZE) {
    return;
  }
  addCrc(writeBuffer.get(),writeAvail,CRC_SIZE,pct);
  pct++;

  while (writeAvail>0) {
    int writeAt = 0;
    YARP_ASSERT(dgram!=NULL);
    int len = 0;

    len = dgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt,
		      remoteHandle);
    YARP_DEBUG(Logger::get(),
	       String("DGRAM wrote ") +
	       NetType::toString(len) + " bytes");

    if (len<0) {
      happy = false;
      throw IOException("DGRAM failed to write");
    }
    writeAt += len;
    writeAvail -= len;

    if (writeAvail!=0) {
      // well, we have a problem
      // checksums will cause dumping
      YARP_DEBUG(Logger::get(), "dgram/mcast send behaving badly");
    }
  }
  // finally: writeAvail should be 0

  // make space for CRC
  writeAvail = CRC_SIZE;
}


bool DgramTwoWayStream::isOk() {
  return happy;
}


void DgramTwoWayStream::reset() {
  readAt = 0;
  readAvail = 0;
  writeAvail = CRC_SIZE;
  pct = 0;
}


void DgramTwoWayStream::beginPacket() {
  //YARP_ERROR(Logger::get(),String("Packet begins: ")+(reader?"reader":"writer"));
  pct = 0;
}

void DgramTwoWayStream::endPacket() {
  //YARP_ERROR(Logger::get(),String("Packet ends: ")+(reader?"reader":"writer"));
  pct = 0;
}

