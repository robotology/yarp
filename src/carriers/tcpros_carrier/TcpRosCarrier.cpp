/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "TcpRosCarrier.h"
#include "RosSlave.h"
#include "WireImage.h"

#include <string>
#include <map>

#include <yarp/os/Bytes.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Name.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

#define dbg_printf if (0) printf

void TcpRosCarrier::setParameters(const Bytes& header) {
    if (header.length()!=8) {
        return;
    }
    Bytes h1(header.get(),4);
    Bytes h2(header.get()+4,4);
    headerLen1 = NetType::netInt(h1);
    headerLen2 = NetType::netInt(h2);
}

bool TcpRosCarrier::checkHeader(const Bytes& header) {
    if (header.length()!=8) {
        return false;
    }
    setParameters(header);
    if (!(headerLen1<60000&&headerLen1>0 &&
          headerLen2<60000&&headerLen2>0)) {
        // not tcpros
        return false;
    }
    // plausibly tcpros.
    dbg_printf("tcpros! %d %d\n", headerLen1,headerLen2);
    return true;
}


ConstString TcpRosCarrier::getRosType(ConnectionState& proto) {
    ConstString typ = "";
    ConstString rtyp = "";
    if (proto.getContactable()) {
        Type t = proto.getContactable()->getType(); 
        typ = t.getName();
        md5sum = t.readProperties().find("md5sum").asString();
        message_definition = t.readProperties().find("message_definition").asString();
        user_type = typ;
        if (typ=="yarp/image") {
            wire_type = "sensor_msgs/Image";
            rtyp = "";
        } else if (typ=="yarp/bottle") {
            rtyp = proto.getContactable()->getType().getNameOnWire();
            if (rtyp=="yarp/image") rtyp = "sensor_msgs/Image";
            wire_type = rtyp;
        } else if (typ!="") {
            rtyp = typ;
            wire_type = typ;
        }
    }
    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString mode = "topic";
    ConstString modeValue = n.getCarrierModifier("topic");
    if (modeValue=="") {
        mode = "service";
        modeValue = n.getCarrierModifier("service");
    }
    if (modeValue!="") {
        ConstString package = n.getCarrierModifier("package");
        if (package!="") {
            rtyp = package + "/" + modeValue;
        }
    }

    dbg_printf("USER TYPE %s\n", user_type.c_str());
    dbg_printf("WIRE TYPE %s\n", wire_type.c_str());

    return rtyp;
}

bool TcpRosCarrier::sendHeader(ConnectionState& proto) {
    dbg_printf("Route is %s\n", proto.getRoute().toString().c_str());
    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString mode = "topic";
    ConstString modeValue = n.getCarrierModifier("topic");
    if (modeValue=="") {
        mode = "service";
        modeValue = n.getCarrierModifier("service");
        isService = true;
    }
    if (modeValue=="") {
        printf("*** no topic or service specified!\n");
        mode = "topic";
        modeValue = "notopic";
        isService = false;
    }
    ConstString rawValue = n.getCarrierModifier("raw");
    if (rawValue=="2") {
        raw = 2;
        dbg_printf("ROS-native mode requested\n");
    } else if (rawValue=="1") {
        raw = 1;
        dbg_printf("Raw mode requested\n");
    } else if (rawValue=="0") {
        raw = 0;
        dbg_printf("Cooked mode requested\n");
    }

    RosHeader header;
    dbg_printf("Writing to %s\n", proto.getStreams().getRemoteAddress().toString().c_str()); 
    dbg_printf("Writing from %s\n", proto.getStreams().getLocalAddress().toString().c_str());

    ConstString rtyp = getRosType(proto);
    if (rtyp!="") {
        header.data["type"] = rtyp.c_str();
    }
    header.data[mode.c_str()] = modeValue.c_str();
    header.data["md5sum"] = (md5sum!="")?md5sum:"*";
    if (message_definition!="") {
        header.data["message_definition"] = message_definition;
    }
    NestedContact nc(proto.getRoute().getFromName());
    header.data["callerid"] = nc.getNodeName();
    header.data["persistent"] = "1";
    string header_serial = header.writeHeader();
    string header_len(4,'\0');
    char *at = (char*)header_len.c_str();
    RosHeader::appendInt(at,header_serial.length());
    dbg_printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_len).c_str(),
               (int)header_len.length());

    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    dbg_printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_serial).c_str(),
               (int)header_serial.length());
    Bytes b2((char*)header_serial.c_str(),header_serial.length());
    proto.os().write(b2);

    return proto.os().isOk();
}


bool TcpRosCarrier::expectReplyToHeader(ConnectionState& proto) {
    RosHeader header;

    char mlen[4];
    Bytes mlen_buf(mlen,4);

    int res = proto.is().readFull(mlen_buf);
    if (res<4) {
        printf("Fail %s %d\n", __FILE__, __LINE__);
        return false;
    }
    int len = NetType::netInt(mlen_buf);
    dbg_printf("Len %d\n", len);
    if (len>10000) {
        printf("not ready for serious messages\n");
        return false;
    }
    ManagedBytes m(len);
    res = proto.is().readFull(m.bytes());
    if (res!=len) {
        printf("Fail %s %d\n", __FILE__, __LINE__);
        return false;
    }
    header.readHeader(string(m.get(),m.length()));
    dbg_printf("Message header: %s\n", header.toString().c_str());
    ConstString rosname = "";
    if (header.data.find("type")!=header.data.end()) {
        rosname = header.data["type"].c_str();
    }
    dbg_printf("<incoming> Type of data is [%s]s\n", rosname.c_str());
    if (header.data.find("callerid")!=header.data.end()) {
        string name = header.data["callerid"];
        dbg_printf("<incoming> callerid is %s\n", name.c_str());
        dbg_printf("Route was %s\n", proto.getRoute().toString().c_str());
        Route route = proto.getRoute();
        route.setToName(name.c_str());
        proto.setRoute(route);
        dbg_printf("Route is now %s\n", proto.getRoute().toString().c_str());
    }

    if (!isService) {
        isService = (header.data.find("request_type")!=header.data.end());
    }
    if (rosname!="" && (user_type != wire_type || user_type == "")) {
        kind = TcpRosStream::rosToKind(rosname.c_str()).c_str();
        TcpRosStream::configureTwiddler(twiddler,kind.c_str(),rosname.c_str(),false,false);
        translate = TCPROS_TRANSLATE_TWIDDLER;
    } else {
        rosname = "";
    }
    dbg_printf("tcpros %s mode\n", isService?"service":"topic");

    // we may be a pull stream
    sender = isService;

    processRosHeader(header);

    TcpRosStream *stream = new TcpRosStream(proto.giveStreams(),sender,
                                            sender,
                                            isService,raw,rosname.c_str());

    if (stream==NULL) { return false; }

    dbg_printf("Getting ready to hand off streams...\n");

    proto.takeStreams(stream);

    return proto.is().isOk();
}

bool TcpRosCarrier::expectSenderSpecifier(ConnectionState& proto) {
    Route route = proto.getRoute();
    route.setFromName("tcpros");
    proto.setRoute(route);
    dbg_printf("Trying for tcpros header\n");
    ManagedBytes m(headerLen1);
    Bytes mrem(m.get()+4,m.length()-4);
    NetInt32 ni = headerLen2;
    memcpy(m.get(),(char*)(&ni), 4);
    dbg_printf("reading %d bytes\n", (int)mrem.length());
    int res = proto.is().readFull(mrem);
    dbg_printf("read %d bytes\n", res);
    if (res!=(int)mrem.length()) {
        if (res>=0) {
            fprintf(stderr,"TCPROS header failure, expected %d bytes, got %d bytes\n",
                    (int)mrem.length(),res);
        } else {
            fprintf(stderr,"TCPROS connection has gone terribly wrong\n");
        }
        return false;
    }
    RosHeader header;
    header.readHeader(string(m.get(),m.length()));
    dbg_printf("Got header %s\n", header.toString().c_str());

    ConstString rosname = "";
    if (header.data.find("type")!=header.data.end()) {
        rosname = header.data["type"].c_str();
    }
    ConstString rtyp = getRosType(proto);
    if (rtyp!="") {
        rosname = rtyp;
        header.data["type"] = rosname;
        header.data["md5sum"] = (md5sum!="")?md5sum:"*";
        if (message_definition!="") {
            header.data["message_definition"] = message_definition;
        }
    }
    dbg_printf("<outgoing> Type of data is %s\n", rosname.c_str());

    route = proto.getRoute();
    if (header.data.find("callerid")!=header.data.end()) {
        route.setFromName(header.data["callerid"].c_str());
    } else {
        route.setFromName("tcpros");
    }
    proto.setRoute(route);

    // Let's just ignore everything that is sane and holy, and
    // send the same header right back.
    // **UPDATE** Oh, ok, let's modify the callerid.  Begrudgingly.
    NestedContact nc(proto.getRoute().getToName());
    header.data["callerid"] = nc.getNodeName().c_str();

    string header_serial = header.writeHeader();
    string header_len(4,'\0');
    char *at = (char*)header_len.c_str();
    RosHeader::appendInt(at,header_serial.length());
    dbg_printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_len).c_str(),
               (int)header_len.length());
    
    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    dbg_printf("Writing %s -- %d bytes\n", 
               RosHeader::showMessage(header_serial).c_str(),
               (int)header_serial.length());
    Bytes b2((char*)header_serial.c_str(),header_serial.length());
    proto.os().write(b2);

    if (header.data.find("probe")!=header.data.end()) {
        dbg_printf("================PROBE===============\n");
        return false;
    }


    if (!isService) {
        isService = (header.data.find("service")!=header.data.end());
    }
    if (rosname!="" && (user_type != wire_type || user_type == "")) {
        if (wire_type!="sensor_msgs/Image") { // currently using a custom method for images
            kind = TcpRosStream::rosToKind(rosname.c_str()).c_str();
            TcpRosStream::configureTwiddler(twiddler,kind.c_str(),rosname.c_str(),true,true);
            translate = TCPROS_TRANSLATE_TWIDDLER;
        }
    } else {
        rosname = "";
    }
    sender = isService; 

    processRosHeader(header);

    if (isService) {
        TcpRosStream *stream = new TcpRosStream(proto.giveStreams(),sender,
                                                false,
                                                isService,raw,rosname.c_str());
        
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return proto.is().isOk();
    }

    return true;
}

bool TcpRosCarrier::write(ConnectionState& proto, SizedWriter& writer) {
    SizedWriter *flex_writer = &writer;

    if (raw!=2) {
        // At startup, we check for what kind of messages are going
        // through, and prepare an appropriate byte-rejiggering if
        // needed.
        if (translate==TCPROS_TRANSLATE_UNKNOWN) {
            dbg_printf("* TCPROS_TRANSLATE_UNKNOWN\n");
            FlexImage *img = NULL;
            if (user_type=="yarp/image"||user_type=="yarp/bottle") {
                img = wi.checkForImage(writer);
            }
            if (img) {
                translate = TCPROS_TRANSLATE_IMAGE;
                ConstString frame = "/frame";
                ri.init(*img,frame);
            } else { 
                if (WireBottle::extractBlobFromBottle(writer,wt)) {
                    translate = TCPROS_TRANSLATE_BOTTLE_BLOB;
                } else {
                    translate = TCPROS_TRANSLATE_INHIBIT;
                }
            }
        }
    } else {
        translate = TCPROS_TRANSLATE_INHIBIT;
    }

    // Apply byte-rejiggering if needed.
    switch (translate) {
    case TCPROS_TRANSLATE_IMAGE:
        {
            dbg_printf("* TCPROS_TRANSLATE_IMAGE\n");
            FlexImage *img = wi.checkForImage(writer);
            if (img==NULL) {
                fprintf(stderr, "TCPROS Expected an image, but did not get one.\n");
                return false;
            }
            ri.update(img,seq,Time::now());  // Time here is the timestamp of the ROS message, so Time::now(), the mutable one is correct.
            seq++;
            flex_writer = &ri;
        }
        break;
    case TCPROS_TRANSLATE_BOTTLE_BLOB:
        {
            dbg_printf("* TCPROS_TRANSLATE_BOTTLE_BLOB\n");
            if (!WireBottle::extractBlobFromBottle(writer,wt)) {
                fprintf(stderr, "TCPROS Expected a bottle blob, but did not get one.\n");
                return false;
            }
            flex_writer = &wt;
        }
        break;
    case TCPROS_TRANSLATE_TWIDDLER:
        {
            dbg_printf("* TCPROS_TRANSLATE_TWIDDLER\n");
            twiddler_output.attach(writer,twiddler);
            if (twiddler_output.update()) {
                flex_writer = &twiddler_output;
            } else {
                flex_writer = NULL;
            }
        }
        break;
    case TCPROS_TRANSLATE_INHIBIT:
        dbg_printf("* TCPROS_TRANSLATE_INHIBIT\n");
        break;
    default:
        dbg_printf("* TCPROS_TRANSLATE_OTHER\n");
        break;
    }

    if (flex_writer == NULL) {
        return false;
    }

    int len = 0;
    for (size_t i=0; i<flex_writer->length(); i++) {
        len += (int)flex_writer->length(i);
    }
    dbg_printf("Prepping to write %d blocks (%d bytes)\n", 
               (int)flex_writer->length(),
               len);

    string header_len(4,'\0');
    char *at = (char*)header_len.c_str();
    RosHeader::appendInt(at,len);
    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    flex_writer->write(proto.os());

    dbg_printf("done sending\n");
    
    if (isService) {
        if (!sender) {
            if (!persistent) {
                proto.os().close();
            }
        }
    }

    return proto.getStreams().isOk();
}

bool TcpRosCarrier::reply(ConnectionState& proto, SizedWriter& writer) {
    char twiddle[1];
    twiddle[0] = 1;
    Bytes twiddle_buf(twiddle,1);
    proto.os().write(twiddle_buf);
    return write(proto,writer);
}


int TcpRosCarrier::connect(const yarp::os::Contact& src,
                           const yarp::os::Contact& dest,
                           const yarp::os::ContactStyle& style,
                           int mode,
                           bool reversed) {
    switch (mode) {
    case YARP_ENACT_DISCONNECT:
        printf("tcpros disconnect not implemented yet in this direction \n");
        return -1;
        break;
    case YARP_ENACT_EXISTS:
        printf("tcpros connection check not implemented yet in this direction \n");
        return -1;
        break;
    }

    if (!reversed) return -1;

    Contact fullDest = dest;
    if (fullDest.getPort()<=0) {
        fullDest = NetworkBase::queryName(fullDest.getName().c_str());
    }

    Contact fullSrc = src;
    if (fullSrc.getPort()<=0) {
        fullSrc = NetworkBase::queryName(fullSrc.getName().c_str());
    }

    Name n((style.carrier + "://test").c_str());
    ConstString topic = n.getCarrierModifier("topic").c_str();
    if (topic=="") {
        printf("Warning, no topic!\n");
        topic = "notopic";
    }

    RosSlave slave(false);
    dbg_printf("Starting temporary slave\n");
    slave.start(fullDest.getHost().c_str(),fullDest.getPort());
    Contact addr_slave = slave.where();
    Bottle cmd, reply;
    cmd.addString("publisherUpdate");
    cmd.addString("dummy_id");
    cmd.addString(topic);
    Bottle& lst = cmd.addList();
    char buf[1000];
    sprintf(buf,"http://%s:%d/", addr_slave.getHost().c_str(), 
            addr_slave.getPort());
    lst.addString(buf);
    ContactStyle req;
    req.carrier = "xmlrpc";
    req.timeout = 4;
    req.quiet = false;
    bool ok = NetworkBase::write(fullSrc,cmd,reply,req);
    dbg_printf("%s\n",reply.toString().c_str());
    if (!ok) {
        fprintf(stderr, "error talking to %s\n", fullSrc.toString().c_str());
    }
    slave.stop();
    if (!slave.isOk()) {
        fprintf(stderr, "Problem: did not get a callback from ROS - can happen if connection already exists.\n");
        ok = false;
    }

    return ok?0:1;
}


void TcpRosCarrier::processRosHeader(RosHeader& header) {
    if (header.data.find("persistent")!=header.data.end()) {
        persistent = (header.data["persistent"]=="1");
    } else {
        persistent = false;
    }
}
