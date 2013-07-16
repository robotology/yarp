// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/DgramTwoWayStream.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/NetType.h>

#include <ace/SOCK_Dgram_Mcast.h>

#include <ace/SOCK_Dgram.h>
#include <ace/Handle_Set.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/ACE.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_Memory.h>
#include <ace/OS_NS_sys_select.h>
#include <ace/os_include/net/os_if.h>

#include <yarp/os/Time.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CRC_SIZE 8
#define READ_SIZE (120000-CRC_SIZE)
#define WRITE_SIZE (60000-CRC_SIZE)


static bool checkCrc(char *buf, ssize_t length, ssize_t crcLength, int pct,
                     int *store_altPct = NULL) {
    NetType::NetInt32 alt = 
        (NetType::NetInt32)NetType::getCrc(buf+crcLength,(length>crcLength)?(length-crcLength):0);
    Bytes b(buf,4);
    Bytes b2(buf+4,4);
    NetType::NetInt32 curr = NetType::netInt(b);
    int altPct = NetType::netInt(b2);
    bool ok = (alt == curr && pct==altPct);
    if (!ok) {
        if (alt!=curr) {
            YARP_DEBUG(Logger::get(), "crc mismatch");
        }
        if (pct!=altPct) {
            YARP_DEBUG(Logger::get(), "packet code broken");
        }
    }
    //YARP_ERROR(Logger::get(), String("check crc read as ") + NetType::toString(curr));
    //YARP_ERROR(Logger::get(), String("check crc count is ") + NetType::toString(altPct));
    //YARP_ERROR(Logger::get(), String("check local count ") + NetType::toString(pct));
    //YARP_ERROR(Logger::get(), String("check remote count ") + NetType::toString(altPct));
    if (store_altPct!=NULL) {
        *store_altPct = altPct;
    }

    return ok;
}


static void addCrc(char *buf, ssize_t length, ssize_t crcLength, int pct) {
    NetType::NetInt32 alt = 
        (NetType::NetInt32)NetType::getCrc(buf+crcLength,
                                           (length>crcLength)?(length-crcLength):0);
    Bytes b(buf,4);
    Bytes b2(buf+4,4);
    NetType::netInt((NetType::NetInt32)alt,b);
    NetType::netInt((NetType::NetInt32)pct,b2);
    //YARP_ERROR(Logger::get(), String("msg len ") + NetType::toString(length));
    //YARP_ERROR(Logger::get(), String("crc set to ") + NetType::toString(alt));
    //YARP_ERROR(Logger::get(), String("crc ct to ") + NetType::toString(pct));
}


bool DgramTwoWayStream::open(const Address& remote) {
    Address local;
    ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
    local = Address(anywhere.get_host_addr(),
                    anywhere.get_port_number());
    return open(local,remote);
}

bool DgramTwoWayStream::open(const Address& local, const Address& remote) {
    localAddress = local;
    remoteAddress = remote;

    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),(ACE_UINT32)INADDR_ANY);
    if (remote.isValid()) {
        remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());
    }
    dgram = new ACE_SOCK_Dgram;
    YARP_ASSERT(dgram!=NULL);

    int result = dgram->open(localHandle,
                             ACE_PROTOCOL_FAMILY_INET,
                             0,
                             1);
    if (result!=0) {
        YARP_ERROR(Logger::get(),"could not open datagram socket");
        return false;
    }

    configureSystemBuffers();

    dgram->get_local_addr(localHandle);
    YARP_DEBUG(Logger::get(),String("starting DGRAM entity on port number ") + NetType::toString(localHandle.get_port_number()));
    localAddress = Address("127.0.0.1",
                           localHandle.get_port_number());
    YARP_DEBUG(Logger::get(),String("Update: DGRAM from ") + 
               localAddress.toString() + 
               " to " + remote.toString());

    allocate();

    return true;
}

void DgramTwoWayStream::allocate(int readSize, int writeSize) {
    int _read_size = READ_SIZE+CRC_SIZE;
    int _write_size = WRITE_SIZE+CRC_SIZE;

    ConstString _env_dgram = NetworkBase::getEnvironment("YARP_DGRAM_SIZE");
    ConstString _env_mode = "";
    if (multiMode) {
        _env_mode = NetworkBase::getEnvironment("YARP_MCAST_SIZE");
    } else {
        _env_mode = NetworkBase::getEnvironment("YARP_UDP_SIZE");
    }
    if ( _env_mode!="") {
        _env_dgram = _env_mode;
    }
    if (_env_dgram!="") {
        int sz = NetType::toInt(_env_dgram);
        if (sz!=0) {
            _read_size = _write_size = sz;
        }
        YARP_INFO(Logger::get(),String("Datagram packet size set to ") +
                  NetType::toString(_read_size));
    }
    if (readSize!=0) {
        _read_size = readSize;
        YARP_INFO(Logger::get(),String("Datagram read size reset to ") +
                  NetType::toString(_read_size));
    }
    if (writeSize!=0) {
        _write_size = writeSize;
        YARP_INFO(Logger::get(),String("Datagram write size reset to ") +
                  NetType::toString(_write_size));
    }
    readBuffer.allocate(_read_size);
    writeBuffer.allocate(_write_size);
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    //happy = true;
    pct = 0;
}


void DgramTwoWayStream::configureSystemBuffers() {
    // ask for more buffer space for udp/mcast

    ConstString _dgram_buffer_size = NetworkBase::getEnvironment("YARP_DGRAM_BUFFER_SIZE");

     int window_size_desired = 600000;

    if (_dgram_buffer_size!="")
        window_size_desired = NetType::toInt(_dgram_buffer_size);

    int window_size = window_size_desired;
    int result = dgram->set_option(SOL_SOCKET, SO_RCVBUF,
                                   (char *) &window_size, sizeof(window_size));
    window_size = 0;
    int len = 4;
    int result2 = dgram->get_option(SOL_SOCKET, SO_RCVBUF,
                                    (char *) &window_size, &len);
    if (result!=0||result2!=0||window_size<window_size_desired) {
        // give a warning if we get CRC problems
        bufferAlertNeeded = true;
        bufferAlerted = false;
    }
    YARP_DEBUG(Logger::get(),
               String("Warning: buffer size set to ")+ NetType::toString(window_size) + String(", you requested ") + NetType::toString(window_size_desired));
}


int DgramTwoWayStream::restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                                     const Address& group,
                                     const Address& ipLocal,
                                     bool add) {
    restrictInterfaceIp = ipLocal;

    YARP_INFO(Logger::get(),
              String("multicast connection ") + group.getName() + " on network interface for " + ipLocal.getName());
    int result = -1;
    // There's some major damage in ACE mcast interfaces.
    // Most require interface names, yet provide no way to query
    // these names - and in the end, convert to IP addresses.
    // Here we try to do an end run around ACE.
    
    // based on: ACE_SOCK_Dgram::set_nic
    
    ip_mreq multicast_address;
    ACE_INET_Addr group_addr(group.getPort(),
                             group.getName().c_str());
    ACE_INET_Addr interface_addr(ipLocal.getPort(),
                                 ipLocal.getName().c_str());
    multicast_address.imr_interface.s_addr =
        htonl (interface_addr.get_ip_address ());
    multicast_address.imr_multiaddr.s_addr =
        htonl (group_addr.get_ip_address ());
    
    if (add) {
        YARP_DEBUG(Logger::get(),"Trying to correct mcast membership...\n");
        result = 
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_ADD_MEMBERSHIP,
                                             &multicast_address,
                                             sizeof (struct ip_mreq));
    } else {
        YARP_DEBUG(Logger::get(),"Trying to correct mcast output...");
        result = 
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_MULTICAST_IF,
                                             &multicast_address.imr_interface.s_addr,
                                             sizeof (struct in_addr));

    }
    if (result!=0) {
        int num = errno;
        YARP_DEBUG(Logger::get(),
                   String("mcast result: ") +
                   strerror(num));
        if (num==98) {
            // our membership is already correct / Address already in use
            result = 0;
        }
        result = 0; // in fact, best to proceed for Windows.
    }
    
    return result;
}


bool DgramTwoWayStream::openMcast(const Address& group, 
                                  const Address& ipLocal) {

    multiMode = true;

    localAddress = ipLocal;
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),
                                (ACE_UINT32)INADDR_ANY);

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;
    dgram = dmcast;
    mgram = dmcast;
    YARP_ASSERT(dgram!=NULL);
    
    int result = -1;
    ACE_INET_Addr addr(group.getPort(),group.getName().c_str());
    if (ipLocal.isValid()) {
        //printf("  sender: determine multicast interface from ip %s\n",
        //     ipLocal.getName().c_str());
        //result = dmcast->open(addr,localHandle,1); 
        result = dmcast->open(addr,NULL,1); 
        if (result==0) {
            //result = restrictMcast(dmcast,group,ipLocal,true);
            result = restrictMcast(dmcast,group,ipLocal,false);
        }
    } else {
        //printf("  generic multicast interface\n");
        result = dmcast->open(addr,NULL,1); 
    }

    if (result!=0) {
        YARP_ERROR(Logger::get(),"could not open multicast datagram socket");
        return false;
    }

    configureSystemBuffers();

    remoteAddress = group;
    localHandle.set(localAddress.getPort(),localAddress.getName().c_str());
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());

    allocate();

    return true;
}


bool DgramTwoWayStream::join(const Address& group, bool sender,
                             const Address& ipLocal) {
    YARP_DEBUG(Logger::get(),String("subscribing to mcast address ") + 
               group.toString() + " for " +
               (sender?"writing":"reading"));

    multiMode = true;

    if (sender) {
        if (ipLocal.isValid()) {
            return openMcast(group,ipLocal);
        } else {
            // just use udp as normal
            return open(group);
        }
        //return;
    }

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;

    //possible flags: ((ACE_SOCK_Dgram_Mcast::options)(ACE_SOCK_Dgram_Mcast::OPT_NULLIFACE_ALL | ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_YES));

    dgram = dmcast;
    mgram = dmcast;
    YARP_ASSERT(dgram!=NULL);

    ACE_INET_Addr addr(group.getPort(),group.getName().c_str());

    int result = -1;
    if (ipLocal.isValid()) {
        //printf("  receiver: determine multicast interface from ip %s\n",
        //     ipLocal.getName().c_str());

        result = 0;
        result = dmcast->join(addr,1); 

        if (result==0) {
            //if (sender) { // try only doing this for sender
            result = restrictMcast(dmcast,group,ipLocal,true);
            //printf("Result is %d\n", result);
            //}
        }
    } else {
        result = dmcast->join(addr,1); 
    }

    configureSystemBuffers();

    if (result!=0) {
        YARP_ERROR(Logger::get(),"cannot connect to multi-cast address");
        happy = false;
        return false;
    }
    localAddress = group;
    remoteAddress = group;
    localHandle.set(localAddress.getPort(),localAddress.getName().c_str());
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());

    allocate();
    return true;
}

DgramTwoWayStream::~DgramTwoWayStream() {
    closeMain();
}

void DgramTwoWayStream::interrupt() {
    bool act = false;
    mutex.wait();
    if ((!closed) && (!interrupting) && happy) {
        act = true;
        interrupting = true;
        closed = true;
    }
    mutex.post();

    if (act) {
        if (reader) {
            int ct = 3;
            while (happy && ct>0) {
                ct--;
                DgramTwoWayStream tmp;
                if (mgram) {
                    YARP_DEBUG(Logger::get(),
                               String("* mcast interrupt, interface ") +
                               restrictInterfaceIp.toString().c_str()
                               );
                    tmp.join(localAddress,true,
                             restrictInterfaceIp);
                } else {
                    YARP_DEBUG(Logger::get(),"* dgram interrupt");
                    tmp.open(Address(localAddress.getName(),0),
                             localAddress);
                }
                YARP_DEBUG(Logger::get(),
                           String("* interrupt state ") +
                           NetType::toString(interrupting) + " " +
                           NetType::toString(closed) + " " +
                           NetType::toString(happy) + " ");
                ManagedBytes empty(10);
                for (size_t i=0; i<empty.length(); i++) {
                    empty.get()[i] = 0;
                }
                
                // don't want this message getting into a valid packet
                tmp.pct = -1;
                
                tmp.write(empty.bytes());
                tmp.flush();
                tmp.close();
                if (happy) {
                    yarp::os::Time::delay(0.25);
                }
            }
            YARP_DEBUG(Logger::get(),"dgram interrupt done");
        } 
        mutex.wait();
        interrupting = false;
        mutex.post();
    } else {
        // wait for interruption to be done
        if (interrupting) {
            while (interrupting) {
                YARP_DEBUG(Logger::get(),
                           "waiting for dgram interrupt to be finished...");
                yarp::os::Time::delay(0.1);
            }
        }
    }

}

void DgramTwoWayStream::closeMain() {
    if (dgram!=NULL) {
        //printf("Dgram closing, interrupt state %d\n", interrupting);
        interrupt();
        mutex.wait();
        closed = true;
        happy = false;
        //printf("Dgram closinger, interrupt state %d\n", interrupting);
        mutex.post();
        while (interrupting) {
            happy = false;
            yarp::os::Time::delay(0.1);
        }
        mutex.wait();
        if (dgram!=NULL) {
            dgram->close();
            delete dgram;
            dgram = NULL;
            mgram = NULL;
        }
        happy = false;
        mutex.post();
    } 
    happy = false;
}

ssize_t DgramTwoWayStream::read(const Bytes& b) {
    reader = true;
    bool done = false;
    
    while (!done) {

        if (closed) { 
            happy = false;
            return -1; 
        }
        
        // if nothing is available, try to grab stuff
        if (readAvail==0) {
            readAt = 0;


            //YARP_ASSERT(dgram!=NULL);
            //YARP_DEBUG(Logger::get(),"DGRAM Waiting for something!");
            ssize_t result = -1;
            if (mgram && restrictInterfaceIp.isValid()) { 
                /*
                printf("Consider remote mcast\n");
                printf("What we know:\n");
                printf("  %s\n",restrictInterfaceIp.toString().c_str());
                printf("  %s\n",localAddress.toString().c_str());
                printf("  %s\n",remoteAddress.toString().c_str());
                */
                ACE_INET_Addr iface(restrictInterfaceIp.getPort(),
                                    restrictInterfaceIp.getName().c_str());
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                result =
                    dgram->recv(readBuffer.get(),readBuffer.length(),dummy);
                YARP_DEBUG(Logger::get(),
                           String("MCAST Got ") + NetType::toString((int)result) +
                           " bytes");
                
            } else if (dgram!=NULL) {
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                YARP_ASSERT(dgram!=NULL);
                //YARP_DEBUG(Logger::get(),"DGRAM Waiting for something!");
                result =
                    dgram->recv(readBuffer.get(),readBuffer.length(),dummy);
                YARP_DEBUG(Logger::get(),
                           String("DGRAM Got ") + NetType::toString((int)result) +
                           " bytes");
            } else {
                onMonitorInput();
                //printf("Monitored input of %d bytes\n", monitor.length());
                if (monitor.length()>readBuffer.length()) {
                    printf("Too big!\n");
                    exit(1);
                }
                memcpy(readBuffer.get(),monitor.get(),monitor.length());
                result = monitor.length();
            }


            /*
              // this message isn't needed anymore
            if (result>WRITE_SIZE*1.25) {
                YARP_ERROR(Logger::get(),
                           String("Got big datagram: ")+NetType::toString(result)+
                           " bytes");
            }
            */
            if (closed||(result<0)) {
                happy = false;
                return result;
            }
            readAvail = result;
            
            // deal with CRC
            int altPct = 0;
            bool crcOk = checkCrc(readBuffer.get(),readAvail,CRC_SIZE,pct,
                                  &altPct);
            if (altPct!=-1) {
                pct++;
                if (!crcOk) {
                    if (bufferAlertNeeded&&!bufferAlerted) {
                        YARP_ERROR(Logger::get(),
                                   "*** Multicast/UDP packet dropped - checksum error ***");
                        YARP_INFO(Logger::get(),
                                  "The UDP/MCAST system buffer limit on your system is low.");
                        YARP_INFO(Logger::get(),
                                  "You may get packet loss under heavy conditions.");
#ifdef __linux__
                        YARP_INFO(Logger::get(),
                                  "To change the buffer limit on linux: sysctl -w net.core.rmem_max=8388608");
                        YARP_INFO(Logger::get(),
                                  "(Might be something like: sudo /sbin/sysctl -w net.core.rmem_max=8388608)");
#else
                        YARP_INFO(Logger::get(),
                                  "To change the limit use: systcl for Linux/FreeBSD, ndd for Solaris, no for AIX");
#endif
                        bufferAlerted = true;
                    } else {
                        errCount++;
                        double now = Time::now();
                        if (now-lastReportTime>1) {
                            YARP_ERROR(Logger::get(),
                                       String("*** ") + NetType::toString(errCount) + " datagram packet(s) dropped - checksum error ***");
                            lastReportTime = now;
                            errCount = 0;
                        }
                    }
                    reset();
                    return -1;
                } else {
                    readAt += CRC_SIZE;
                    readAvail -= CRC_SIZE;
                }
                done = true;
            } else {
                readAvail = 0;
            }
        }

        // if stuff is available, take it
        if (readAvail>0) {
            size_t take = readAvail;
            if (take>b.length()) {
                take = b.length();
            }
            ACE_OS::memcpy(b.get(),readBuffer.get()+readAt,take);
            readAt += take;
            readAvail -= take;
            return take;
        }
    }

    return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
    //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
    //ACE_OS::printf("DGRAM write %d bytes\n",b.length());

    if (reader) {
        return;
    }
    if (writeBuffer.get()==NULL) {
        return;
    }

    Bytes local = b;
    while (local.length()>0) {
        //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
        ssize_t rem = local.length();
        ssize_t space = writeBuffer.length()-writeAvail;
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
    if (writeBuffer.get()==NULL) {
        return;
    }

    // should set CRC
    if (writeAvail<=CRC_SIZE) {
        return;
    }
    addCrc(writeBuffer.get(),writeAvail,CRC_SIZE,pct);
    pct++;

    while (writeAvail>0) {
        ssize_t writeAt = 0;
        //YARP_ASSERT(dgram!=NULL);
        ssize_t len = 0;

        if (mgram!=NULL) {
            len = mgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt);
            YARP_DEBUG(Logger::get(),
                       String("MCAST - wrote ") +
                       NetType::toString((int)len) + " bytes"
                       );
        } else if (dgram!=NULL) {
            len = dgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt,
                              remoteHandle);
            YARP_DEBUG(Logger::get(),
                       String("DGRAM - wrote ") +
                       NetType::toString((int)len) + " bytes to " +
                       remoteAddress.toString()
                       );
        } else {
            Bytes b(writeBuffer.get()+writeAt,writeAvail-writeAt);
            monitor = ManagedBytes(b,false);
            monitor.copy();
            //printf("Monitored output of %d bytes\n", monitor.length());
            len = monitor.length();
            onMonitorOutput();
        }
        //if (len>WRITE_SIZE*0.75) {
        if (len>writeBuffer.length()*0.75) {
            YARP_DEBUG(Logger::get(),
                       "long dgrams might need a little time");

            // Under heavy loads, packets could get dropped
            // 640x480x3 images correspond to about 15 datagrams
            // so there's not much time possible between them
            // looked at iperf, it just does a busy-waiting delay
            // there's an implementation below, but commented out -
            // better solution was to increase recv buffer size

            double first = yarp::os::Time::now();
            double now = first;
            int ct = 0;
            do {
                //printf("Busy wait... %d\n", ct);
                yarp::os::Time::delay(0);
                now = yarp::os::Time::now();
                ct++;
            } while (now-first<0.001);
        }

        if (len<0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"DGRAM failed to write");
            return;
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
    if (!reader) {
        pct = 0;
    }
}

Bytes DgramTwoWayStream::getMonitor() {
    return monitor.bytes();
}


void DgramTwoWayStream::removeMonitor() {
    monitor.clear();
}


#else

int DgramTwoWayStreamDummySymbol = 42;

#endif // YARP_HAS_ACE

