// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/ConstString.h>

#include "PortMonitor.h"


using namespace yarp::os;

#ifdef _MSC_VER
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif 


/**
 * Class PortMonitor
 */

// Read connection settings.
bool PortMonitor::configure(yarp::os::ConnectionState& proto) 
{    
    //printf("configure\n");
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    
    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    ConstString script = options.check("script", Value("lua")).asString();
    if((binder = MonitorBinding::create(script.c_str())) == NULL)
    {
         YARP_LOG_ERROR("Currently only \'lua\' scripting is supported by portmonitor");
         return false;
    }
   
    ConstString filename = options.check("file", Value("modifier")).asString();
    return (bReady =  binder->loadScript(filename.c_str()));
}

void PortMonitor::setCarrierParams(const yarp::os::Property& params) 
{
    if(binder == NULL)
        return;
    binder->setParams(params);
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) 
{
    if(binder == NULL)
        return;
    binder->getParams(params);
}


yarp::os::ConnectionReader& PortMonitor::modifyIncomingData(yarp::os::ConnectionReader& reader) 
{
    //printf("modifiesIncomingData\n");
     if(binder == NULL)
        return reader;

    return binder->updateData(reader);

    /*
    // minimal test of image modification
    in.read(reader);
    out.copy(in);
    out.pixel(0,0).r = 42;
    out.write(con.getWriter());
    return con.getReader();
    */

    
    // minimal test of bottle modification
    /*
    con.setTextMode(reader.isTextMode());
    Bottle b;
    b.read(reader);
    b.addInt(42);
    b.addString("(p.s. bork bork bork)");
    b.write(con.getWriter());
    return con.getReader();
    */
    //return *local;
}

YARP_SSIZE_T PortMonitor::read(const yarp::os::Bytes& b) 
{
//    printf("read\n");

    /*
    // copy across small stuff - the image header
    if (consumed<sizeof(header)) {
        size_t len = b.length();
        if (len>sizeof(header)-consumed) {
            len = sizeof(header)-consumed;
        }
        memcpy(b.get(),((char*)(&header))+consumed,len);
        consumed += len;
        return (YARP_SSIZE_T) len;
    }
    // sane client will want to read image into correct-sized block
    if (b.length()==image_data_len) {
        // life is good!
        processDirect(b);
        consumed += image_data_len;
        return image_data_len;
    }
    // funky client, fall back on image copy
    processBuffered();
    if (consumed<sizeof(header)+out.getRawImageSize()) {
        size_t len = b.length();
        if (len>sizeof(header)+out.getRawImageSize()-consumed) {
            len = sizeof(header)+out.getRawImageSize()-consumed;
        }
        memcpy(b.get(),out.getRawImage()+consumed-sizeof(header),len);
        consumed += len;
        return (YARP_SSIZE_T) len;
    }
    */
    return -1;
}

