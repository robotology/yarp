/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_XMLREADER_H
#define ROBOTINTERFACE_XMLREADER_H

#include <iosfwd>

namespace RobotInterface
{
class Robot;

class XMLReader
{
public:
    XMLReader();
    virtual ~XMLReader();

    Robot& getRobot(const std::string &filename);

private:
    class Private;
    Private * const mPriv;
}; // class XMLReader

} // namespace RobotInterface

#endif // ROBOTINTERFACE_XMLREADER_H
