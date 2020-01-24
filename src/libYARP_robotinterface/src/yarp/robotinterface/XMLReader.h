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
class XMLReaderFileVx;

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
