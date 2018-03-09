// This is an automatically generated file.

// Generated from the following "sensor_msgs/CameraInfo" msg definition:
//   # This message defines meta information for a camera. It should be in a
//   # camera namespace on topic "camera_info" and accompanied by up to five
//   # image topics named:
//   #
//   #   image_raw - raw data from the camera driver, possibly Bayer encoded
//   #   image            - monochrome, distorted
//   #   image_color      - color, distorted
//   #   image_rect       - monochrome, rectified
//   #   image_rect_color - color, rectified
//   #
//   # The image_pipeline contains packages (image_proc, stereo_image_proc)
//   # for producing the four processed image topics from image_raw and
//   # camera_info. The meaning of the camera parameters are described in
//   # detail at http://www.ros.org/wiki/image_pipeline/CameraInfo.
//   #
//   # The image_geometry package provides a user-friendly interface to
//   # common operations using this meta information. If you want to, e.g.,
//   # project a 3d point into image coordinates, we strongly recommend
//   # using image_geometry.
//   #
//   # If the camera is uncalibrated, the matrices D, K, R, P should be left
//   # zeroed out. In particular, clients may assume that K[0] == 0.0
//   # indicates an uncalibrated camera.
//   
//   #######################################################################
//   #                     Image acquisition info                          #
//   #######################################################################
//   
//   # Time of image acquisition, camera coordinate frame ID
//   Header header    # Header timestamp should be acquisition time of image
//                    # Header frame_id should be optical frame of camera
//                    # origin of frame should be optical center of camera
//                    # +x should point to the right in the image
//                    # +y should point down in the image
//                    # +z should point into the plane of the image
//   
//   
//   #######################################################################
//   #                      Calibration Parameters                         #
//   #######################################################################
//   # These are fixed during camera calibration. Their values will be the #
//   # same in all messages until the camera is recalibrated. Note that    #
//   # self-calibrating systems may "recalibrate" frequently.              #
//   #                                                                     #
//   # The internal parameters can be used to warp a raw (distorted) image #
//   # to:                                                                 #
//   #   1. An undistorted image (requires D and K)                        #
//   #   2. A rectified image (requires D, K, R)                           #
//   # The projection matrix P projects 3D points into the rectified image.#
//   #######################################################################
//   
//   # The image dimensions with which the camera was calibrated. Normally
//   # this will be the full camera resolution in pixels.
//   uint32 height
//   uint32 width
//   
//   # The distortion model used. Supported models are listed in
//   # sensor_msgs/distortion_models.h. For most cameras, "plumb_bob" - a
//   # simple model of radial and tangential distortion - is sufficient.
//   string distortion_model
//   
//   # The distortion parameters, size depending on the distortion model.
//   # For "plumb_bob", the 5 parameters are: (k1, k2, t1, t2, k3).
//   float64[] D
//   
//   # Intrinsic camera matrix for the raw (distorted) images.
//   #     [fx  0 cx]
//   # K = [ 0 fy cy]
//   #     [ 0  0  1]
//   # Projects 3D points in the camera coordinate frame to 2D pixel
//   # coordinates using the focal lengths (fx, fy) and principal point
//   # (cx, cy).
//   float64[9]  K # 3x3 row-major matrix
//   
//   # Rectification matrix (stereo cameras only)
//   # A rotation matrix aligning the camera coordinate system to the ideal
//   # stereo image plane so that epipolar lines in both stereo images are
//   # parallel.
//   float64[9]  R # 3x3 row-major matrix
//   
//   # Projection/camera matrix
//   #     [fx'  0  cx' Tx]
//   # P = [ 0  fy' cy' Ty]
//   #     [ 0   0   1   0]
//   # By convention, this matrix specifies the intrinsic (camera) matrix
//   #  of the processed (rectified) image. That is, the left 3x3 portion
//   #  is the normal camera intrinsic matrix for the rectified image.
//   # It projects 3D points in the camera coordinate frame to 2D pixel
//   #  coordinates using the focal lengths (fx', fy') and principal point
//   #  (cx', cy') - these may differ from the values in K.
//   # For monocular cameras, Tx = Ty = 0. Normally, monocular cameras will
//   #  also have R = the identity and P[1:3,1:3] = K.
//   # For a stereo pair, the fourth column [Tx Ty 0]' is related to the
//   #  position of the optical center of the second camera in the first
//   #  camera's frame. We assume Tz = 0 so both cameras are in the same
//   #  stereo image plane. The first camera always has Tx = Ty = 0. For
//   #  the right (second) camera of a horizontal stereo pair, Ty = 0 and
//   #  Tx = -fx' * B, where B is the baseline between the cameras.
//   # Given a 3D point [X Y Z]', the projection (x, y) of the point onto
//   #  the rectified image is given by:
//   #  [u v w]' = P * [X Y Z 1]'
//   #         x = u / w
//   #         y = v / w
//   #  This holds for both images of a stereo pair.
//   float64[12] P # 3x4 row-major matrix
//   
//   
//   #######################################################################
//   #                      Operational Parameters                         #
//   #######################################################################
//   # These define the image region actually captured by the camera       #
//   # driver. Although they affect the geometry of the output image, they #
//   # may be changed freely without recalibrating the camera.             #
//   #######################################################################
//   
//   # Binning refers here to any camera setting which combines rectangular
//   #  neighborhoods of pixels into larger "super-pixels." It reduces the
//   #  resolution of the output image to
//   #  (width / binning_x) x (height / binning_y).
//   # The default values binning_x = binning_y = 0 is considered the same
//   #  as binning_x = binning_y = 1 (no subsampling).
//   uint32 binning_x
//   uint32 binning_y
//   
//   # Region of interest (subwindow of full camera resolution), given in
//   #  full resolution (unbinned) image coordinates. A particular ROI
//   #  always denotes the same window of pixels on the camera sensor,
//   #  regardless of binning settings.
//   # The default setting of roi (all values 0) is considered the same as
//   #  full resolution (roi.width = width, roi.height = height).
//   RegionOfInterest roi
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_CameraInfo_h
#define YARP_ROSMSG_sensor_msgs_CameraInfo_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/sensor_msgs/RegionOfInterest.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class CameraInfo : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::os::NetUint32 height;
    yarp::os::NetUint32 width;
    std::string distortion_model;
    std::vector<yarp::os::NetFloat64> D;
    std::vector<yarp::os::NetFloat64> K;
    std::vector<yarp::os::NetFloat64> R;
    std::vector<yarp::os::NetFloat64> P;
    yarp::os::NetUint32 binning_x;
    yarp::os::NetUint32 binning_y;
    yarp::rosmsg::sensor_msgs::RegionOfInterest roi;

    CameraInfo() :
            header(),
            height(0),
            width(0),
            distortion_model(""),
            D(),
            K(),
            R(),
            P(),
            binning_x(0),
            binning_y(0),
            roi()
    {
        K.resize(9, 0.0);
        R.resize(9, 0.0);
        P.resize(12, 0.0);
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** height ***
        height = 0;

        // *** width ***
        width = 0;

        // *** distortion_model ***
        distortion_model = "";

        // *** D ***
        D.clear();

        // *** K ***
        K.clear();
        K.resize(9, 0.0);

        // *** R ***
        R.clear();
        R.resize(9, 0.0);

        // *** P ***
        P.clear();
        P.resize(12, 0.0);

        // *** binning_x ***
        binning_x = 0;

        // *** binning_y ***
        binning_y = 0;

        // *** roi ***
        roi.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** height ***
        height = connection.expectInt();

        // *** width ***
        width = connection.expectInt();

        // *** distortion_model ***
        int len = connection.expectInt();
        distortion_model.resize(len);
        if (!connection.expectBlock((char*)distortion_model.c_str(), len)) {
            return false;
        }

        // *** D ***
        len = connection.expectInt();
        D.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&D[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** K ***
        len = 9;
        K.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&K[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** R ***
        len = 9;
        R.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&R[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** P ***
        len = 12;
        P.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&P[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** binning_x ***
        binning_x = connection.expectInt();

        // *** binning_y ***
        binning_y = connection.expectInt();

        // *** roi ***
        if (!roi.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(11)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** height ***
        height = reader.expectInt();

        // *** width ***
        width = reader.expectInt();

        // *** distortion_model ***
        if (!reader.readString(distortion_model)) {
            return false;
        }

        // *** D ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        D.resize(len);
        for (int i=0; i<len; i++) {
            D[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** K ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        K.resize(len);
        for (int i=0; i<len; i++) {
            K[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** R ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        R.resize(len);
        for (int i=0; i<len; i++) {
            R[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** P ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        P.resize(len);
        for (int i=0; i<len; i++) {
            P[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** binning_x ***
        binning_x = reader.expectInt();

        // *** binning_y ***
        binning_y = reader.expectInt();

        // *** roi ***
        if (!roi.read(connection)) {
            return false;
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** height ***
        connection.appendInt(height);

        // *** width ***
        connection.appendInt(width);

        // *** distortion_model ***
        connection.appendInt(distortion_model.length());
        connection.appendExternalBlock((char*)distortion_model.c_str(), distortion_model.length());

        // *** D ***
        connection.appendInt(D.size());
        if (D.size()>0) {
            connection.appendExternalBlock((char*)&D[0], sizeof(yarp::os::NetFloat64)*D.size());
        }

        // *** K ***
        if (K.size()>0) {
            connection.appendExternalBlock((char*)&K[0], sizeof(yarp::os::NetFloat64)*K.size());
        }

        // *** R ***
        if (R.size()>0) {
            connection.appendExternalBlock((char*)&R[0], sizeof(yarp::os::NetFloat64)*R.size());
        }

        // *** P ***
        if (P.size()>0) {
            connection.appendExternalBlock((char*)&P[0], sizeof(yarp::os::NetFloat64)*P.size());
        }

        // *** binning_x ***
        connection.appendInt(binning_x);

        // *** binning_y ***
        connection.appendInt(binning_y);

        // *** roi ***
        if (!roi.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(11);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** height ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)height);

        // *** width ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)width);

        // *** distortion_model ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(distortion_model.length());
        connection.appendExternalBlock((char*)distortion_model.c_str(), distortion_model.length());

        // *** D ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(D.size());
        for (size_t i=0; i<D.size(); i++) {
            connection.appendDouble((double)D[i]);
        }

        // *** K ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(K.size());
        for (size_t i=0; i<K.size(); i++) {
            connection.appendDouble((double)K[i]);
        }

        // *** R ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(R.size());
        for (size_t i=0; i<R.size(); i++) {
            connection.appendDouble((double)R[i]);
        }

        // *** P ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(P.size());
        for (size_t i=0; i<P.size(); i++) {
            connection.appendDouble((double)P[i]);
        }

        // *** binning_x ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)binning_x);

        // *** binning_y ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)binning_y);

        // *** roi ***
        if (!roi.write(connection)) {
            return false;
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::CameraInfo> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::CameraInfo> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This message defines meta information for a camera. It should be in a\n\
# camera namespace on topic \"camera_info\" and accompanied by up to five\n\
# image topics named:\n\
#\n\
#   image_raw - raw data from the camera driver, possibly Bayer encoded\n\
#   image            - monochrome, distorted\n\
#   image_color      - color, distorted\n\
#   image_rect       - monochrome, rectified\n\
#   image_rect_color - color, rectified\n\
#\n\
# The image_pipeline contains packages (image_proc, stereo_image_proc)\n\
# for producing the four processed image topics from image_raw and\n\
# camera_info. The meaning of the camera parameters are described in\n\
# detail at http://www.ros.org/wiki/image_pipeline/CameraInfo.\n\
#\n\
# The image_geometry package provides a user-friendly interface to\n\
# common operations using this meta information. If you want to, e.g.,\n\
# project a 3d point into image coordinates, we strongly recommend\n\
# using image_geometry.\n\
#\n\
# If the camera is uncalibrated, the matrices D, K, R, P should be left\n\
# zeroed out. In particular, clients may assume that K[0] == 0.0\n\
# indicates an uncalibrated camera.\n\
\n\
#######################################################################\n\
#                     Image acquisition info                          #\n\
#######################################################################\n\
\n\
# Time of image acquisition, camera coordinate frame ID\n\
Header header    # Header timestamp should be acquisition time of image\n\
                 # Header frame_id should be optical frame of camera\n\
                 # origin of frame should be optical center of camera\n\
                 # +x should point to the right in the image\n\
                 # +y should point down in the image\n\
                 # +z should point into the plane of the image\n\
\n\
\n\
#######################################################################\n\
#                      Calibration Parameters                         #\n\
#######################################################################\n\
# These are fixed during camera calibration. Their values will be the #\n\
# same in all messages until the camera is recalibrated. Note that    #\n\
# self-calibrating systems may \"recalibrate\" frequently.              #\n\
#                                                                     #\n\
# The internal parameters can be used to warp a raw (distorted) image #\n\
# to:                                                                 #\n\
#   1. An undistorted image (requires D and K)                        #\n\
#   2. A rectified image (requires D, K, R)                           #\n\
# The projection matrix P projects 3D points into the rectified image.#\n\
#######################################################################\n\
\n\
# The image dimensions with which the camera was calibrated. Normally\n\
# this will be the full camera resolution in pixels.\n\
uint32 height\n\
uint32 width\n\
\n\
# The distortion model used. Supported models are listed in\n\
# sensor_msgs/distortion_models.h. For most cameras, \"plumb_bob\" - a\n\
# simple model of radial and tangential distortion - is sufficient.\n\
string distortion_model\n\
\n\
# The distortion parameters, size depending on the distortion model.\n\
# For \"plumb_bob\", the 5 parameters are: (k1, k2, t1, t2, k3).\n\
float64[] D\n\
\n\
# Intrinsic camera matrix for the raw (distorted) images.\n\
#     [fx  0 cx]\n\
# K = [ 0 fy cy]\n\
#     [ 0  0  1]\n\
# Projects 3D points in the camera coordinate frame to 2D pixel\n\
# coordinates using the focal lengths (fx, fy) and principal point\n\
# (cx, cy).\n\
float64[9]  K # 3x3 row-major matrix\n\
\n\
# Rectification matrix (stereo cameras only)\n\
# A rotation matrix aligning the camera coordinate system to the ideal\n\
# stereo image plane so that epipolar lines in both stereo images are\n\
# parallel.\n\
float64[9]  R # 3x3 row-major matrix\n\
\n\
# Projection/camera matrix\n\
#     [fx'  0  cx' Tx]\n\
# P = [ 0  fy' cy' Ty]\n\
#     [ 0   0   1   0]\n\
# By convention, this matrix specifies the intrinsic (camera) matrix\n\
#  of the processed (rectified) image. That is, the left 3x3 portion\n\
#  is the normal camera intrinsic matrix for the rectified image.\n\
# It projects 3D points in the camera coordinate frame to 2D pixel\n\
#  coordinates using the focal lengths (fx', fy') and principal point\n\
#  (cx', cy') - these may differ from the values in K.\n\
# For monocular cameras, Tx = Ty = 0. Normally, monocular cameras will\n\
#  also have R = the identity and P[1:3,1:3] = K.\n\
# For a stereo pair, the fourth column [Tx Ty 0]' is related to the\n\
#  position of the optical center of the second camera in the first\n\
#  camera's frame. We assume Tz = 0 so both cameras are in the same\n\
#  stereo image plane. The first camera always has Tx = Ty = 0. For\n\
#  the right (second) camera of a horizontal stereo pair, Ty = 0 and\n\
#  Tx = -fx' * B, where B is the baseline between the cameras.\n\
# Given a 3D point [X Y Z]', the projection (x, y) of the point onto\n\
#  the rectified image is given by:\n\
#  [u v w]' = P * [X Y Z 1]'\n\
#         x = u / w\n\
#         y = v / w\n\
#  This holds for both images of a stereo pair.\n\
float64[12] P # 3x4 row-major matrix\n\
\n\
\n\
#######################################################################\n\
#                      Operational Parameters                         #\n\
#######################################################################\n\
# These define the image region actually captured by the camera       #\n\
# driver. Although they affect the geometry of the output image, they #\n\
# may be changed freely without recalibrating the camera.             #\n\
#######################################################################\n\
\n\
# Binning refers here to any camera setting which combines rectangular\n\
#  neighborhoods of pixels into larger \"super-pixels.\" It reduces the\n\
#  resolution of the output image to\n\
#  (width / binning_x) x (height / binning_y).\n\
# The default values binning_x = binning_y = 0 is considered the same\n\
#  as binning_x = binning_y = 1 (no subsampling).\n\
uint32 binning_x\n\
uint32 binning_y\n\
\n\
# Region of interest (subwindow of full camera resolution), given in\n\
#  full resolution (unbinned) image coordinates. A particular ROI\n\
#  always denotes the same window of pixels on the camera sensor,\n\
#  regardless of binning settings.\n\
# The default setting of roi (all values 0) is considered the same as\n\
#  full resolution (roi.width = width, roi.height = height).\n\
RegionOfInterest roi\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: sensor_msgs/RegionOfInterest\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/CameraInfo", "sensor_msgs/CameraInfo");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_CameraInfo_h
