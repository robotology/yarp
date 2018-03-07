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
#include <yarp/sig/PointCloud.h>

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
inline bool toPCL(yarp::sig::PointCloud< T1 > &yarpCloud, ::pcl::PointCloud< T2 > &pclCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    pclCloud.points.resize(yarpCloud.size());
    pclCloud.width  = yarpCloud.width();
    pclCloud.height = yarpCloud.height();
    memcpy((char*)& pclCloud.points.at(0), yarpCloud.getRawData(), yarpCloud.dataSizeBytes());
    return true;
}

/**
 * Convert a  pcl::PointCloud  to a yarp::sig::PointCloud  object
 * @param pclCloud pcl::PointCloud input
 * @return a yarp cloud filled with data contained in the pcl cloud.
 */
template< class T1, class T2 >
inline bool fromPCL(::pcl::PointCloud< T1 > &pclCloud, yarp::sig::PointCloud< T2 > &yarpCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    yarpCloud.fromExternalPC((char*) &pclCloud(0,0), yarpCloud.getPointType(), pclCloud.width, pclCloud.height, pclCloud.is_dense);
    return true;
}

/**
 * Save a yarp::sig::PointCloud to PCD file, ASCII format
 * @param file_name name of the file to be created wth the cloud
 * @param yarpCloud yarp::sig::PointCloud input
 * @return result of the save operation
 */
template< class T1, class T2 >
inline int savePCD(const std::string &file_name, yarp::sig::PointCloud< T1 > &yarpCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    ::pcl::PointCloud<T2> pclCloud(yarpCloud.width(), yarpCloud.height());
    yarp::pcl::toPCL< T1, T2 >(yarpCloud, pclCloud);
    return ::pcl::io::savePCDFile(file_name, pclCloud);
}

/**
 * Load a yarp::sig::PointCloud from a PCD file, ASCII format
 * @param file_name of the PCD file containing the cloud
 * @param yarpCloud yarp::sig::PointCloud obtained from the PCD file
 * @return result of the load operation
 */
template< class T1, class T2 >
inline int loadPCD(const std::string &file_name, yarp::sig::PointCloud<T2> &yarpCloud)
{
    yAssert(sizeof(T1) == sizeof(T2));
    ::pcl::PointCloud<T1> pclCloud;
    int ret = ::pcl::io::loadPCDFile(file_name, pclCloud);
    yarp::pcl::fromPCL< T1, T2 >(pclCloud, yarpCloud);
    return ret;
}

} // pcl
} // yarp


#endif
