/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/experimental/XMLReader.h>

#include <yarp/robotinterface/experimental/Action.h>
#include <yarp/robotinterface/experimental/Device.h>
#include <yarp/robotinterface/experimental/Param.h>
#include <yarp/robotinterface/experimental/Robot.h>
#include <yarp/robotinterface/experimental/Types.h>
#include <yarp/robotinterface/experimental/XMLReader.h>
#include <yarp/robotinterface/impl/XMLReaderFileVx.h>
#include <yarp/robotinterface/impl/XMLReaderFileV1.h>
#include <yarp/robotinterface/impl/XMLReaderFileV3.h>
#include <yarp/robotinterface/impl/RobotInterfaceDTD.h>

#include <yarp/conf/filesystem.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <tinyxml.h>
#include <vector>

#define SYNTAX_ERROR(line) yError() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."

// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0

class yarp::robotinterface::experimental::XMLReader::Private
{
public:
    ~Private()
    {
        delete mReader;
    }

    bool verbose{false};
    bool enable_deprecated{false};
    yarp::robotinterface::impl::XMLReaderFileVx* mReader{nullptr};
};


yarp::robotinterface::experimental::XMLReader::XMLReader() :
        mPriv(new Private)
{
}

yarp::robotinterface::experimental::XMLReader::~XMLReader()
{
    delete mPriv;
}

void yarp::robotinterface::experimental::XMLReader::setVerbose(bool verb)
{
    mPriv->verbose = verb;
}

void yarp::robotinterface::experimental::XMLReader::setEnableDeprecated(bool enab)
{
    mPriv->enable_deprecated = enab;
}

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::experimental::XMLReader::getRobotFromFile(const std::string& fileName,
                                                                                                                    const yarp::os::Searchable& config)
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
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
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

    if (dtd.majorVersion == 1) {
        yError() << "DTD V1.x has been deprecated. Please update your configuration files to DTD v3.x";
        if (mPriv->enable_deprecated) {
            yWarning() << "yarprobotinterface: using DEPRECATED xml parser for DTD v1.x";
            mPriv->mReader = new yarp::robotinterface::impl::XMLReaderFileV1;
            return mPriv->mReader->getRobotFromFile(filename, config, mPriv->verbose);
        }

        yError("Invalid DTD version, execution stopped.");
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();

    }
    if (dtd.majorVersion == 3) {
        yDebug() << "yarprobotinterface: using xml parser for DTD v3.x";
        mPriv->mReader = new yarp::robotinterface::impl::XMLReaderFileV3;
        return mPriv->mReader->getRobotFromFile(filename, config, mPriv->verbose);
    }

    //ERROR HERE
    yError("Invalid DTD version. Unable to choose parser for DTD.major: %d", dtd.majorVersion);
    return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
}

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::experimental::XMLReader::getRobotFromString(const std::string& xmlString,
                                                                                                                      const yarp::os::Searchable& config)
{
    std::string curr_filename = " XML runtime string ";
    std::unique_ptr<TiXmlDocument> doc = std::make_unique<TiXmlDocument>();
    if (!doc->Parse(xmlString.data())) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }
    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    RobotInterfaceDTD dtd;

    for (TiXmlNode* childNode = doc->FirstChild(); childNode != nullptr; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            std::string curr_filename = " XML runtime string ";
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

    if (dtd.majorVersion == 1) {
        yError() << "DTD V1.x has been deprecated. Please update your configuration files to DTD v3.x";
        if (mPriv->enable_deprecated) {
            yWarning() << "yarprobotinterface: using DEPRECATED xml parser for DTD v1.x";
            mPriv->mReader = new yarp::robotinterface::impl::XMLReaderFileV1;
            return mPriv->mReader->getRobotFromString(xmlString, config, mPriv->verbose);
        } else {
            yError("Invalid DTD version, execution stopped.");
            return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
        }
    } else if (dtd.majorVersion == 3) {
        yDebug() << "yarprobotinterface: using xml parser for DTD v3.x";
        mPriv->mReader = new yarp::robotinterface::impl::XMLReaderFileV3;
        return mPriv->mReader->getRobotFromString(xmlString, config, mPriv->verbose);
    }

    //ERROR HERE
    yError("Invalid DTD version. Unable to choose parser for DTD.major: %d", dtd.majorVersion);
    return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
}
