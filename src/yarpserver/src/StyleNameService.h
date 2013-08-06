// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_STYLENAMESERVICE_INC
#define YARPDB_STYLENAMESERVICE_INC

#include <yarp/os/Property.h>
#include <yarp/name/NameService.h>

namespace yarp {
    namespace name {
        class StyleNameService;
    }
}

class yarp::name::StyleNameService : public NameService {
public:
    StyleNameService() {
    }

    bool configure(const yarp::os::Searchable& options) {
        this->options.fromString(options.toString());
        return true;
    }

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote);
private:
    yarp::os::Property options;
    yarp::os::Property content;
    yarp::os::Property mime;
};

#endif
