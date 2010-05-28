// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "TcpRosCarrier.h"
#include "RosHeader.h"

#include <string>
#include <map>

#include <yarp/os/Bytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/Name.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace std;

bool TcpRosCarrier::expectSenderSpecifier(Protocol& proto) {
    proto.setRoute(proto.getRoute().addFromName("tcpros"));
    return true;
}

bool TcpRosCarrier::write(Protocol& proto, SizedWriter& writer) {

    // In the absence of access to the ROS type system, we just
    // work with blobs of uninterpreted bytes.  This could be
    // improved by hooking into types.

    // We also deal for now just with RPC (service) operation.
    // It looks like topic connections have inverted initiation
    // logic compared to YARP, which is fine but ... later.

    // This code is very messy, it has lots of commented out
    // sections from experimentation, kept around for later
    // use.

    if (firstRound) {
        // Read the header.

        printf("Route is %s\n", proto.getRoute().toString().c_str());
        Name n(proto.getRoute().getCarrierName() + "://test");
        String mode = "topic";
        String modeValue = n.getCarrierModifier("topic");
        if (modeValue=="") {
            mode = "service";
            modeValue = n.getCarrierModifier("service");
        }
        if (modeValue=="") {
            printf("need to be a service or topic, this will break\n");
        }
        
        RosHeader header;
        printf("Writing to %s\n", proto.getStreams().getRemoteAddress().toString().c_str()); 
        printf("Writing from %s\n", proto.getStreams().getLocalAddress().toString().c_str());
        //header.data["type"] = "std_msgs/String";
        header.data[mode.c_str()] = modeValue.c_str();
        header.data["md5sum"] = "*";
        header.data["callerid"] = "/not/valid/at/all/i/am/afraid/old/chum";
        string header_serial = header.writeHeader();
        string header_len(4,'\0');
        char *at = (char*)header_len.c_str();
        RosHeader::appendInt(at,header_serial.length());
        printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_len).c_str(),
               header_len.length());
    
        Bytes b1((char*)header_len.c_str(),header_len.length());
        proto.os().write(b1);
        printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_serial).c_str(),
               header_serial.length());
        Bytes b2((char*)header_serial.c_str(),header_serial.length());
        proto.os().write(b2);

        char mlen[4];
        Bytes mlen_buf(mlen,4);
        
        int res = NetType::readFull(proto.is(),mlen_buf);
        if (res<4) {
            printf("Fail %s %d\n", __FILE__, __LINE__);
            return false;
        }
        int len = NetType::netInt(mlen_buf);
        printf("Len %d\n", len);
        if (len>10000) {
            printf("not ready for serious messages\n");
            return false;
        }
        ManagedBytes m(len);
        res = NetType::readFull(proto.is(),m.bytes());
        if (res!=len) {
            printf("Fail %s %d\n", __FILE__, __LINE__);
            return false;
        }
        header.readHeader(string(m.get(),m.length()));
        firstRound = false;
    }

    printf("Writing message...\n");

    /*
    // hand crafted AddTwoInts
    int res, len;
    char mlen[4];
    Bytes mlen_buf(mlen,4);
    NetType::netInt(16,mlen_buf);
    proto.os().write(mlen_buf);
    NetType::netInt(42,mlen_buf);
    proto.os().write(mlen_buf);
    NetType::netInt(0,mlen_buf);
    proto.os().write(mlen_buf);
    NetType::netInt(10,mlen_buf);
    proto.os().write(mlen_buf);
    NetType::netInt(0,mlen_buf);
    proto.os().write(mlen_buf);
    */

    // dump out our binary blob
    writer.write(proto.os());

    /*
    while (true) {
        ManagedBytes b(1);
        res = NetType::readFull(proto.is(),b.bytes());
        if (res<=0) {
            break;
        } 
        unsigned int ch = (unsigned char)(b.get()[0]);
        printf("%x %c %d\n", ch,
               (ch>=32)?ch:'.',
               ch);
    }
    */

    // I have no idea what this is yet, but let's consume it for now.
    // It is probably frightfully important.
    char twiddle[1];
    Bytes twiddle_buf(twiddle,1);
    NetType::readFull(proto.is(),twiddle_buf);

    // ok, remaining bytes should be response.  Let client consume that.
    
    /*
    printf("read %d bytes\n",res);
    res = NetType::readFull(proto.is(),mlen_buf);
    printf("read %d bytes\n",res);
    printf("Answer %d\n", NetType::netInt(mlen_buf));
    res = NetType::readFull(proto.is(),mlen_buf);
    printf("read %d bytes\n",res);
    printf("Answer 2 %d\n", NetType::netInt(mlen_buf));
    */

    //while (true) {

    /*
    res = NetType::readFull(proto.is(),mlen_buf);
    printf("read %d bytes\n",res);
    if (res<4) {
        printf("Fail %s %d\n", __FILE__, __LINE__);
        return false;
    }
    len = NetType::netInt(mlen_buf);
    ManagedBytes payload(len);
    res = NetType::readFull(proto.is(),payload.bytes());
    if (res>0) {
        printf("Read %s -- %d bytes\n", 
               RosHeader::showMessage(string(payload.get(),payload.length())).c_str(),
               res);
    }

    printf("Socket ok? %d  read %d\n", proto.os().isOk(), res);
    //    }
    */

    return proto.getStreams().isOk();
}

bool TcpRosCarrier::reply(Protocol& proto, SizedWriter& writer) {
    // don't need to do anything special for now.
    return proto.defaultReply(writer);
}
