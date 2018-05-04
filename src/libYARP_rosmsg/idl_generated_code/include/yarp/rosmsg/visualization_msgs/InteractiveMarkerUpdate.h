/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarkerUpdate" msg definition:
//   # Identifying string. Must be unique in the topic namespace
//   # that this server works on.
//   string server_id
//   
//   # Sequence number.
//   # The client will use this to detect if it has missed an update.
//   uint64 seq_num
//   
//   # Type holds the purpose of this message.  It must be one of UPDATE or KEEP_ALIVE.
//   # UPDATE: Incremental update to previous state. 
//   #         The sequence number must be 1 higher than for
//   #         the previous update.
//   # KEEP_ALIVE: Indicates the that the server is still living.
//   #             The sequence number does not increase.
//   #             No payload data should be filled out (markers, poses, or erases).
//   uint8 KEEP_ALIVE = 0
//   uint8 UPDATE = 1
//   
//   uint8 type
//   
//   #Note: No guarantees on the order of processing.
//   #      Contents must be kept consistent by sender.
//   
//   #Markers to be added or updated
//   InteractiveMarker[] markers
//   
//   #Poses of markers that should be moved
//   InteractiveMarkerPose[] poses
//   
//   #Names of markers to be erased
//   string[] erases
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarkerUpdate_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarkerUpdate_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarker.h>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarkerPose.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarkerUpdate : public yarp::os::idl::WirePortable
{
public:
    std::string server_id;
    yarp::os::NetUint64 seq_num;
    static const unsigned char KEEP_ALIVE = 0;
    static const unsigned char UPDATE = 1;
    unsigned char type;
    std::vector<yarp::rosmsg::visualization_msgs::InteractiveMarker> markers;
    std::vector<yarp::rosmsg::visualization_msgs::InteractiveMarkerPose> poses;
    std::vector<std::string> erases;

    InteractiveMarkerUpdate() :
            server_id(""),
            seq_num(0),
            type(0),
            markers(),
            poses(),
            erases()
    {
    }

    void clear()
    {
        // *** server_id ***
        server_id = "";

        // *** seq_num ***
        seq_num = 0;

        // *** KEEP_ALIVE ***

        // *** UPDATE ***

        // *** type ***
        type = 0;

        // *** markers ***
        markers.clear();

        // *** poses ***
        poses.clear();

        // *** erases ***
        erases.clear();
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

        // *** type ***
        if (!connection.expectBlock((char*)&type, 1)) {
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

        // *** poses ***
        len = connection.expectInt();
        poses.resize(len);
        for (int i=0; i<len; i++) {
            if (!poses[i].read(connection)) {
                return false;
            }
        }

        // *** erases ***
        len = connection.expectInt();
        erases.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt();
            erases[i].resize(len2);
            if (!connection.expectBlock((char*)erases[i].c_str(), len2)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** server_id ***
        if (!reader.readString(server_id)) {
            return false;
        }

        // *** seq_num ***
        seq_num = reader.expectInt();

        // *** type ***
        type = reader.expectInt();

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

        // *** poses ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        poses.resize(len);
        for (int i=0; i<len; i++) {
            if (!poses[i].read(connection)) {
                return false;
            }
        }

        // *** erases ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {
            return false;
        }
        len = connection.expectInt();
        erases.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt();
            erases[i].resize(len2);
            if (!connection.expectBlock((char*)erases[i].c_str(), len2)) {
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

        // *** type ***
        connection.appendBlock((char*)&type, 1);

        // *** markers ***
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** poses ***
        connection.appendInt(poses.size());
        for (size_t i=0; i<poses.size(); i++) {
            if (!poses[i].write(connection)) {
                return false;
            }
        }

        // *** erases ***
        connection.appendInt(erases.size());
        for (size_t i=0; i<erases.size(); i++) {
            connection.appendInt(erases[i].length());
            connection.appendExternalBlock((char*)erases[i].c_str(), erases[i].length());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(8);

        // *** server_id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(server_id.length());
        connection.appendExternalBlock((char*)server_id.c_str(), server_id.length());

        // *** seq_num ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)seq_num);

        // *** type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)type);

        // *** markers ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** poses ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(poses.size());
        for (size_t i=0; i<poses.size(); i++) {
            if (!poses[i].write(connection)) {
                return false;
            }
        }

        // *** erases ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
        connection.appendInt(erases.size());
        for (size_t i=0; i<erases.size(); i++) {
            connection.appendInt(erases[i].length());
            connection.appendExternalBlock((char*)erases[i].c_str(), erases[i].length());
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerUpdate> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerUpdate> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# Identifying string. Must be unique in the topic namespace\n\
# that this server works on.\n\
string server_id\n\
\n\
# Sequence number.\n\
# The client will use this to detect if it has missed an update.\n\
uint64 seq_num\n\
\n\
# Type holds the purpose of this message.  It must be one of UPDATE or KEEP_ALIVE.\n\
# UPDATE: Incremental update to previous state. \n\
#         The sequence number must be 1 higher than for\n\
#         the previous update.\n\
# KEEP_ALIVE: Indicates the that the server is still living.\n\
#             The sequence number does not increase.\n\
#             No payload data should be filled out (markers, poses, or erases).\n\
uint8 KEEP_ALIVE = 0\n\
uint8 UPDATE = 1\n\
\n\
uint8 type\n\
\n\
#Note: No guarantees on the order of processing.\n\
#      Contents must be kept consistent by sender.\n\
\n\
#Markers to be added or updated\n\
InteractiveMarker[] markers\n\
\n\
#Poses of markers that should be moved\n\
InteractiveMarkerPose[] poses\n\
\n\
#Names of markers to be erased\n\
string[] erases\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: visualization_msgs/InteractiveMarker\n\
") + yarp::rosmsg::visualization_msgs::InteractiveMarker::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: visualization_msgs/InteractiveMarkerPose\n\
") + yarp::rosmsg::visualization_msgs::InteractiveMarkerPose::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::InteractiveMarkerUpdate::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarkerUpdate", "visualization_msgs/InteractiveMarkerUpdate");
        typ.addProperty("md5sum", yarp::os::Value("710d308d0a9276d65945e92dd30b3946"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerUpdate_h
