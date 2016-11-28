/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H
#define YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H

#include <yarp/os/Property.h>
#include <yarp/name/NameService.h>


namespace yarp {
namespace serversql {
namespace impl {

class StyleNameService : public yarp::name::NameService {
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

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H
