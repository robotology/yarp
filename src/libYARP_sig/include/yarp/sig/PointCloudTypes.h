/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Willow Garage, Inc.
 * Copyright (C) 2012 Open Perception, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_SIG_POINTCLOUDTYPES_H
#define YARP_SIG_POINTCLOUDTYPES_H

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>

namespace yarp {
    namespace sig {


        // Plain xy point is not supported for now ... does it make sense to have it?
        // How to let the user to add his own type?

        // Define a bit for each piece of information we want to carry
        // Is enum better?? Define some helper to get a string from number
        enum YarpPointCloudBasicTypesEnum {
            PC_XY_DATA                  =      (1 << 0) ,
            PC_XYZ_DATA                 =      (1 << 1) ,
            PC_RGBA_DATA                =      (1 << 2) ,
            PC_INTENSITY_DATA           =      (1 << 3) ,
            PC_INTEREST_DATA            =      (1 << 4) ,  // in PCL this field is also called strenght
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

        enum YarpPointCloudCompositeTypesEnum {
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
        enum YarpBorderTraitEnum
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

        typedef struct
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

        // RGBA fields - quite useless alone
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
        //        float data_c[4];
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
                union
                {
                    struct
                    {
                        float curvature;
                    };
                    float data_c[4];
                };

            };
        } NORMAL_DATA;
        YARP_END_PACK
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
        } NORMAL_NO_CURV;
        YARP_END_PACK
        // curvature
        YARP_BEGIN_PACK
        typedef struct
        {
            union
            {
                struct
                {
                    float curvature;
                };
            };
        } CURVATURE_DATA;
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
        typedef struct
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
        typedef struct
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

    } // sig
} // yarp




#endif // YARP_SIG_POINTCLOUDTYPES_H
