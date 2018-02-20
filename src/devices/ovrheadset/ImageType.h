/*
 * Copyright (C) 2015-2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */


#ifndef YARP_OVRHEADSET_IMAGETYPE_H
#define YARP_OVRHEADSET_IMAGETYPE_H

namespace yarp { namespace sig { template <typename T> class ImageOf; }}
namespace yarp { namespace sig { struct PixelRgb; }}
typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> ImageType;

#endif // YARP_OVRHEADSET_IMAGETYPE_H
