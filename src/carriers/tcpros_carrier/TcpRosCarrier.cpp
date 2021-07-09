/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TcpRosCarrier.h"
#include "RosSlave.h"
#include "TcpRosLogComponent.h"

#include <string>
#include <map>

#include <yarp/os/Bytes.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Name.h>
#include <yarp/os/Route.h>
#include <yarp/wire_rep_utils/WireImage.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::wire_rep_utils;
using namespace std;

void TcpRosCarrier::setParameters(const Bytes& header) {
    if (header.length()!=8) {
        return;
    }
    headerLen1 = *reinterpret_cast<const NetInt32*>(header.get());
    headerLen2 = *reinterpret_cast<const NetInt32*>(header.get() + 4);
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
    yCTrace(TCPROSCARRIER, "tcpros! %d %d", headerLen1,headerLen2);
    return true;
}


std::string TcpRosCarrier::getRosType(ConnectionState& proto) {
    std::string typ;
    std::string rtyp;
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
    std::string mode = "topic";
    std::string modeValue = n.getCarrierModifier("topic");
    if (modeValue=="") {
        mode = "service";
        modeValue = n.getCarrierModifier("service");
    }
    if (modeValue!="") {
        std::string package = n.getCarrierModifier("package");
        if (package!="") {
            rtyp = package + "/" + modeValue;
        }
    }

    yCTrace(TCPROSCARRIER, "USER TYPE %s", user_type.c_str());
    yCTrace(TCPROSCARRIER, "WIRE TYPE %s", wire_type.c_str());

    return rtyp;
}

bool TcpRosCarrier::sendHeader(ConnectionState& proto) {
    yCTrace(TCPROSCARRIER, "Route is %s", proto.getRoute().toString().c_str());
    Name n(proto.getRoute().getCarrierName() + "://test");
    std::string mode = "topic";
    std::string modeValue = n.getCarrierModifier("topic");
    if (modeValue=="") {
        mode = "service";
        modeValue = n.getCarrierModifier("service");
        isService = true;
    }
    if (modeValue=="") {
        yCInfo(TCPROSCARRIER, "*** no topic or service specified!");
        mode = "topic";
        modeValue = "notopic";
        isService = false;
    }
    std::string rawValue = n.getCarrierModifier("raw");
    if (rawValue=="2") {
        raw = 2;
        yCTrace(TCPROSCARRIER, "ROS-native mode requested");
    } else if (rawValue=="1") {
        raw = 1;
        yCTrace(TCPROSCARRIER, "Raw mode requested");
    } else if (rawValue=="0") {
        raw = 0;
        yCTrace(TCPROSCARRIER, "Cooked mode requested");
    }

    RosHeader header;
    yCTrace(TCPROSCARRIER, "Writing to %s", proto.getStreams().getRemoteAddress().toString().c_str());
    yCTrace(TCPROSCARRIER, "Writing from %s", proto.getStreams().getLocalAddress().toString().c_str());

    std::string rtyp = getRosType(proto);
    if (rtyp!="") {
        header.data["type"] = rtyp;
    }
    header.data[mode] = modeValue;
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
    RosHeader::appendInt32(at,header_serial.length());
    yCTrace(TCPROSCARRIER, "Writing %s -- %d bytes",
               RosHeader::showMessage(header_len).c_str(),
               (int)header_len.length());

    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    yCTrace(TCPROSCARRIER, "Writing %s -- %d bytes",
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
        yCWarning(TCPROSCARRIER, "Fail %s %d", __FILE__, __LINE__);
        return false;
    }
    int len = NetType::netInt(mlen_buf);
    yCTrace(TCPROSCARRIER, "Len %d", len);
    if (len>10000) {
        yCWarning(TCPROSCARRIER, "not ready for serious messages");
        return false;
    }
    ManagedBytes m(len);
    res = proto.is().readFull(m.bytes());
    if (res!=len) {
        yCWarning(TCPROSCARRIER, "Fail %s %d", __FILE__, __LINE__);
        return false;
    }
    header.readHeader(string(m.get(),m.length()));
    yCTrace(TCPROSCARRIER, "Message header: %s", header.toString().c_str());
    std::string rosname;
    if (header.data.find("type")!=header.data.end()) {
        rosname = header.data["type"];
    }
    yCTrace(TCPROSCARRIER, "<incoming> Type of data is [%s]s", rosname.c_str());
    if (header.data.find("callerid")!=header.data.end()) {
        string name = header.data["callerid"];
        yCTrace(TCPROSCARRIER, "<incoming> callerid is %s", name.c_str());
        yCTrace(TCPROSCARRIER, "Route was %s", proto.getRoute().toString().c_str());
        Route route = proto.getRoute();
        route.setToName(name);
        proto.setRoute(route);
        yCTrace(TCPROSCARRIER, "Route is now %s", proto.getRoute().toString().c_str());
    }

    if (!isService) {
        isService = (header.data.find("request_type")!=header.data.end());
    }
    if (rosname!="" && (user_type != wire_type || user_type == "")) {
        kind = TcpRosStream::rosToKind(rosname.c_str());
        TcpRosStream::configureTwiddler(twiddler,kind.c_str(),rosname.c_str(),false,false);
        translate = TCPROS_TRANSLATE_TWIDDLER;
    } else {
        rosname = "";
    }
    yCTrace(TCPROSCARRIER, "tcpros %s mode", isService?"service":"topic");

    // we may be a pull stream
    sender = isService;

    processRosHeader(header);

    auto* stream = new TcpRosStream(proto.giveStreams(),
                                    sender,
                                    sender,
                                    isService,
                                    raw,
                                    rosname.c_str());

    if (stream==nullptr) { return false; }

    yCTrace(TCPROSCARRIER, "Getting ready to hand off streams...");

    proto.takeStreams(stream);

    return proto.is().isOk();
}

bool TcpRosCarrier::expectSenderSpecifier(ConnectionState& proto) {
    Route route = proto.getRoute();
    route.setFromName("tcpros");
    proto.setRoute(route);
    yCTrace(TCPROSCARRIER, "Trying for tcpros header");
    ManagedBytes m(headerLen1);
    Bytes mrem(m.get()+4,m.length()-4);
    NetInt32 ni = headerLen2;
    memcpy(m.get(),(char*)(&ni), 4);
    yCTrace(TCPROSCARRIER, "reading %d bytes", (int)mrem.length());
    int res = proto.is().readFull(mrem);
    yCTrace(TCPROSCARRIER, "read %d bytes", res);
    if (res!=(int)mrem.length()) {
        if (res>=0) {
            yCError(TCPROSCARRIER, "TCPROS header failure, expected %d bytes, got %d bytes",
                    (int)mrem.length(),res);
        } else {
            yCError(TCPROSCARRIER, "TCPROS connection has gone terribly wrong");
        }
        return false;
    }
    RosHeader header;
    header.readHeader(string(m.get(),m.length()));
    yCTrace(TCPROSCARRIER, "Got header %s", header.toString().c_str());

    std::string rosname;
    if (header.data.find("type")!=header.data.end()) {
        rosname = header.data["type"];
    }
    std::string rtyp = getRosType(proto);
    if (rtyp!="") {
        rosname = rtyp;
        header.data["type"] = rosname;
        header.data["md5sum"] = (md5sum!="")?md5sum:"*";
        if (message_definition!="") {
            header.data["message_definition"] = message_definition;
        }
    }
    yCTrace(TCPROSCARRIER, "<outgoing> Type of data is %s", rosname.c_str());

    route = proto.getRoute();
    if (header.data.find("callerid")!=header.data.end()) {
        route.setFromName(header.data["callerid"]);
    } else {
        route.setFromName("tcpros");
    }
    proto.setRoute(route);

    // Let's just ignore everything that is sane and holy, and
    // send the same header right back.
    // **UPDATE** Oh, ok, let's modify the callerid.  Begrudgingly.
    NestedContact nc(proto.getRoute().getToName());
    header.data["callerid"] = nc.getNodeName();

    string header_serial = header.writeHeader();
    string header_len(4,'\0');
    char *at = (char*)header_len.c_str();
    RosHeader::appendInt32(at,header_serial.length());
    yCTrace(TCPROSCARRIER, "Writing %s -- %d bytes",
               RosHeader::showMessage(header_len).c_str(),
               (int)header_len.length());

    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    yCTrace(TCPROSCARRIER, "Writing %s -- %d bytes",
               RosHeader::showMessage(header_serial).c_str(),
               (int)header_serial.length());
    Bytes b2((char*)header_serial.c_str(),header_serial.length());
    proto.os().write(b2);

    if (header.data.find("probe")!=header.data.end()) {
        yCTrace(TCPROSCARRIER, "================PROBE===============");
        return false;
    }


    if (!isService) {
        isService = (header.data.find("service")!=header.data.end());
    }
    if (rosname!="" && (user_type != wire_type || user_type == "")) {
        if (wire_type!="sensor_msgs/Image") { // currently using a custom method for images
            kind = TcpRosStream::rosToKind(rosname.c_str());
            TcpRosStream::configureTwiddler(twiddler,kind.c_str(),rosname.c_str(),true,true);
            translate = TCPROS_TRANSLATE_TWIDDLER;
        }
    } else {
        rosname = "";
    }
    sender = isService;

    processRosHeader(header);

    if (isService) {
        auto* stream = new TcpRosStream(proto.giveStreams(),
                                        sender,
                                        false,
                                        isService,
                                        raw,
                                        rosname.c_str());
        if (stream==nullptr) { return false; }
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
            yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_UNKNOWN");
            FlexImage *img = nullptr;
            if (user_type=="yarp/image"||user_type=="yarp/bottle") {
                img = wi.checkForImage(writer);
            }
            if (img) {
                translate = TCPROS_TRANSLATE_IMAGE;
                std::string frame = "/frame";
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
            yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_IMAGE");
            FlexImage *img = wi.checkForImage(writer);
            if (img==nullptr) {
                yCError(TCPROSCARRIER, "TCPROS Expected an image, but did not get one.");
                return false;
            }
            ri.update(img,seq,Time::now());  // Time here is the timestamp of the ROS message, so Time::now(), the mutable one is correct.
            seq++;
            flex_writer = &ri;
        }
        break;
    case TCPROS_TRANSLATE_BOTTLE_BLOB:
        {
            yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_BOTTLE_BLOB");
            if (!WireBottle::extractBlobFromBottle(writer,wt)) {
                yCError(TCPROSCARRIER, "TCPROS Expected a bottle blob, but did not get one.");
                return false;
            }
            flex_writer = &wt;
        }
        break;
    case TCPROS_TRANSLATE_TWIDDLER:
        {
            yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_TWIDDLER");
            twiddler_output.attach(writer,twiddler);
            if (twiddler_output.update()) {
                flex_writer = &twiddler_output;
            } else {
                flex_writer = nullptr;
            }
        }
        break;
    case TCPROS_TRANSLATE_INHIBIT:
        yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_INHIBIT");
        break;
    default:
        yCTrace(TCPROSCARRIER, "* TCPROS_TRANSLATE_OTHER");
        break;
    }

    if (flex_writer == nullptr) {
        return false;
    }

    int len = 0;
    for (size_t i=0; i<flex_writer->length(); i++) {
        len += (int)flex_writer->length(i);
    }
    yCTrace(TCPROSCARRIER, "Prepping to write %d blocks (%d bytes)",
               (int)flex_writer->length(),
               len);

    string header_len(4,'\0');
    char *at = (char*)header_len.c_str();
    RosHeader::appendInt32(at,len);
    Bytes b1((char*)header_len.c_str(),header_len.length());
    proto.os().write(b1);
    flex_writer->write(proto.os());

    yCTrace(TCPROSCARRIER, "done sending");

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
        yCInfo(TCPROSCARRIER, "tcpros disconnect not implemented yet in this direction ");
        return -1;
        break;
    case YARP_ENACT_EXISTS:
        yCInfo(TCPROSCARRIER, "tcpros connection check not implemented yet in this direction ");
        return -1;
        break;
    }

    if (!reversed) return -1;

    Contact fullDest = dest;
    if (fullDest.getPort()<=0) {
        fullDest = NetworkBase::queryName(fullDest.getName());
    }

    Contact fullSrc = src;
    if (fullSrc.getPort()<=0) {
        fullSrc = NetworkBase::queryName(fullSrc.getName());
    }

    Name n(style.carrier + "://test");
    std::string topic = n.getCarrierModifier("topic");
    if (topic=="") {
        yCInfo(TCPROSCARRIER, "Warning, no topic!");
        topic = "notopic";
    }

    RosSlave slave;
    yCTrace(TCPROSCARRIER, "Starting temporary slave");
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
    yCTrace(TCPROSCARRIER, "%s",reply.toString().c_str());
    if (!ok) {
        yCError(TCPROSCARRIER, "error talking to %s", fullSrc.toString().c_str());
    }
    slave.stop();
    if (!slave.isOk()) {
        yCError(TCPROSCARRIER, "Problem: did not get a callback from ROS - can happen if connection already exists.");
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
