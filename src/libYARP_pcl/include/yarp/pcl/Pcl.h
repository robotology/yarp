/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_PCL_PCL_H
#define YARP_PCL_PCL_H

#include <pcl/io/pcd_io.h>
#include <yarp/sig/PointCloud.hpp>

namespace yarp
{
namespace pcl
{

/**
 * Convert a yarp::sig::PointCloud to a pcl::PointCloud object
 * @param yarpCloud yarp::sig::PointCloud input
 * @return a pcl PointCloud filled with data contained in the yarp cloud.
 */
template< class T1, class T2 >
inline ::pcl::PointCloud<T1> toPCL(yarp::sig::PointCloud< T2 > &yarpCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    ::pcl::PointCloud<T1> pclCloud(yarpCloud.width(), yarpCloud.height());
//    pclCloud.resize(yarpCloud.width(), yarpCloud.height());
    memcpy((char*)& pclCloud.points.at(0), yarpCloud.getRawData(), yarpCloud.dataSizeBytes());
    return pclCloud;
}

/**
 * Convert a  pcl::PointCloud  to a yarp::sig::PointCloud  object
 * @param pclCloud pcl::PointCloud input
 * @return a yarp cloud filled with data contained in the pcl cloud.
 */
template< class T1, class T2 >
inline yarp::sig::PointCloud<T1> fromPCL(::pcl::PointCloud< T2 > &pclCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    yarp::sig::PointCloud<T1> yarpCloud;
    yarpCloud.fromExternalPC((char*) &pclCloud(0,0), yarpCloud.getPointType(), pclCloud.width, pclCloud.height, pclCloud.is_dense);
    return yarpCloud;
}


} // pcl
} // yarp


#endif
