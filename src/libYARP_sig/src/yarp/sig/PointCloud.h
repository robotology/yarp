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



namespace yarp {
namespace sig {

template <class T>
/**
 * @brief The PointCloud class.
 */
class PointCloud : public PointCloudBase
{
    static_assert(std::is_same<T, DataXY>::value ||
                  std::is_same<T, DataXYZ>::value ||
                  std::is_same<T, DataNormal>::value ||
                  std::is_same<T, DataXYZRGBA>::value ||
                  std::is_same<T, DataXYZI>::value ||
                  std::is_same<T, DataInterestPointXYZ>::value ||
                  std::is_same<T, DataXYZNormal>::value ||
                  std::is_same<T, DataXYZNormalRGBA>::value, "yarp::sig::PointCloud<T>: T chosen is not supported");
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
    virtual void resize(size_t width, size_t height)
    {
        header.width = width;
        header.height = height;
        m_storage.resize(width * height);
    }

    /**
     * @brief Resize the PointCloud.
     * @note This function resizes the point cloud and has the side effect
     * of reducing the height to 1. Thus, by definition, the point cloud
     * becomes unorganized.
     * @param width.
     */
    virtual void resize(size_t width)
    {
        header.width = width;
        header.height = 1;
        m_storage.resize(width);
    }

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
    inline PointCloud<T>&
    operator+=(const PointCloud<T>& rhs)
    {

        yAssert(getPointType() == rhs.getPointType());

        size_t nr_points = m_storage.size();
        m_storage.resize(nr_points + rhs.size());
        for (size_t i = nr_points; i < m_storage.size(); ++i) {
            m_storage[i] = rhs.m_storage[i - nr_points];
        }

        header.width = m_storage.size();
        header.height = 1;
        if (rhs.isDense() && isDense()) {
            header.isDense = 1;
        } else {
            header.isDense = 0;
        }
        return (*this);
    }

    /**
     * @brief Concatenate a point cloud to another cloud.
     * @param rhs the cloud to add to the current cloud
     * @return the new cloud as a concatenation of the current cloud and the new given cloud
     */
    inline const PointCloud<T>
    operator+(const PointCloud<T>& rhs)
    {
        return (PointCloud<T>(*this) += rhs);
    }

    /**
     * @brief Insert a new point in the cloud, at the end of the container.
     * @note This breaks the organized structure of the cloud by setting the height to 1.
     * @param[in] pt the point to insert.
     */
    inline void push_back(const T& pt)
    {
        m_storage.push_back(pt);
        header.width = m_storage.size();
        header.height = 1;
    }

    /**
     * @brief Clear the data.
     */
    virtual inline void clear()
    {
        m_storage.clear();
        header.width = 0;
        header.height = 0;
    }

    /**
     * @brief Copy the content of an external PointCloud.
     * @param source, pointer to the source data.
     * @param type, enum representing the type of the source cloud.
     * @param width, width of the source cloud.
     * @param height, height of the source cloud.
     * @param isDense
     */
    virtual void fromExternalPC(const char* source, int type, size_t width, size_t height, bool isDense = true)
    {
        yAssert(source);
        header.isDense = isDense;
        resize(width, height);
        if (this->getPointType() == type) {
            memcpy(const_cast<char*>(getRawData()), source, dataSizeBytes());
        } else {
            std::vector<int> recipe = getComposition(type);
            copyFromRawData(getRawData(), source, recipe);
        }
    }


    template <class T1>
    /**
     * Copy operator
     * @brief clones the content of another point cloud
     * @param alt the point cloud to clone
     */
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

    bool read(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::sig::PointCloudNetworkHeader _header;
        bool ok = connection.expectBlock((char*)&_header, sizeof(_header));
        if (!ok) {
            return false;
        }

        m_storage.resize(_header.height * _header.width);
        std::memset((void*)m_storage.data(), 0, m_storage.size() * sizeof(T));

        header.height = _header.height;
        header.width = _header.width;
        header.isDense = _header.isDense;

        if (header.pointType == _header.pointType) {
            return m_storage.read(connection);
        }

        T* tmp = m_storage.data();

        yAssert(tmp != nullptr);

        // Skip the vector header....
        connection.expectInt32();
        connection.expectInt32();

        std::vector<int> recipe = getComposition(_header.pointType);

        yarp::os::ManagedBytes dummy;
        for (size_t i = 0; i < m_storage.size(); i++) {
            for (size_t j = 0; j < recipe.size(); j++) {
                size_t sizeToRead = pointType2Size(recipe[j]);
                if ((header.pointType & recipe[j])) {
                    size_t offset = getOffset(header.pointType, recipe[j]);
                    connection.expectBlock((char*)&tmp[i] + offset, sizeToRead);
                } else {
                    dummy.allocateOnNeed(sizeToRead, sizeToRead);
                    connection.expectBlock(dummy.bytes().get(), sizeToRead);
                }
            }
        }

        connection.convertTextMode();
        return true;
    }

    bool write(yarp::os::ConnectionWriter& writer) const override
    {
        writer.appendBlock((char*)&header, sizeof(PointCloudNetworkHeader));
        return m_storage.write(writer);
    }

    virtual std::string toString(int precision = -1, int width = -1) const
    {
        std::string ret;
        if (isOrganized()) {
            for (size_t r = 0; r < this->width(); r++) {
                for (size_t c = 0; c < this->height(); c++) {
                    ret += (*this)(r, c).toString(precision, width);
                }
                if (r < this->width() - 1) // if it is not the last row
                {
                    ret += "\n";
                }
            }

        } else {
            for (size_t i = 0; i < this->size(); i++) {
                ret += (*this)(i).toString(precision, width);
            }
        }
        return ret;
    }

    /**
     * @brief Generate a yarp::os::Bottle filled with the PointCloud data.
     * @return the yarp::os::Bottle generated
     */
    yarp::os::Bottle toBottle() const
    {
        yarp::os::Bottle ret;
        ret.addInt32(width());
        ret.addInt32(height());
        ret.addInt32(getPointType());
        ret.addInt32(isDense());

        for (size_t i = 0; i < this->size(); i++) {
            ret.addList().append((*this)(i).toBottle());
        }
        return ret;
    }

    /**
     * @brief Populate the PointCloud from a yarp::os::Bottle
     * @param[in] bt, the yarp::os::Bottle to read from. It has to be formatted in
     * the same way it is generated by the toBottle() method.
     * @return true for success, false otherwise
     */

    bool fromBottle(const yarp::os::Bottle& bt)
    {
        if (bt.isNull()) {
            return false;
        }

        if (this->getPointType() != bt.get(2).asInt32()) {
            return false;
        }

        this->resize(bt.get(0).asInt32(), bt.get(1).asInt32());
        this->header.isDense = bt.get(3).asInt32();

        if ((size_t)bt.size() != 4 + width() * height()) {
            return false;
        }

        for (size_t i = 0; i < this->size(); i++) {
            (*this)(i).fromBottle(bt, i + 4);
        }

        return true;
    }

    int getBottleTag() const override
    {
        return BottleTagMap<T>();
    }

private:
    yarp::sig::VectorOf<T> m_storage;

    void setPointType()
    {
        if (std::is_same<T, DataXY>::value) {
            header.pointType = PCL_POINT2D_XY;
            return;
        }

        if (std::is_same<T, DataXYZ>::value) {
            header.pointType = PCL_POINT_XYZ;
            return;
        }

        if (std::is_same<T, DataNormal>::value) {
            header.pointType = PCL_NORMAL;
            return;
        }

        if (std::is_same<T, DataXYZRGBA>::value) {
            header.pointType = PCL_POINT_XYZ_RGBA;
            return;
        }

        if (std::is_same<T, DataXYZI>::value) {
            header.pointType = PCL_POINT_XYZ_I;
            return;
        }

        if (std::is_same<T, DataInterestPointXYZ>::value) {
            header.pointType = PCL_INTEREST_POINT_XYZ;
            return;
        }

        if (std::is_same<T, DataXYZNormal>::value) {
            header.pointType = PCL_POINT_XYZ_NORMAL;
            return;
        }

        if (std::is_same<T, DataXYZNormalRGBA>::value) {
            header.pointType = PCL_POINT_XYZ_NORMAL_RGBA;
            return;
        }

//        DataRGBA       has sense to implement them?
//        intensity       has sense to implement them?
//        DataViewpoint  has sense to implement them?

        header.pointType = 0;
    }
};

using PointCloudXY               = PointCloud<DataXY>;
using PointCloudXYZ              = PointCloud<DataXYZ>;
using PointCloudNormal           = PointCloud<DataNormal>;
using PointCloudXYZRGBA          = PointCloud<DataXYZRGBA>;
using PointCloudXYZI             = PointCloud<DataXYZI>;
using PointCloudInterestPointXYZ = PointCloud<DataInterestPointXYZ>;
using PointCloudXYZNormal        = PointCloud<DataXYZNormal>;
using PointCloudXYZNormalRGBA    = PointCloud<DataXYZNormalRGBA>;

} // namespace sig
} // namespace yarp

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
