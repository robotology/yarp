/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
 * @param[in] yarpCloud yarp::sig::PointCloud input
 * @param[out] pclCloud pcl::PointCloud filled with data contained in the yarp cloud
 * @return true on success, false otherwise.
 */
template< class T1, class T2 >
inline bool toPCL(const yarp::sig::PointCloud< T1 > &yarpCloud, ::pcl::PointCloud< T2 > &pclCloud)
{
    static_assert(sizeof(T1) == sizeof(T2), "yarp::pcl::toPcl: T1 and T2 are incompatible");
    pclCloud.points.resize(yarpCloud.size());
    pclCloud.width    = yarpCloud.width();
    pclCloud.height   = yarpCloud.height();
    pclCloud.is_dense = yarpCloud.isDense();
    memcpy((char*)& pclCloud.points.at(0), yarpCloud.getRawData(), yarpCloud.dataSizeBytes());
    return true;
}

/**
 * Convert a pcl::PointCloud to a yarp::sig::PointCloud object
 * @param[in] pclCloud pcl::PointCloud input
 * @param[out] yarpCloud yarp cloud filled with data contained in the pcl cloud.
 * @return true on success, false otherwise.
 */
template< class T1, class T2 >
inline bool fromPCL(const ::pcl::PointCloud< T1 > &pclCloud, yarp::sig::PointCloud< T2 > &yarpCloud)
{
    static_assert(sizeof(T1) == sizeof(T2), "yarp::pcl::fromPCL: T1 and T2 are incompatible");
    yarpCloud.fromExternalPC((char*) &pclCloud(0,0), yarpCloud.getPointType(), pclCloud.width, pclCloud.height, pclCloud.is_dense);
    return true;
}

/**
 * Save a yarp::sig::PointCloud to PCD file, ASCII format
 * @param[in] file_name name of the file to be created with the cloud
 * @param[in] yarpCloud yarp::sig::PointCloud input
 * @return result of the save operation(see pcl::io::savePCDFile documentation)
 */
template< class T1, class T2 >
inline int savePCD(const std::string &file_name, const yarp::sig::PointCloud< T1 > &yarpCloud)
{
    static_assert(sizeof(T1) == sizeof(T2), "yarp::pcl::savePCD: T1 and T2 are incompatible");
    ::pcl::PointCloud<T2> pclCloud(yarpCloud.width(), yarpCloud.height());
    yarp::pcl::toPCL< T1, T2 >(yarpCloud, pclCloud);
    return ::pcl::io::savePCDFile(file_name, pclCloud);
}

/**
 * Load a yarp::sig::PointCloud from a PCD file, ASCII format
 * @param[in] file_name of the PCD file containing the cloud
 * @param[out] yarpCloud yarp::sig::PointCloud obtained from the PCD file
 * @return result of the load operation(see pcl::io::loadPCDFile documentation)
 */
template< class T1, class T2 >
inline int loadPCD(const std::string &file_name, yarp::sig::PointCloud<T2> &yarpCloud)
{
    static_assert(sizeof(T1) == sizeof(T2), "yarp::pcl::loadPCD: T1 and T2 are incompatible");
    ::pcl::PointCloud<T1> pclCloud;
    int ret = ::pcl::io::loadPCDFile(file_name, pclCloud);
    yarp::pcl::fromPCL< T1, T2 >(pclCloud, yarpCloud);
    return ret;
}

} // namespace pcl
} // namespace yarp


#endif
