/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H
#define YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H

#include <yarp/robotinterface/Robot.h>

#include <string>

namespace yarp {
namespace robotinterface {

class XMLReaderFileVx
{
public:
    bool verbose;
    virtual ~XMLReaderFileVx() {};
    virtual  Robot& getRobotFile(const std::string &filename, bool verbose = false)=0;
};

class XMLReaderFileV1 : public XMLReaderFileVx
{
public:
    XMLReaderFileV1();
    virtual ~XMLReaderFileV1();

    Robot& getRobotFile(const std::string &filename, bool verbose = false) override;

private:
    class privateXMLReaderFileV1;
    privateXMLReaderFileV1 * const mPriv;
};

class XMLReaderFileV3: public XMLReaderFileVx
{
public:
    XMLReaderFileV3();
    virtual ~XMLReaderFileV3();

    Robot& getRobotFile(const std::string &filename, bool verbose = false) override;

private:
    class privateXMLReaderFileV3;
    privateXMLReaderFileV3 * const mPriv;
};

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADERFILEVX_H