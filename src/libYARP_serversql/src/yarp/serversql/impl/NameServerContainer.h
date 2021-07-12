/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_IMPL_NAMESERVERCONTAINER_H
#define YARP_SERVERSQL_IMPL_NAMESERVERCONTAINER_H

#include <yarp/os/Contact.h>
#include <yarp/os/NameSpace.h>
#include <yarp/os/Searchable.h>

#include <yarp/serversql/impl/AllocatorOnTriples.h>
#include <yarp/serversql/impl/ComposedNameService.h>
#include <yarp/serversql/impl/NameServiceOnTriples.h>
#include <yarp/serversql/impl/StyleNameService.h>
#include <yarp/serversql/impl/SubscriberOnSql.h>
#include <yarp/serversql/impl/TripleSourceCreator.h>

namespace yarp {
namespace serversql {
namespace impl {

class NameServerContainer : public ComposedNameService
{
private:
    yarp::os::Contact contact;
    TripleSourceCreator db;
    SubscriberOnSql subscriber;
    AllocatorConfig config;
    AllocatorOnTriples alloc;
    NameServiceOnTriples ns;
    StyleNameService style;
    ComposedNameService combo1;
    yarp::os::NameSpace *space{nullptr};
public:
    using ComposedNameService::open;

    ~NameServerContainer() override;

    const yarp::os::Contact& where();
    yarp::os::Contact whereDelegate();
    void preregister(const yarp::os::Contact& c);
    bool open(yarp::os::Searchable& options);
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_NAMESERVERCONTAINER_H
