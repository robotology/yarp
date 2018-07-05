/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#include "XMLReader.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"
#include "Robot.h"
#include "Types.h"
#include "RobotInterfaceDTD.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <tinyxml.h>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."


// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0

RobotInterface::XMLReader::XMLReader() :
    mReader(0)
{
    verbose = false;
}

RobotInterface::XMLReader::~XMLReader()
{
    if (mReader)
    {
        delete mReader;
        mReader = 0;
    }
}

void RobotInterface::XMLReader::setVerbose(bool verb)
{
    verbose = verb;
}

RobotInterface::Robot& RobotInterface::XMLReader::getRobot(const std::string& fileName)
{
    std::string filename = fileName;
#if defined(_WIN32)
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif
    std::string curr_filename = fileName;
    std::string path = filename.substr(0, filename.rfind(yarp::os::Network::getDirectorySeparator()));

    yDebug() << "Reading file" << filename.c_str();
    TiXmlDocument *doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    RobotInterfaceDTD dtd;

    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if (dtd.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!dtd.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        dtd.setDefault();
        dtd.type = RobotInterfaceDTD::DocTypeRobot;
    }

    if (dtd.type != RobotInterfaceDTD::DocTypeRobot) {
        SYNTAX_WARNING(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeRobot)
            << ". Found" << DocTypeToString(dtd.type);
    }

    if (dtd.majorVersion == 1)
    {
        mReader = new RobotInterface::XMLReaderFileV1;
        return mReader->getRobotFile(filename,verbose);
    }
    else if (dtd.majorVersion == 3)
    {
        mReader = new RobotInterface::XMLReaderFileV3;
        return mReader->getRobotFile(filename,verbose);
    }
    
    //ERROR HERE
    yFatal() << "Invalid DTD version. Unable to choose parser for DTD.major:" << dtd.majorVersion;
    RobotInterface::Robot* r=0;
    return *r;
}
