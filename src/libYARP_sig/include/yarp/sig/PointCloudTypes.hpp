/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_SIG_POINTCLOUDTYPES_H
#define YARP_SIG_POINTCLOUDTYPES_H

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/PointCloud.hpp>

namespace yarp {
    namespace sig {
        class PointCloud_NetworkHeader;
    }
}

// Definition of single fields data structures
YARP_BEGIN_PACK
typedef struct
{
    float x;
    float y;
} XY_DATA;
YARP_END_PACK

// xyz
YARP_BEGIN_PACK
typedef struct
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
} XYZ_DATA;
YARP_END_PACK

// RGBA fiels - quite useless alone
YARP_BEGIN_PACK
typedef struct
{
    union
    {
        struct
        {
            char b;
            char g;
            char r;
            char a;
        };
        yarp::os::NetInt32 rgba;
        float data_c[4];
    };
} RGBA_DATA;
YARP_END_PACK

// Intensity
typedef float intensity;

// Intensity
typedef float strength;

// Normal
YARP_BEGIN_PACK
typedef struct
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
} NORMAL_DATA;
YARP_END_PACK

// Range
typedef float range;

// viewPoint
YARP_BEGIN_PACK
typedef struct
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
} VIEWPOINT_DATA;
YARP_END_PACK

// TBD: many others ...


//
// Definition of packed types - PCL style
//

// xyz + rgba - most common type
YARP_BEGIN_PACK
typedef struct
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
            char b;
            char g;
            char r;
            char a;
        };
        yarp::os::NetInt32 rgba;
        float rgba_padding[4];
    };
} XYZ_RGBA_DATA;
YARP_END_PACK

// xyz + intensity
YARP_BEGIN_PACK
typedef struct
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
} XYZ_I_DATA;
YARP_END_PACK

// interest point -> xyz + strength
YARP_BEGIN_PACK
typedef struct
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
} INTEREST_POINT_XYZ_DATA;
YARP_END_PACK


// point xyz + normals
YARP_BEGIN_PACK
typedef union
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
} XYZ_NORMAL_DATA;
YARP_END_PACK

// point xyz + normals + RGBA
YARP_BEGIN_PACK
typedef union
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
                    char b;
                    char g;
                    char r;
                    char a;
                };
                yarp::os::NetInt32 rgba;
            };
            float curvature;
        };
        float filler_others[4];
    };
} XYZ_NORMAL_RGBA_DATA;
YARP_END_PACK

// TBD: many others ...



#endif // YARP_SIG_POINTCLOUDTYPES_H
