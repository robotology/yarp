/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_IMAGETYPE_H
#define YARP_OVRHEADSET_IMAGETYPE_H


namespace yarp { namespace sig { template <typename T> class PosedImage; }}
namespace yarp { namespace sig { template <typename T> class ImageOf; }}
namespace yarp { namespace sig { struct PixelRgb; }}

//typedef yarp::sig::PosedImage <yarp::sig::ImageOf<yarp::sig::PixelRgb> > ImageType;
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageType;


#endif // YARP_OVRHEADSET_IMAGETYPE_H
