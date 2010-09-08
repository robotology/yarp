// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Ping.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

#if WIN32
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif 

void Ping::apply() {
    last.clear();
    double start = Time::now();
    Contact c = NetworkBase::queryName(target);
    double afterQuery = Time::now();
    if (!c.isValid()) {
        YARP_LOG_ERROR("Port not found during ping");
    }
    ContactStyle rpc;
    rpc.admin = true;
    rpc.quiet = true;
    Bottle cmd, reply;
    cmd.addVocab(Vocab::encode("ver"));
    bool ok = NetworkBase::write(c,cmd,reply,rpc);
    if (!ok) {
        YARP_LOG_ERROR("Port did not respond as expected");
    }
    double stop = Time::now();
    last.totalTime.add(stop-start);
    last.targetTime.add(stop-afterQuery);
    accum.add(last);
}

void Ping::report() {
    printf("Ping #%d:\n", (int)(accum.targetTime.count()+0.5));
    int space = 14;
    int decimal = 5;
    printf("  %s connection time (%s with name lookup)\n",
           renderTime(last.targetTime.mean(),space,decimal).c_str(), 
           renderTime(last.totalTime.mean(),space,decimal).c_str());
    if (accum.totalTime.count()>1) {
    printf("  %s +/- %s on average (%s +/- %s with name lookup)\n",
           renderTime(accum.targetTime.mean(),space,decimal).c_str(), 
           renderTime(accum.targetTime.deviation(),space,decimal).c_str(), 
           renderTime(accum.totalTime.mean(),space,decimal).c_str(),
           renderTime(accum.totalTime.deviation(),space,decimal).c_str());
    }
}


ConstString Ping::renderTime(double t, int space, int decimal) {
    ConstString unit = "";
    double times = 1;
    if (space<0) {
        YARP_LOG_ERROR("Negative space");
    }
    if (t>=1) {
        unit = "sec";
    } else if (t>1e-3) {
        unit = " ms";
        times = 1e3;
    } else if (t>1e-6) {
        unit = " us";
        times = 1e6;
    } else if (t>1e-9) {
        unit = " ns";
        times = 1e9;
    }
    char buf[512];
    safe_printf(buf,sizeof(buf),"%.*f%s",decimal,t*times,
                unit.c_str());
    return buf;
}


