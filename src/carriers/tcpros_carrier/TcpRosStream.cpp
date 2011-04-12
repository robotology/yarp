// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "TcpRosStream.h"

#include <yarp/os/impl/NetType.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>

#include "WireBottle.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;

#define dbg_printf if (0) printf

int TcpRosStream::read(const Bytes& b) {
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
                cursor = NULL;
                remaining = header.blobLen;
            }
        } else {
            scan.clear();
            phase = 0;
        }
    }
    if (phase==0) {
        if (expectTwiddle) {
            // I have no idea what this is yet, but let's consume it for now.
            // It is probably frightfully important.
            char twiddle[1];
            Bytes twiddle_buf(twiddle,1);
            NetType::readFull(delegate->getInputStream(),twiddle_buf);
        }

        char mlen[4];
        Bytes mlen_buf(mlen,4);
        int res = NetType::readFull(delegate->getInputStream(),mlen_buf);
        if (res<4) {
            dbg_printf("tcpros_carrier failed, %s %d\n", __FILE__, __LINE__);
            phase = -1;
            return -1;
        }
        int len = NetType::netInt(mlen_buf);
        dbg_printf("Unit length %d\n", len);

        if (raw==-1) {
            scan.allocate(len);
            int res = NetType::readFull(delegate->getInputStream(),
                                        scan.bytes());
            dbg_printf("Read %d bytes with raw==-1\n", res);
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
                cursor = NULL;
                remaining = header.blobLen;
            }
        } else {
            phase = 1;
            cursor = (char*) &header;
            remaining = sizeof(header);
        }
    }
    if (remaining>0) {
        if (cursor!=NULL) {
            int allow = remaining;
            if (b.length()<allow) {
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
    delegate->getOutputStream().write(b);
}


void TcpRosStream::updateKind(const char *kind) {
    string code = rosToKind(kind);
    if (code!="") {
        code = string("skip int32 * ") + code;
        twiddler.configure(code.c_str());
        this->kind = code.c_str();
    } else {
        this->kind = "";
    }
}


std::string TcpRosStream::rosToKind(const char *rosname) {
    // stub for testing

    string t = rosname;
    string code = "";
    if (t=="std_msgs/String") {
        code = "vector string 1 *";
    } else if (t == "std_msgs/Int32") {
        code = "vector int32 1 *";
    }
    return code;
}

