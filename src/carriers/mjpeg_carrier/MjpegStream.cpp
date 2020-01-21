/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MjpegStream.h"
#include "MjpegDecompression.h"

#include <yarp/os/Log.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <cstdio>
#include <cstring>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

yarp::conf::ssize_t MjpegStream::read(Bytes& b) {
    bool debug = false;
    if (remaining==0) {
        if (phase==1) {
            phase = 2;
            cursor = (char*)(img.getRawImage());
            remaining = img.getRawImageSize();
        } else if (phase==3) {
            phase = 4;
            cursor = nullptr;
            remaining = blobHeader.blobLen;
        } else {
            phase = 0;
        }
    }
    while (phase==0 && delegate->getInputStream().isOk()) {
        std::string s;
        do {
            s = delegate->getInputStream().readLine();
            if (debug) {
                printf("Read \"%s\"\n", s.c_str());
            }
        } while ((s.length()==0||s[0]!='-') && delegate->getInputStream().isOk());
        s = delegate->getInputStream().readLine();
        if (s!="Content-Type: image/jpeg") {
            if (!delegate->getInputStream().isOk()) {
                break;
            }
            printf("Unknown content type - \"%s\"\n", s.c_str());
            continue;
        }
        if (debug) {
            printf("Read content type - \"%s\"\n", s.c_str());
        }
        s = delegate->getInputStream().readLine();
        if (debug) {
            printf("Read content length - \"%s\"\n", s.c_str());
        }
        Bottle b(s);
        if (b.get(0).asString()!="Content-Length:") {
            if (!delegate->getInputStream().isOk()) {
                break;
            }
            printf("Expected Content-Length: got - \"%s\"\n", b.get(0).asString().c_str());
            continue;
        }
        int len = b.get(1).asInt32();
        if (debug) {
            printf("Length is \"%d\"\n", len);
        }
        do {
            s = delegate->getInputStream().readLine();
            if (debug) {
                printf("Read \"%s\"\n", s.c_str());
            }
        } while (s.length()>0);
        if (autocompress) {
            cimg.allocate(len);
            delegate->getInputStream().readFull(cimg.bytes());
            if (!decompression.decompress(cimg.bytes(), img)) {
                if (delegate->getInputStream().isOk()) {
                    yError("Skipping a problematic JPEG frame");
                }
            }
            imgHeader.setFromImage(img);
            phase = 1;
            cursor = (char*)(&imgHeader);
            remaining = sizeof(imgHeader);
        } else {
            // User wants us to pass on jpeg bytes, without compressing.
            // This can save time if not every frame will be used.

            // We pass on the information in bottle-blob-compatible format.
            phase = 3;
            blobHeader.init(len);
            cursor = (char*)(&blobHeader);
            remaining = sizeof(blobHeader);
        }
    }

    if (remaining>0) {
        int allow = remaining;
        if ((int)b.length()<allow) {
            allow = b.length();
        }
        if (cursor!=nullptr) {
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            if (debug) printf("returning %d bytes\n", allow);
            return allow;
        } else {
            int result = delegate->getInputStream().read(b);
            if (debug) printf("Read %d bytes\n", result);
            if (result>0) {
                remaining-=result;
                if (debug) printf("%d bytes of meat\n", result);
                return result;
            }
        }
    }
    return -1;
}


void MjpegStream::write(const Bytes& b) {
    delegate->getOutputStream().write(b);
}
