/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEV1_H
#define YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEV1_H

#include <yarp/robotinterface/impl/XMLReaderFileVx.h>

#include <string>

namespace yarp {
namespace robotinterface {

namespace experimental {
class XMLReaderResult;
} // namespace experimental

namespace impl {

class XMLReaderFileV1 : public XMLReaderFileVx
{
public:
    XMLReaderFileV1();
    ~XMLReaderFileV1() override;

    yarp::robotinterface::experimental::XMLReaderResult getRobotFromFile(const std::string& filename,
                                                                         const yarp::os::Searchable& config,
                                                                         bool verbose = false) override;
    yarp::robotinterface::experimental::XMLReaderResult getRobotFromString(const std::string& xmlString,
                                                                           const yarp::os::Searchable& config,
                                                                           bool verbose = false) override;

private:
    class Private;
    Private* const mPriv;
};

} // namespace impl
} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADERFILEV1_H
