/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_POINTCLOUD_H
#define YARP_SIG_POINTCLOUD_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/PointCloudBase.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

namespace yarp::sig {

template <class T>
/**
 * @brief The PointCloud class.
 */
class YARP_sig_API PointCloud : public PointCloudBase
{
    static_assert(std::is_same<T, DataXY>::value ||
                  std::is_same<T, DataXYZ>::value ||
                  std::is_same<T, DataNormal>::value ||
                  std::is_same<T, DataXYZRGBA>::value ||
                  std::is_same<T, DataXYZI>::value ||
                  std::is_same<T, DataInterestPointXYZ>::value ||
                  std::is_same<T, DataXYZNormal>::value ||
                  std::is_same<T, DataXYZNormalRGBA>::value, "yarp::sig::PointCloud<T>: T chosen is not supported");

private:

    static bool compZ(const T& a, const T& b);

public:

    /**
     * @brief PointCloud, default constructor.
     */
    PointCloud()
    {
        m_storage.clear();
        setPointType();
    }

    /**
     * @brief PointCloud, copy constructor.
     * Clones the content of another point cloud.
     * @param alt the point cloud to clone.
     */
    template <class T1>
    PointCloud(const PointCloud<T1>& alt)
    {
        setPointType();
        copy<T1>(alt);
    }

    /**
     * @brief Resize the PointCloud.
     * @param width.
     * @param height.
     */
    virtual void resize(size_t width, size_t height);

    /**
     * @brief Resize the PointCloud.
     * @note This function resizes the point cloud and has the side effect
     * of reducing the height to 1. Thus, by definition, the point cloud
     * becomes unorganized.
     * @param width.
     */
    virtual void resize(size_t width);

    const char* getRawData() const override
    {
        return m_storage.getMemoryBlock();
    }

    /**
     * @brief Get the size of the data + the header in terms of
     * number of bytes.
     * @return the size of the data sent through the network
     */
    size_t wireSizeBytes() const override
    {
        return sizeof(header) + dataSizeBytes();
    }

    /**
     * @brief Get the size of the data in terms of
     * number of bytes.
     * @return the size of the data
     */
    size_t dataSizeBytes() const override
    {
        return header.width * header.height * (sizeof(T));
    }

    size_t size() const override
    {
        return m_storage.size();
    }

    /**
     * @brief Obtain the point given by the (column, row) coordinates.
     * Only works on organized clouds (those that have height != 1).
     * @param u, column coordinate
     * @param v, row coordinate
     */
    inline T& operator()(size_t u, size_t v)
    {
        return m_storage[u + v * width()];
    }

    /**
     * @brief Obtain the point given by the (column, row) coordinates (const version).
     * Only works on organized clouds (those that have height != 1).
     * @param u, column coordinate
     * @param v, row coordinate
     */
    inline const T& operator()(size_t u, size_t v) const
    {
        return m_storage[u + v * width()];
    }

    /**
     * @brief Obtain the point given by the index.
     * @param i, index
     */
    inline T& operator()(size_t i)
    {
        return m_storage[i];
    }

    /**
     * @brief Obtain the point given by the index (const version).
     * @param i, index
     */
    inline const T& operator()(size_t i) const
    {
        return m_storage[i];
    }

    template <class T1>
    /**
     * Assignment operator.
     * @brief Clones the content of another image.
     * @param alt the image to clone
     */
    const PointCloud<T>& operator=(const PointCloud<T1>& alt)
    {
        copy(alt);
        return *this;
    }

    /**
     * @brief Concatenate a point cloud to the current cloud.
     * @param rhs the cloud to add to the current cloud
     * @return the new cloud as a concatenation of the current cloud and the new given cloud
     */
    PointCloud<T>& operator+=(const PointCloud<T>& rhs);

    /**
     * @brief Concatenate a point cloud to another cloud.
     * @param rhs the cloud to add to the current cloud
     * @return the new cloud as a concatenation of the current cloud and the new given cloud
     */
    const PointCloud<T>  operator+(const PointCloud<T>& rhs);

    /**
     * @brief Insert a new point in the cloud, at the end of the container.
     * @note This breaks the organized structure of the cloud by setting the height to 1.
     * @param[in] pt the point to insert.
     */
    void push_back(const T& pt);

    /**
     * @brief Clear the data.
     */
    virtual void clear();

    /**
     * @brief Copy the content of an external PointCloud.
     * @param source, pointer to the source data.
     * @param type, enum representing the type of the source cloud.
     * @param width, width of the source cloud.
     * @param height, height of the source cloud.
     * @param isDense
     */
    virtual void fromExternalPC(const char* source, int type, size_t width, size_t height, bool isDense = true);

    /**
     * Copy operator
     * @brief clones the content of another point cloud
     * @param alt the point cloud to clone
     */
    template <class T1>
    void copy(const PointCloud<T1>& alt)
    {
        resize(alt.width(), alt.height());
        if (std::is_same<T, T1>::value) {
            yAssert(dataSizeBytes() == alt.dataSizeBytes());
            memcpy(const_cast<char*>(getRawData()), alt.getRawData(), dataSizeBytes());
        } else {
            std::vector<int> recipe = getComposition(alt.getPointType());
            copyFromRawData(getRawData(), alt.getRawData(), recipe);
        }
    }

    bool read(yarp::os::ConnectionReader& connection) override;

    bool write(yarp::os::ConnectionWriter& writer) const override;

    virtual std::string toString(int precision = -1, int width = -1) const;

    /**
     * @brief Generate a yarp::os::Bottle filled with the PointCloud data.
     * @return the yarp::os::Bottle generated
     */
    yarp::os::Bottle toBottle() const;

    /**
     * @brief Populate the PointCloud from a yarp::os::Bottle
     * @param[in] bt, the yarp::os::Bottle to read from. It has to be formatted in
     * the same way it is generated by the toBottle() method.
     * @return true for success, false otherwise
     */
    bool fromBottle(const yarp::os::Bottle& bt);

    int getBottleTag() const override;

    /**
     * @brief Rearranges the pointcloud data so that the points are ordered from the nearest to the farthest.
     * @return true for success, false otherwise
     */
    bool sortDataZ();

    /**
     * @brief Filter out points which are not included in the specified range
     * @param[minZ] minimum distance. Points with z smaller than minZ are removed.
     * @param[minZ] maximum distance. Points with z bugger than maxZ are removed.
     * @return true for success, false otherwise
     */
    bool filterDataZ(double minZ=0, double maxZ=std::numeric_limits<double>::infinity());

private:
    yarp::sig::VectorOf<T> m_storage;

    void setPointType();
};

using PointCloudXY               = PointCloud<DataXY>;
using PointCloudXYZ              = PointCloud<DataXYZ>;
using PointCloudNormal           = PointCloud<DataNormal>;
using PointCloudXYZRGBA          = PointCloud<DataXYZRGBA>;
using PointCloudXYZI             = PointCloud<DataXYZI>;
using PointCloudInterestPointXYZ = PointCloud<DataInterestPointXYZ>;
using PointCloudXYZNormal        = PointCloud<DataXYZNormal>;
using PointCloudXYZNormalRGBA    = PointCloud<DataXYZNormalRGBA>;

//Tests if the point of the pointcloud has the z member
template <typename T, typename = void>
struct has_member_z : std::false_type {};
template <typename T>
struct has_member_z<T, std::void_t<decltype(std::declval<T>().z)>> : std::true_type {};

} // namespace yarp::sig

template <>
inline int BottleTagMap<yarp::sig::DataXY>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataXYZ>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataNormal>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataXYZRGBA>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataXYZI>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataInterestPointXYZ>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataXYZNormal>()
{
    return BOTTLE_TAG_FLOAT64;
}

template <>
inline int BottleTagMap<yarp::sig::DataXYZNormalRGBA>()
{
    return BOTTLE_TAG_FLOAT64;
}


#endif // YARP_SIG_POINTCLOUD_H
