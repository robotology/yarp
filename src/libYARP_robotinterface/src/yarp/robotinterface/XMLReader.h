/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_XMLREADER_H
#define YARP_ROBOTINTERFACE_XMLREADER_H

#include <yarp/robotinterface/api.h>
#include <yarp/robotinterface/Robot.h>

#include <yarp/os/Searchable.h>
#include <yarp/os/Property.h>

#include <string>


namespace yarp {
namespace robotinterface {

/**
 * Result of the parsing of XMLReader.
 */
class YARP_robotinterface_API XMLReaderResult
{
public:
    static XMLReaderResult ParsingFailed()
    {
        XMLReaderResult result;
        result.parsingIsSuccessful = false;
        return result;
    }

    /**
     * True if the parsing was successful, false otherwise.
     */
    bool parsingIsSuccessful = false;

    /**
     * If parsingIsSuccessful is true, contains a valid robot instance.
     */
    Robot robot;
};

class YARP_robotinterface_API XMLReader
{
public:
    XMLReader();
    virtual ~XMLReader();

    /**
     * Parse the XML description of a robotinterface from a file.
     *
     * \param filename path to the XML file to load.
     * \return result of parsing.
     */
    XMLReaderResult getRobotFromFile(const std::string& filename,
                                     const yarp::os::Searchable& config = yarp::os::Property());

    /**
     * Parse the XML description of a robotinterface from a string.
     *
     * \param xmlString string containing the XML code to parse.
     * \return result of parsing.
     */
    XMLReaderResult getRobotFromString(const std::string& filename,
                                       const yarp::os::Searchable& config = yarp::os::Property());

    void setVerbose(bool verbose);
    void setEnableDeprecated(bool enab);

private:
    class Private;
    Private* const mPriv;

}; // class XMLReader

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADER_H
