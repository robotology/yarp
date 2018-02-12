// This is an automatically generated file.
// Generated from this sensor_msgs_RegionOfInterest.msg definition:
//   # This message is used to specify a region of interest within an image.
//   #
//   # When used to specify the ROI setting of the camera when the image was
//   # taken, the height and width fields should either match the height and
//   # width fields for the associated image; or height = width = 0
//   # indicates that the full resolution image was captured.
//   
//   uint32 x_offset  # Leftmost pixel of the ROI
//                    # (0 if the ROI includes the left edge of the image)
//   uint32 y_offset  # Topmost pixel of the ROI
//                    # (0 if the ROI includes the top edge of the image)
//   uint32 height    # Height of ROI
//   uint32 width     # Width of ROI
//   
//   # True if a distinct rectified ROI should be calculated from the "raw"
//   # ROI in this message. Typically this should be False if the full image
//   # is captured (ROI not used), and True if a subwindow is captured (ROI
//   # used).
//   bool do_rectify
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_RegionOfInterest
#define YARPMSG_TYPE_sensor_msgs_RegionOfInterest

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"

class sensor_msgs_RegionOfInterest : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetUint32 x_offset;
    yarp::os::NetUint32 y_offset;
    yarp::os::NetUint32 height;
    yarp::os::NetUint32 width;
    bool do_rectify;

    sensor_msgs_RegionOfInterest() :
            x_offset(0),
            y_offset(0),
            height(0),
            width(0),
            do_rectify(false)
    {
    }

    void clear()
    {
        // *** x_offset ***
        x_offset = 0;

        // *** y_offset ***
        y_offset = 0;

        // *** height ***
        height = 0;

        // *** width ***
        width = 0;

        // *** do_rectify ***
        do_rectify = false;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x_offset ***
        x_offset = connection.expectInt();

        // *** y_offset ***
        y_offset = connection.expectInt();

        // *** height ***
        height = connection.expectInt();

        // *** width ***
        width = connection.expectInt();

        // *** do_rectify ***
        if (!connection.expectBlock((char*)&do_rectify, 1)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(5)) {
            return false;
        }

        // *** x_offset ***
        x_offset = reader.expectInt();

        // *** y_offset ***
        y_offset = reader.expectInt();

        // *** height ***
        height = reader.expectInt();

        // *** width ***
        width = reader.expectInt();

        // *** do_rectify ***
        do_rectify = reader.expectInt();

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
        // *** x_offset ***
        connection.appendInt(x_offset);

        // *** y_offset ***
        connection.appendInt(y_offset);

        // *** height ***
        connection.appendInt(height);

        // *** width ***
        connection.appendInt(width);

        // *** do_rectify ***
        connection.appendBlock((char*)&do_rectify, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(5);

        // *** x_offset ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)x_offset);

        // *** y_offset ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)y_offset);

        // *** height ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)height);

        // *** width ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)width);

        // *** do_rectify ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)do_rectify);

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
    typedef yarp::os::idl::BareStyle<sensor_msgs_RegionOfInterest> rosStyle;
    typedef yarp::os::idl::BottleStyle<sensor_msgs_RegionOfInterest> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This message is used to specify a region of interest within an image.\n\
#\n\
# When used to specify the ROI setting of the camera when the image was\n\
# taken, the height and width fields should either match the height and\n\
# width fields for the associated image; or height = width = 0\n\
# indicates that the full resolution image was captured.\n\
\n\
uint32 x_offset  # Leftmost pixel of the ROI\n\
                 # (0 if the ROI includes the left edge of the image)\n\
uint32 y_offset  # Topmost pixel of the ROI\n\
                 # (0 if the ROI includes the top edge of the image)\n\
uint32 height    # Height of ROI\n\
uint32 width     # Width of ROI\n\
\n\
# True if a distinct rectified ROI should be calculated from the \"raw\"\n\
# ROI in this message. Typically this should be False if the full image\n\
# is captured (ROI not used), and True if a subwindow is captured (ROI\n\
# used).\n\
bool do_rectify";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/RegionOfInterest", "sensor_msgs/RegionOfInterest");
        typ.addProperty("md5sum", yarp::os::Value("bdb633039d588fcccb441a4d43ccfe09"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

#endif
