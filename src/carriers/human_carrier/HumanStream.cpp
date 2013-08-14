// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <string.h>
#include "HumanStream.h"

YARP_SSIZE_T HumanStream::read(const Bytes& b) {
    if (interrupting) { return -1; }
    while (inputCache.size() < (unsigned int)b.length()) {
        cout << "*** CHECK OTHER TERMINAL FOR SOMETHING TO TYPE:"
             << endl;
        char buf[1000];
        needInterrupt = true;  // should be mutexed, in real implementation
        cin.getline(buf,1000);
        needInterrupt = false;
        if (interrupting) { return -1; }
        inputCache += buf;
        inputCache += "\r\n";
        cout << "Thank you" << endl;
    }
    memcpy(b.get(),inputCache.c_str(),b.length());
    inputCache = inputCache.substr(b.length());
    return b.length();
}

void HumanStream::write(const Bytes& b) {
    outputCache.append(b.get(),b.length());
    while (outputCache.find("\n")!=string::npos) {
        size_t idx = outputCache.find("\n");
        string show;
        show.append(outputCache.c_str(),idx);
        cout << "*** TYPE THIS ON THE OTHER TERMINAL: " << show << endl;
        outputCache = outputCache.substr(idx+1);
        Time::delay(1);
    }
}
