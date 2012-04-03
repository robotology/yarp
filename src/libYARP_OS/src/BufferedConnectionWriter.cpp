// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;

bool BufferedConnectionWriter::applyConvertTextMode() {
    if (convertTextModePending) {
        convertTextModePending = false;

        Bottle b;
        StringOutputStream sos;
        for (size_t i=0; i<lst.size(); i++) {
            yarp::os::ManagedBytes& m = *(lst[i]);
            sos.write(m.usedBytes());
        }
        const String& str = sos.str();
        b.fromBinary(str.c_str(),str.length());
        ConstString replacement = b.toString() + "\n";
        for (size_t i=0; i<lst.size(); i++) {
            delete lst[i];
        }
        target = &lst;
        lst.clear();
        Bytes data((char*)replacement.c_str(),replacement.length());
        appendBlockCopy(data);
    }
    return true;
}

bool BufferedConnectionWriter::convertTextMode() {
    if (isTextMode()) {
        convertTextModePending = true;
    }
    return true;
}



bool BufferedConnectionWriter::forceConvertTextMode() {
    bool mode = textMode;
    textMode = true;
    convertTextMode();
    textMode = mode;
    return true;
}


