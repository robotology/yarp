#ifndef YARP_SIG_POINTCLOUD_NETWORKHEADER_H
#define YARP_SIG_POINTCLOUD_NETWORKHEADER_H

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>

namespace yarp {
    namespace sig {
        class PointCloud_NetworkHeader;
    }
}

// Plain xy point is not supported for now ... does it make sense to have it?
// How to let the user to add his own type?

// Define a bit for each piece of information we want to carry
// Is enum better?? Define some helper to get a string from number
#define   PC_XY_DATA                        1 << 0
#define   PC_XYZ_DATA                       1 << 1
#define   PC_RGBA_DATA                      1 << 2
#define   PC_INTENSITY_DATA                 1 << 3
#define   PC_INTEREST_DATA                  1 << 4   // in PCL this field is also called strenght
#define   PC_NORMAL_DATA                    1 << 5   // contains also a float curvature
#define   PC_RANGE_DATA                     1 << 6
#define   PC_VIEWPOINT_DATA                 1 << 7
#define   PC_MOMENT_INV_DATA                1 << 8
#define   PC_RADII_RSD_DATA                 1 << 9
#define   PC_BOUNDARY_DATA                  1 << 10
#define   PC_PRINCIPAL_CURVATURE_DATA       1 << 11
#define   PC_PFH_SIGNAT_125_DATA            1 << 12
#define   PC_FPFH_SIGNAT_33_DATA            1 << 13
#define   PC_VFH_SIGNAT_308_DATA            1 << 14
#define   PC_NARF_36_DATA                   1 << 15
#define   PC_BORDER_DATA                    1 << 16
#define   PC_INTENSITY_GRAD_DATA            1 << 17
#define   PC_HISTOGRAM_DATA                 1 << 18
#define   PC_SCALE_DATA                     1 << 19
#define   PC_CONFIDENCE_DATA                1 << 20
#define   PC_RADIUS_DATA                    1 << 21
#define   PC_USER_DEFINED                   1 << 22

// Shortcuts names for matching PCL predefined types
#define   PCL_POINT2D_XY                (PC_XY_DATA)
#define   PCL_POINT_XYZ                 (PC_XYZ_DATA)
#define   PCL_POINT_XYZ_RGBA            (PC_XYZ_DATA | PC_RGBA_DATA)
#define   PCL_POINT_XYZ_I               (PC_XYZ_DATA | PC_INTENSITY_DATA)
#define   PCL_INTEREST_POINT_XYZ        (PC_XYZ_DATA | PC_INTEREST_DATA)
#define   PCL_NORMAL                    (PC_NORMAL_DATA)        // add 'POINT' in the name?
#define   PCL_POINT_XYZ_NORMAL          (PC_XYZ_DATA | PC_NORMAL_DATA)
#define   PCL_POINT_XYZ_NORMAL_RGBA     (PC_XYZ_DATA | PC_RGBA_DATA | PC_NORMAL_DATA)   // Actually PCL has PointXYZRGBNormal, not RGBA, but downgrade from rgba to rgb can be done while casting
#define   PCL_POINT_XYZ_I_NORMAL        (PC_XYZ_DATA | PC_INTENSITY_DATA | PC_NORMAL_DATA)
#define   PCL_POINT_XYZ_I_NORMAL        (PC_XYZ_DATA | PC_INTENSITY_DATA | PC_NORMAL_DATA)
#define   PCL_POINT_XYZ_RANGE           (PC_XYZ_DATA | PC_RANGE_DATA)
#define   PCL_POINT_XYZ_VIEWPOINT       (PC_XYZ_DATA | PC_VIEWPOINT_DATA)
#define   PCL_MOMENT_INVARIANTS         (PC_MOMENT_INV_DATA)
#define   PCL_PRINCIPAL_RADII_RSD       (PC_RADII_RSD_DATA)
#define   PCL_BOUNDARY                  (PC_BOUNDARY_DATA)
#define   PCL_PRINCIPAL_CURVATURES      (PC_PRINCIPAL_CURVATURE_DATA)
#define   PCL_PFH_SIGNAT_125            (PC_PFH_SIGNAT_125_DATA)
#define   PCL_FPFH_SIGNAT_33            (PC_FPFH_SIGNAT_33_DATA)
#define   PCL_VFH_SIGNAT_308            (PC_VFH_SIGNAT_308_DATA)
#define   PCL_NARF_36                   (PC_NARF_36_DATA)
#define   PCL_POINT2D_BORDER            (PC_XY_DATA | PC_BORDER_DATA)
#define   PCL_INTENSITY_GRADIENT        (PC_INTENSITY_GRAD_DATA)
#define   PCL_PC_HISTOGRAM_N            (PC_HISTOGRAM_DATA)
#define   PCL_POINT_XYZ_SCALE           (PC_XYZ_DATA | PC_SCALE_DATA)
#define   PCL_POINT_XYZ_SURFEL          (PC_XYZ_DATA | PC_RGBA_DATA | PC_NORMAL_DATA | PC_RADIUS_DATA | PC_CONFIDENCE_DATA)


// Defined as in PCL pointTypes.h file for better compatibility
enum BorderTrait
{
    BORDER_TRAIT__OBSTACLE_BORDER,
    BORDER_TRAIT__SHADOW_BORDER,
    BORDER_TRAIT__VEIL_POINT,
    BORDER_TRAIT__SHADOW_BORDER_TOP,
    BORDER_TRAIT__SHADOW_BORDER_RIGHT,
    BORDER_TRAIT__SHADOW_BORDER_BOTTOM,
    BORDER_TRAIT__SHADOW_BORDER_LEFT,
    BORDER_TRAIT__OBSTACLE_BORDER_TOP,
    BORDER_TRAIT__OBSTACLE_BORDER_RIGHT,
    BORDER_TRAIT__OBSTACLE_BORDER_BOTTOM,
    BORDER_TRAIT__OBSTACLE_BORDER_LEFT,
    BORDER_TRAIT__VEIL_POINT_TOP,
    BORDER_TRAIT__VEIL_POINT_RIGHT,
    BORDER_TRAIT__VEIL_POINT_BOTTOM,
    BORDER_TRAIT__VEIL_POINT_LEFT,
};

/**
 *
 * Byte order in Point Cloud header for network transmission.
 *
 */
// YARP_BEGIN_PACK
class yarp::sig::PointCloud_NetworkHeader
{
public:
    PointCloud_NetworkHeader() :    width(10),
                                    height(1),
                                    isDense(false),
//                                     pose(4, 0.0f),
//                                     orientation(4, 0.0f),
                                    hasCustomData(false)
                                    { }
    // PCL like fields
    yarp::os::NetInt32  width;
    yarp::os::NetInt32  height;
    bool                isDense;         // this guy is mis-aligned // convert to (char?) when serialized
//     yarp::sig::Vector   pose;            // translation from origin -- could be an Eigen::Vector4f for better PCL compatibility if yarp can afford to depend from it
//     yarp::sig::Vector   orientation;     // orientation wrt origin  -- could be an Eigen::Quaternion for better PCL compatibility if yarp can afford to depend from it

    // YARPish fileds
    yarp::os::NetInt32  pointType;       // bitwise of all possible informations -> could also be int64 or just an enum, but I thin bitwise gives more freedom about all possible combinations
//     char                *data;           // actual pointCloud data.

    // Other
    bool                hasCustomData;   // this guy is mis-aligned --> for future use, to have addictional data not initially forseen or fields from ROS which does not fit into yarp pointTypes
//     yarp::os::Bottle    *customData;     // used in case of custom data, to check if a different type can be better optimized for transmission?
};
// YARP_END_PACK



#endif // YARP_SIG_POINTCLOUD_NETWORKHEADER_H
