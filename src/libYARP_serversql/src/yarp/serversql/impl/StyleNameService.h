/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H
#define YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H

#include <yarp/os/Property.h>
#include <yarp/name/NameService.h>


namespace yarp {
namespace serversql {
namespace impl {

class StyleNameService : public yarp::name::NameService
{
public:
    StyleNameService() = default;

    bool configure(const yarp::os::Searchable& options)
    {
        this->options.fromString(options.toString());
        return true;
    }

    bool apply(yarp::os::Bottle& cmd,
               yarp::os::Bottle& reply,
               yarp::os::Bottle& event,
               const yarp::os::Contact& remote) override;

private:
    yarp::os::Property options;
    yarp::os::Property content;
    yarp::os::Property mime;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_STYLENAMESERVICE_H
