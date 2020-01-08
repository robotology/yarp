/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_XMLREADER_H
#define YARP_ROBOTINTERFACE_XMLREADER_H

#include <string>

#include <yarp/robotinterface/api.h>

namespace yarp {
namespace robotinterface {

class Robot;


class YARP_robotinterface_API XMLReaderFileVx
{
public:
    bool verbose;
    virtual ~XMLReaderFileVx() {};
    virtual  Robot& getRobotFile(const std::string &filename, bool verbose = false)=0;
};

class YARP_robotinterface_API XMLReaderFileV1 : public XMLReaderFileVx
{
public:
    XMLReaderFileV1();
    virtual ~XMLReaderFileV1();

    Robot& getRobotFile(const std::string &filename, bool verbose = false) override;

private:
    class privateXMLReaderFileV1;
    privateXMLReaderFileV1 * const mPriv;
};

class YARP_robotinterface_API XMLReaderFileV3: public XMLReaderFileVx
{
public:
    XMLReaderFileV3();
    virtual ~XMLReaderFileV3();

    Robot& getRobotFile(const std::string &filename, bool verbose = false) override;

private:
    class privateXMLReaderFileV3;
    privateXMLReaderFileV3 * const mPriv;
};

class YARP_robotinterface_API XMLReader
{
public:
    XMLReader();
    virtual ~XMLReader();

    Robot& getRobot(const std::string &filename);
    void setVerbose(bool verbose);
    void setEnableDeprecated(bool enab);
private:
    bool verbose;
    bool enable_deprecated;
    XMLReaderFileVx * mReader;
}; // class XMLReader

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADER_H
