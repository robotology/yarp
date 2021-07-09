/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Willow Garage Inc.
 * SPDX-FileCopyrightText: 2012 Open Perception Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_SIG_POINTCLOUDTYPES_H
#define YARP_SIG_POINTCLOUDTYPES_H

#include <yarp/conf/system.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetInt32.h>
#include <yarp/sig/Vector.h>

namespace yarp {
namespace sig {


// Plain xy point is not supported for now ... does it make sense to have it?
// How to let the user to add his own type?

// Define a bit for each piece of information we want to carry
// Is enum better?? Define some helper to get a string from number
/**
 * @brief The PointCloudBasicTypes enum
 */
enum PointCloudBasicType : std::int32_t {
    PC_XY_DATA                  =      (1 << 0) ,
    PC_XYZ_DATA                 =      (1 << 1) ,
    PC_RGBA_DATA                =      (1 << 2) ,
    PC_INTENSITY_DATA           =      (1 << 3) ,
    PC_INTEREST_DATA            =      (1 << 4) ,  // in PCL this field is also called strength
    PC_NORMAL_DATA              =      (1 << 5) ,
    PC_CURVATURE_DATA           =      (1 << 6) ,
    PC_RANGE_DATA               =      (1 << 7) ,
    PC_VIEWPOINT_DATA           =      (1 << 8) ,
    PC_MOMENT_INV_DATA          =      (1 << 9) ,
    PC_RADII_RSD_DATA           =      (1 << 10),
    PC_BOUNDARY_DATA            =      (1 << 11),
    PC_PRINCIPAL_CURVATURE_DATA =      (1 << 12),
    PC_PFH_SIGNAT_125_DATA      =      (1 << 13),
    PC_FPFH_SIGNAT_33_DATA      =      (1 << 14),
    PC_VFH_SIGNAT_308_DATA      =      (1 << 15),
    PC_NARF_36_DATA             =      (1 << 16),
    PC_BORDER_DATA              =      (1 << 17),
    PC_INTENSITY_GRAD_DATA      =      (1 << 18),
    PC_HISTOGRAM_DATA           =      (1 << 19),
    PC_SCALE_DATA               =      (1 << 20),
    PC_CONFIDENCE_DATA          =      (1 << 21),
    PC_RADIUS_DATA              =      (1 << 22),
    PC_USER_DEFINED             =      (1 << 23),
    PC_PADDING2                 =      (1 << 24),
    PC_PADDING3                 =      (1 << 25)
};
/**
 * @brief The PointCloudCompositeType enum
 */
enum PointCloudCompositeType : std::int32_t {
// Shortcuts names for matching PCL predefined types
    PCL_POINT2D_XY             =   (PC_XY_DATA),
    PCL_POINT_XYZ              =   (PC_XYZ_DATA),
    PCL_POINT_XYZ_RGBA         =   (PC_XYZ_DATA | PC_RGBA_DATA | PC_PADDING3),
    PCL_POINT_XYZ_I            =   (PC_XYZ_DATA | PC_INTENSITY_DATA),
    PCL_INTEREST_POINT_XYZ     =   (PC_XYZ_DATA | PC_INTEREST_DATA),
    PCL_NORMAL                 =   (PC_NORMAL_DATA | PC_CURVATURE_DATA | PC_PADDING3),
    PCL_POINT_XYZ_NORMAL       =   (PC_XYZ_DATA | PC_NORMAL_DATA | PC_CURVATURE_DATA | PC_PADDING3),
    PCL_POINT_XYZ_NORMAL_RGBA  =   (PC_XYZ_DATA | PC_RGBA_DATA | PC_NORMAL_DATA | PC_CURVATURE_DATA | PC_PADDING2),   // Actually PCL has PointXYZRGBNormal, not RGBA, but downgrade from rgba to rgb can be done while casting
    PCL_POINT_XYZ_I_NORMAL     =   (PC_XYZ_DATA | PC_INTENSITY_DATA | PC_NORMAL_DATA | PC_CURVATURE_DATA),
    PCL_POINT_XYZ_RANGE        =   (PC_XYZ_DATA | PC_RANGE_DATA),
    PCL_POINT_XYZ_VIEWPOINT    =   (PC_XYZ_DATA | PC_VIEWPOINT_DATA),
    PCL_MOMENT_INVARIANTS      =   (PC_MOMENT_INV_DATA),
    PCL_PRINCIPAL_RADII_RSD    =   (PC_RADII_RSD_DATA),
    PCL_BOUNDARY               =   (PC_BOUNDARY_DATA),
    PCL_PRINCIPAL_CURVATURES   =   (PC_PRINCIPAL_CURVATURE_DATA),
    PCL_PFH_SIGNAT_125         =   (PC_PFH_SIGNAT_125_DATA),
    PCL_FPFH_SIGNAT_33         =   (PC_FPFH_SIGNAT_33_DATA),
    PCL_VFH_SIGNAT_308         =   (PC_VFH_SIGNAT_308_DATA),
    PCL_NARF_36                =   (PC_NARF_36_DATA),
    PCL_POINT2D_BORDER         =   (PC_XY_DATA | PC_BORDER_DATA),
    PCL_INTENSITY_GRADIENT     =   (PC_INTENSITY_GRAD_DATA),
    PCL_PC_HISTOGRAM_N         =   (PC_HISTOGRAM_DATA),
    PCL_POINT_XYZ_SCALE        =   (PC_XYZ_DATA | PC_SCALE_DATA),
    PCL_POINT_XYZ_SURFEL       =   (PC_XYZ_DATA | PC_RGBA_DATA | PC_NORMAL_DATA | PC_RADIUS_DATA | PC_CONFIDENCE_DATA)
};

// Defined as in PCL pointTypes.h file for better compatibility
enum PointCloudBorderTrait : std::int32_t
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

// Definition of single fields data structures
YARP_BEGIN_PACK
struct DataXY
{
    union
    {
        float _xy[2];
        struct
        {
            float x;
            float y;
        };
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf\t", precision, x,
                                                  precision, y);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf", width, precision, x,
                                                  width, precision, y);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();
        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        return;
    }
};
YARP_END_PACK

// xyz
YARP_BEGIN_PACK
struct DataXYZ
{
    union
    {
        float _xyz[4];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf\t", precision, x,
                                                         precision, y,
                                                         precision, z);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf", width, precision, x,
                                                          width, precision, y,
                                                          width, precision, z);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        return ret;
    }
    yarp::sig::Vector toVector3() const
    {
        yarp::sig::Vector v(3);
        v[0] = x;
        v[1] = y;
        v[2] = z;
        return v;
    }
    yarp::sig::Vector toVector4() const
    {
        yarp::sig::Vector v(4);
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = 1;
        return v;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        return;
    }
};
YARP_END_PACK

// RGBA fields - quite useless alone
YARP_BEGIN_PACK
struct DataRGBA
{
    union
    {
        struct
        {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char a;
        };
        yarp::os::NetInt32 rgba;
//        float data_c[4];
    };
    std::string toString(int precision, int width) const
    {
        YARP_UNUSED(precision);
        YARP_UNUSED(width);
        std::string ret = "";
        char tmp[128];
        snprintf(tmp, 128, "%d %d %d %d\t", r, g, b, a);
        ret += tmp;
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addInt32(r);
        ret.addInt32(g);
        ret.addInt32(b);
        ret.addInt32(a);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();
        r = intBt->get(0).asInt32();
        g = intBt->get(1).asInt32();
        b = intBt->get(2).asInt32();
        a = intBt->get(3).asInt32();
        return;
    }
};
YARP_END_PACK

// Normal
YARP_BEGIN_PACK
struct DataNormal
{
    union
    {
        float filler_n[4];
        float normal[3];
        struct
        {
            float normal_x;
            float normal_y;
            float normal_z;
        };
    };
    union
    {
        struct
        {
            float curvature;
        };
        float data_c[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf % .*lf\t", precision, normal_x,
                                                                precision, normal_y,
                                                                precision, normal_z,
                                                                precision, curvature);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf % *.*lf", width, precision, normal_x,
                                                                  width, precision, normal_y,
                                                                  width, precision, normal_z,
                                                                  width, precision, curvature);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(normal_x);
        ret.addFloat64(normal_y);
        ret.addFloat64(normal_z);
        ret.addFloat64(curvature);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        normal_x = static_cast<float>(intBt->get(0).asFloat64());
        normal_y = static_cast<float>(intBt->get(1).asFloat64());
        normal_z = static_cast<float>(intBt->get(2).asFloat64());
        curvature = static_cast<float>(intBt->get(3).asFloat64());
        return;
    }
};
YARP_END_PACK

YARP_BEGIN_PACK
struct DataNormalNoCurvature
{
    union
    {
        float filler_n[4];
        float normal[3];
        struct
        {
            float normal_x;
            float normal_y;
            float normal_z;
        };
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf\t", precision, normal_x,
                                                         precision, normal_y,
                                                         precision, normal_z);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf", width, precision, normal_x,
                                                          width, precision, normal_y,
                                                          width, precision, normal_z);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(normal_x);
        ret.addFloat64(normal_y);
        ret.addFloat64(normal_z);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        normal_x = static_cast<float>(intBt->get(0).asFloat64());
        normal_y = static_cast<float>(intBt->get(1).asFloat64());
        normal_z = static_cast<float>(intBt->get(2).asFloat64());
        return;
    }
};
YARP_END_PACK

// curvature
YARP_BEGIN_PACK
struct DataCurvature
{
    union
    {
        struct
        {
            float curvature;
        };
    };
};
YARP_END_PACK
// Range
typedef float Range;

// viewPoint
YARP_BEGIN_PACK
struct DataViewpoint
{
    union
    {
        float _xyz[4];
        struct
        {
            float vp_x;
            float vp_y;
            float vp_z;
        };
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf\t", precision, vp_x,
                                                         precision, vp_y,
                                                         precision, vp_z);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf", width, precision, vp_x,
                                                          width, precision, vp_y,
                                                          width, precision, vp_z);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(vp_x);
        ret.addFloat64(vp_y);
        ret.addFloat64(vp_z);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        vp_x = static_cast<float>(intBt->get(0).asFloat64());
        vp_y = static_cast<float>(intBt->get(1).asFloat64());
        vp_z = static_cast<float>(intBt->get(2).asFloat64());
        return;
    }
};
YARP_END_PACK

// TBD: many others ...


//
// Definition of packed types - PCL style
//

// xyz + rgba - most common type
YARP_BEGIN_PACK
struct DataXYZRGBA
{
    union
    {
        float _xyz[4];
        struct
        {
            float x;
            float y;
            float z;
            float xyz_padding;
        };
    };

    union
    {
        struct
        {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char a;
        };
        yarp::os::NetInt32 rgba;
        float rgba_padding[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf ", precision, x,
                                                        precision, y,
                                                        precision, z);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf ", width, precision, x,
                                                           width, precision, y,
                                                           width, precision, z);
            ret += tmp;
        }
        snprintf(tmp, 128, "%d %d %d %d\t", r, g, b, a);
        ret += tmp;
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        ret.addInt32(r);
        ret.addInt32(g);
        ret.addInt32(b);
        ret.addInt32(a);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        r = intBt->get(3).asInt32();
        g = intBt->get(4).asInt32();
        b = intBt->get(5).asInt32();
        a = intBt->get(6).asInt32();
        return;
    }
};
YARP_END_PACK

// xyz + intensity
YARP_BEGIN_PACK
struct DataXYZI
{
    union
    {
        float _xyz[4];
        struct
        {
            float x;
            float y;
            float z;
        };
    };

    union
    {
        struct
        {
            float intensity;
        };
        float intensity_padding[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf % .*lf\t", precision, x,
                                                                precision, y,
                                                                precision, z,
                                                                precision, intensity);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf % *.*lf", width, precision, x,
                                                                  width, precision, y,
                                                                  width, precision, z,
                                                                  width, precision, intensity);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        ret.addFloat64(intensity);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        intensity = static_cast<float>(intBt->get(3).asFloat64());
        return;
    }
};
YARP_END_PACK

// interest point -> xyz + strength
YARP_BEGIN_PACK
struct DataInterestPointXYZ
{
    union
    {
        float _xyz[4];
        struct
        {
            float x;
            float y;
            float z;
        };
    };

    union
    {
        struct
        {
            float strength;
        };
        float strength_padding[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf % .*lf\t", precision, x,
                                                                precision, y,
                                                                precision, z,
                                                                precision, strength);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf % *.*lf", width, precision, x,
                                                                  width, precision, y,
                                                                  width, precision, z,
                                                                  width, precision, strength);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        ret.addFloat64(strength);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        strength = static_cast<float>(intBt->get(3).asFloat64());
        return;
    }
};
YARP_END_PACK


// point xyz + normals
YARP_BEGIN_PACK
struct DataXYZNormal
{
    union
    {
        float data[4];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
    union
    {
        float filler_n[4];
        float normal[3];
        struct
        {
            float normal_x;
            float normal_y;
            float normal_z;
        };
    };
    union
    {
        struct
        {
            float curvature;
        };
        float filler_c[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf ", precision, x,
                                                        precision, y,
                                                        precision, z);
            ret += tmp;
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf % .*lf\t", precision, normal_x,
                                                                precision, normal_y,
                                                                precision, normal_z,
                                                                precision, curvature);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf ", width, precision, x,
                                                           width, precision, y,
                                                           width, precision, z);
            ret += tmp;
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf % *.*lf", width, precision, normal_x,
                                                                  width, precision, normal_y,
                                                                  width, precision, normal_z,
                                                                  width, precision, curvature);
            ret += tmp;
        }
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        ret.addFloat64(normal_x);
        ret.addFloat64(normal_y);
        ret.addFloat64(normal_z);
        ret.addFloat64(curvature);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        normal_x = static_cast<float>(intBt->get(3).asFloat64());
        normal_y = static_cast<float>(intBt->get(4).asFloat64());
        normal_z = static_cast<float>(intBt->get(5).asFloat64());
        curvature = static_cast<float>(intBt->get(6).asFloat64());
        return;
    }
};
YARP_END_PACK

// point xyz + normals + RGBA
YARP_BEGIN_PACK
struct DataXYZNormalRGBA
{
    union
    {
        float data[4];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
    union
    {
        float filler_n[4];
        float normal[3];
        struct
        {
            float normal_x;
            float normal_y;
            float normal_z;
        };
    };
    union
    {
        struct
        {
            // PCL here uses float rgb, probably for ROS compatibility as stated for PointXYZRGB
            // Check compatibility, it should be ok if rgb component are in the right place and we drop 'a'
            union
            {
                struct
                {
                    unsigned char b;
                    unsigned char g;
                    unsigned char r;
                    unsigned char a;
                };
                yarp::os::NetInt32 rgba;
            };
            float curvature;
        };
        float filler_others[4];
    };
    std::string toString(int precision, int width) const
    {
        std::string ret = "";
        char tmp[128];
        if (width < 0) {
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf ", precision, x,
                                                        precision, y,
                                                        precision, z);
            ret += tmp;
            snprintf(tmp, 128, "% .*lf % .*lf % .*lf % .*lf ", precision, normal_x,
                                                               precision, normal_y,
                                                               precision, normal_z,
                                                               precision, curvature);
            ret += tmp;

        } else {
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf ", width, precision, x,
                                                           width, precision, y,
                                                           width, precision, z);
            ret += tmp;
            snprintf(tmp, 128, "% *.*lf % *.*lf % *.*lf % *.*lf ", width, precision, normal_x,
                                                                   width, precision, normal_y,
                                                                   width, precision, normal_z,
                                                                   width, precision, curvature);
            ret += tmp;
        }
        snprintf(tmp, 128, "%d %d %d %d\t", r, g, b, a);
        ret += tmp;
        return ret;
    }
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addFloat64(x);
        ret.addFloat64(y);
        ret.addFloat64(z);
        ret.addFloat64(normal_x);
        ret.addFloat64(normal_y);
        ret.addFloat64(normal_z);
        ret.addFloat64(curvature);
        ret.addInt32(r);
        ret.addInt32(g);
        ret.addInt32(b);
        ret.addInt32(a);
        return ret;
    }
    void fromBottle(const yarp::os::Bottle& bt, size_t i)
    {
        yarp::os::Bottle* intBt = bt.get(static_cast<int>(i)).asList();

        if (!intBt) {
            return;
        }

        x = static_cast<float>(intBt->get(0).asFloat64());
        y = static_cast<float>(intBt->get(1).asFloat64());
        z = static_cast<float>(intBt->get(2).asFloat64());
        normal_x = static_cast<float>(intBt->get(3).asFloat64());
        normal_y = static_cast<float>(intBt->get(4).asFloat64());
        normal_z = static_cast<float>(intBt->get(5).asFloat64());
        curvature = static_cast<float>(intBt->get(6).asFloat64());
        r = intBt->get(7).asInt32();
        g = intBt->get(8).asInt32();
        b = intBt->get(9).asInt32();
        a = intBt->get(10).asInt32();
        return;
    }
};
YARP_END_PACK

// TBD: many others ...

} // namespace sig
} // namespace yarp


#endif // YARP_SIG_POINTCLOUDTYPES_H
