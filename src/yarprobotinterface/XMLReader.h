/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_YARPROBOTINTERFACE_XMLREADER_H
#define YARP_YARPROBOTINTERFACE_XMLREADER_H

#include <string>

namespace RobotInterface
{
class Robot;


class XMLReaderFileVx
{
public:
    virtual  Robot& getRobotFile(const std::string &filename)=0;
};

class XMLReaderFileV1 : public XMLReaderFileVx
{
public:
    XMLReaderFileV1();
    virtual ~XMLReaderFileV1();

    virtual Robot& getRobotFile(const std::string &filename) override;

private:
    class privateXMLReaderFileV1;
    privateXMLReaderFileV1 * const mPriv;
};

class XMLReaderFileV3: public XMLReaderFileVx
{
public:
    XMLReaderFileV3();
    virtual ~XMLReaderFileV3();

    virtual Robot& getRobotFile(const std::string &filename) override;

private:
    class privateXMLReaderFileV3;
    privateXMLReaderFileV3 * const mPriv;
};

class XMLReader
{
public:
    XMLReader();
    virtual ~XMLReader();

    Robot& getRobot(const std::string &filename);

private:
    XMLReaderFileVx * mReader;
}; // class XMLReader

} // namespace RobotInterface

#endif // YARP_YARPROBOTINTERFACE_XMLREADER_H
