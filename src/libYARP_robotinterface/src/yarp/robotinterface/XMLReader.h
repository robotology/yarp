/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
