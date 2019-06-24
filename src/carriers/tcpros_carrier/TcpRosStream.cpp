/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "TcpRosStream.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <yarp/os/NetType.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>

#include <yarp/wire_rep_utils/WireBottle.h>

using namespace yarp::os;
using namespace yarp::wire_rep_utils;
using namespace std;

#define dbg_printf if (0) printf

yarp::conf::ssize_t TcpRosStream::read(Bytes& b) {
    if (!setInitiative) {
        initiative = false;
        setInitiative = true;
    }

    if (kind!="") {
      return twiddlerReader.read(b);
    }

    if (phase==-1) return -1;
    if (remaining==0) {
        if (phase==1) {
            phase = 2;
            if (scan.length()>0) {
                cursor = scan.get();
                remaining = scan.length();
            } else {
                cursor = nullptr;
                remaining = header.blobLen;
            }
        } else {
            scan.clear();
            phase = 0;
        }
    }
    if (phase==0) {
        if (expectTwiddle&&initiative) {
            // There's a success/failure byte to check.
            // Here we just consume it, but if it shows failure
            // we should in fact expect a string afterwards.
            char twiddle[1];
            Bytes twiddle_buf(twiddle,1);
            delegate->getInputStream().readFull(twiddle_buf);
        }

        char mlen[4];
        Bytes mlen_buf(mlen,4);
        int res = delegate->getInputStream().readFull(mlen_buf);
        if (res<4) {
            dbg_printf("tcpros_carrier failed, %s %d\n", __FILE__, __LINE__);
            phase = -1;
            return -1;
        }
        int len = NetType::netInt(mlen_buf);
        dbg_printf("Unit length %d\n", len);

        // inhibit type scanning for now, it is unreliable
        if (raw==-1) raw = 2;
        if (raw==-2) {
            scan.allocate(len);
            int res = delegate->getInputStream().readFull(scan.bytes());
            dbg_printf("Read %d bytes with raw==-2\n", res);
            if (res<0) {
                dbg_printf("tcpros_carrier failed, %s %d\n", __FILE__, __LINE__);
                phase = -1;
                return -1;
            }
            int len_scan = scan.length();
            if (WireBottle::checkBottle(scan.get(),len_scan)) {
                dbg_printf("Looks YARP-compatible\n");
                raw = 1;
            } else {
                dbg_printf("Looks strange, blobbing...\n");
                raw = 0;
            }
        }

        header.init(len);
        if (raw==1) {
            phase = 2;
            if (scan.length()>0) {
                cursor = scan.get();
                remaining = scan.length();
            } else {
                cursor = nullptr;
                remaining = header.blobLen;
            }
        } else if (raw==2) {
            cursor = nullptr;
            remaining = header.blobLen;
            phase = 2;
        } else {
            phase = 1;
            cursor = (char*) &header;
            remaining = sizeof(header);
        }
    }
    dbg_printf("phase %d remaining %d\n", phase, remaining);
    if (remaining>0) {
        if (cursor!=nullptr) {
            int allow = remaining;
            if ((int)b.length()<allow) {
                allow = b.length();
            }
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            dbg_printf("%d bytes of header\n", allow);
            return allow;
        } else {
            int result = delegate->getInputStream().read(b);
            dbg_printf("Read %d bytes\n", result);
            if (result>0) {
                remaining-=result;
                dbg_printf("%d bytes of meat\n", result);
                return result;
            }
        }
    }
    phase = -1;
    return -1;
}


void TcpRosStream::write(const Bytes& b) {
    if (!setInitiative) {
        initiative = true;
        setInitiative = true;
    }
    dbg_printf("                   [[[[[ write %d bytes ]]]]]\n", (int)b.length());
    delegate->getOutputStream().write(b);
}


void TcpRosStream::updateKind(const char *kind, bool sender, bool reply) {
    string code = rosToKind(kind);
    if (code!="") {
        configureTwiddler(twiddler,code.c_str(),kind,sender,reply);
        this->kind = code;
    } else {
        this->kind = "";
    }
}


std::map<std::string, std::string> TcpRosStream::rosToKind() {
    std::map<std::string, std::string> kinds;
    kinds["std_msgs/String"] = "vector string 1 *";
    kinds["std_msgs/Int32"] = "vector int32 1 *";
    kinds["std_msgs/Float64"] = "vector float64 1 *";

    // these two are specialized, TODO link them specifically to
    // yarp/image and yarp/vector
    kinds["sensor_msgs/Image"] = "list 4 skip uint32 * skip uint32 * skip uint32 * skip string *    >height uint32 * >width uint32 * >encoding string * skip int8 * >step int32 *  compute image_params    <=[mat] vocab * <translated_encoding vocab * item_vector int32 5 <depth item * <img_size item * <quantum item * <width item * <height item * blob *";

    kinds["test_roscpp/TestStringString"] = "vector string 1 * --- vector string 1 *";
    // kinds["rospy_tutorials/AddTwoInts"] = "vector int64 2 * --- vector int64 1 *";
    return kinds;
}

std::string TcpRosStream::rosToKind(const char *rosname) {
    if (std::string(rosname)=="") return {};
    std::map<std::string, std::string> kinds = rosToKind();

    if (kinds.find(rosname)!=kinds.end()) {
        return kinds[rosname];
    }
    Port port;
    port.openFake("yarpidl_rosmsg");
    if (port.addOutput("/typ")) {
        Bottle cmd, resp;
        cmd.addString(std::string("twiddle ") + rosname);
        dbg_printf("QUERY yarpidl_rosmsg %s\n", cmd.toString().c_str());
        port.write(cmd,resp);
        dbg_printf("GOT yarpidl_rosmsg %s\n", resp.toString().c_str());
        std::string txt = resp.get(0).asString();
        if (txt!="?") return txt;
    }
    port.close();
    if (std::string(rosname)!="") {
        fprintf(stderr, "Do not know anything about type '%s'\n", rosname);
        fprintf(stderr, "Could not connect to a type server to look up type '%s'\n", rosname);
        ::exit(1);
    }
    return {};
}


bool TcpRosStream::configureTwiddler(WireTwiddler& twiddler, const char *txt, const char *prompt, bool sender, bool reply) {
    dbg_printf("CONFIGURE AS %s [%s/%s]\n", txt,
               sender?"sender":"receiver",
               reply?"reply":"main");
    std::string str(txt);
    if (reply) {
        size_t idx = str.find("---");
        if (idx!=std::string::npos) {
            str = str.substr(idx+3,str.length());
        }
    }
    str = std::string("skip int32 * ") + str;
    if (reply) {
        str = std::string("skip int8 * ") + str;
    }
    return twiddler.configure(str.c_str(),prompt);
}
