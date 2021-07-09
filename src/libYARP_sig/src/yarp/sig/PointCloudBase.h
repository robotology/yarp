/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_POINTCLOUDBASE_H
#define YARP_SIG_POINTCLOUDBASE_H

#include <yarp/sig/PointCloudNetworkHeader.h>

#include <cstring>
#include <map>
#include <vector>

namespace yarp {
namespace sig {

/**
 * @brief The PointCloudBase class
 *
 * A Base class for a yarp::sig::PointCloud<T>, provide default implementation for
 * some methods and hides some internal implementation to the user.
 * It can't be instantiated, only inherited.
 */
class YARP_sig_API PointCloudBase : public yarp::os::Portable
{
public:
    virtual ~PointCloudBase() = default;

    /**
     * @brief Get the size of the data + the header in terms of
     * number of bytes.
     * @return the size of the data sent through the network
     */
    virtual size_t wireSizeBytes() const = 0;

    /**
     * @brief Get the size of the data in terms of
     * number of bytes.
     * @return the size of the data
     */
    virtual size_t dataSizeBytes() const = 0;

    /**
     * @brief
     * @return the number of points of the PointCloud
     */
    virtual size_t size() const = 0;

    /**
     * @brief Get the pointer to the data.
     * @return the pointer to the data.
     */
    virtual const char* getRawData() const = 0;

    bool read(yarp::os::ConnectionReader& connection) override = 0;

    bool write(yarp::os::ConnectionWriter& writer) const override = 0;

    virtual int getBottleTag() const = 0;

    /**
     * @return the height specified in the yarp::sig::PointCloudNetworkHeader.
     */
    virtual size_t height() const;

    /**
     * @return the width specified int the yarp::sig::PointCloudNetworkHeader.
     */
    virtual size_t width() const;

    /**
     * @return the enum representing the point type.
     */
    virtual int getPointType() const;

    yarp::os::Type getType() const override;

    /**
     * @return true if the point cloud is organized in an image-like structure
     */
    virtual bool isOrganized() const;

    /**
     * @return true if the point cloud doesn't contain NaN or Inf values
     */
    virtual inline bool isDense() const
    {
        return header.isDense != 0;
    }

protected:
    PointCloudBase() = default;

    virtual void copyFromRawData(const char* dst, const char* source, std::vector<int>& recipe);

    virtual std::vector<int> getComposition(int type_composite) const;

    virtual size_t pointType2Size(int type) const;

    virtual size_t getOffset(int type_composite, int type_basic) const;

    yarp::sig::PointCloudNetworkHeader header;
};


} // namespace sig
} // namespace yarp

#endif
