/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarkerInit" msg definition:
//   # Identifying string. Must be unique in the topic namespace
//   # that this server works on.
//   string server_id
//   
//   # Sequence number.
//   # The client will use this to detect if it has missed a subsequent
//   # update.  Every update message will have the same sequence number as
//   # an init message.  Clients will likely want to unsubscribe from the
//   # init topic after a successful initialization to avoid receiving
//   # duplicate data.
//   uint64 seq_num
//   
//   # All markers.
//   InteractiveMarker[] markers
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarkerInit_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarkerInit_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarker.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarkerInit : public yarp::os::idl::WirePortable
{
public:
    std::string server_id;
    yarp::os::NetUint64 seq_num;
    std::vector<yarp::rosmsg::visualization_msgs::InteractiveMarker> markers;

    InteractiveMarkerInit() :
            server_id(""),
            seq_num(0),
            markers()
    {
    }

    void clear()
    {
        // *** server_id ***
        server_id = "";

        // *** seq_num ***
        seq_num = 0;

        // *** markers ***
        markers.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** server_id ***
        int len = connection.expectInt();
        server_id.resize(len);
        if (!connection.expectBlock((char*)server_id.c_str(), len)) {
            return false;
        }

        // *** seq_num ***
        if (!connection.expectBlock((char*)&seq_num, 8)) {
            return false;
        }

        // *** markers ***
        len = connection.expectInt();
        markers.resize(len);
        for (int i=0; i<len; i++) {
            if (!markers[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** server_id ***
        if (!reader.readString(server_id)) {
            return false;
        }

        // *** seq_num ***
        seq_num = reader.expectInt();

        // *** markers ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        markers.resize(len);
        for (int i=0; i<len; i++) {
            if (!markers[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::read;
    bool read(yarp::os::ConnectionReader& connection) override
    {
        return (connection.isBareMode() ? readBare(connection)
                                        : readBottle(connection));
    }

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** server_id ***
        connection.appendInt(server_id.length());
        connection.appendExternalBlock((char*)server_id.c_str(), server_id.length());

        // *** seq_num ***
        connection.appendBlock((char*)&seq_num, 8);

        // *** markers ***
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** server_id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(server_id.length());
        connection.appendExternalBlock((char*)server_id.c_str(), server_id.length());

        // *** seq_num ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)seq_num);

        // *** markers ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerInit> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerInit> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# Identifying string. Must be unique in the topic namespace\n\
# that this server works on.\n\
string server_id\n\
\n\
# Sequence number.\n\
# The client will use this to detect if it has missed a subsequent\n\
# update.  Every update message will have the same sequence number as\n\
# an init message.  Clients will likely want to unsubscribe from the\n\
# init topic after a successful initialization to avoid receiving\n\
# duplicate data.\n\
uint64 seq_num\n\
\n\
# All markers.\n\
InteractiveMarker[] markers\n\
") + std::string("\n\
================================================================================\n\
MSG: visualization_msgs/InteractiveMarker\n\
") + yarp::rosmsg::visualization_msgs::InteractiveMarker::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::InteractiveMarkerInit::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarkerInit", "visualization_msgs/InteractiveMarkerInit");
        typ.addProperty("md5sum", yarp::os::Value("d5f2c5045a72456d228676ab91048734"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerInit_h
