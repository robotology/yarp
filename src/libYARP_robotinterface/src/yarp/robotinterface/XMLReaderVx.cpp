/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "XMLReader.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"
#include "Robot.h"
#include "Types.h"
#include "impl/RobotInterfaceDTD.h"
#include "impl/XMLReaderFileVx.h"


#include <yarp/conf/filesystem.h>
#include <yarp/os/LogStream.h>
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

yarp::robotinterface::XMLReader::XMLReader() :
    mReader(nullptr)
{
    enable_deprecated = false;
    verbose = false;
}

yarp::robotinterface::XMLReader::~XMLReader()
{
    if (mReader)
    {
        delete mReader;
        mReader = nullptr;
    }
}

void yarp::robotinterface::XMLReader::setVerbose(bool verb)
{
    verbose = verb;
}

void yarp::robotinterface::XMLReader::setEnableDeprecated(bool enab)
{
    enable_deprecated = enab;
}

yarp::robotinterface::Robot& yarp::robotinterface::XMLReader::getRobot(const std::string& fileName)
{
    std::string filename = fileName;
#if defined(_WIN32)
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif
    std::string curr_filename = fileName;
    std::string path = filename.substr(0, filename.rfind(yarp::conf::filesystem::preferred_separator));

    yDebug() << "Reading file" << filename.c_str();
    auto* doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    RobotInterfaceDTD dtd;

    for (TiXmlNode* childNode = doc->FirstChild(); childNode != nullptr; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if (dtd.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!dtd.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV3.0";
        dtd.setDefault();
        dtd.type = RobotInterfaceDTD::DocTypeRobot;
    }

    if (dtd.type != RobotInterfaceDTD::DocTypeRobot) {
        SYNTAX_WARNING(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeRobot)
            << ". Found" << DocTypeToString(dtd.type);
    }

    if (dtd.majorVersion == 1)
    {
        yError() << "DTD V1.x has been deprecated. Please update your configuration files to DTD v3.x";
        if (enable_deprecated)
        {
            yWarning() << "yarprobotinterface: using DEPRECATED xml parser for DTD v1.x";
            mReader = new yarp::robotinterface::XMLReaderFileV1;
            return mReader->getRobotFile(filename, verbose);
        }
        else
        {
            yFatal("Invalid DTD version, execution stopped.");
        }
    }
    else if (dtd.majorVersion == 3)
    {
        yDebug() << "yarprobotinterface: using xml parser for DTD v3.x";
        mReader = new yarp::robotinterface::XMLReaderFileV3;
        return mReader->getRobotFile(filename,verbose);
    }

    //ERROR HERE
    yFatal("Invalid DTD version. Unable to choose parser for DTD.major: %d", dtd.majorVersion);
}
