/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/PointCloud.h>

#include <algorithm>
#include <type_traits>
#include <cmath>

namespace yarp::sig {

template <class T>
bool PointCloud<T>::compZ(const T& a, const T& b)
{
    if constexpr (has_member_z<T>::value)
    {
        return (a.z < b.z);
    }
    return false;
}

template <class T>
void PointCloud<T>::resize(size_t width, size_t height)
{
    header.width = width;
    header.height = height;
    m_storage.resize(width * height);
}

template <class T>
void PointCloud<T>::resize(size_t width)
{
    header.width = width;
    header.height = 1;
    m_storage.resize(width);
}

template <class T>
void PointCloud<T>::clear()
{
    m_storage.clear();
    header.width = 0;
    header.height = 0;
}

template <class T>
void PointCloud<T>::fromExternalPC(const char* source, int type, size_t width, size_t height, bool isDense)
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

template <class T>
bool PointCloud<T>::read(yarp::os::ConnectionReader& connection)
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

template <class T>
bool PointCloud<T>::write(yarp::os::ConnectionWriter& writer) const
{
    writer.appendBlock((char*)&header, sizeof(PointCloudNetworkHeader));
    return m_storage.write(writer);
}

template <class T>
std::string PointCloud<T>::toString(int precision, int width) const
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

template <class T>
yarp::os::Bottle PointCloud<T>::toBottle() const
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

template <class T>
bool PointCloud<T>::fromBottle(const yarp::os::Bottle& bt)
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

template <class T>
bool PointCloud<T>::sortDataZ()
{
    if constexpr (has_member_z<T>::value)
    {
        //remove the points whose z value is nan
        m_storage.erase(std::remove_if(m_storage.begin(), m_storage.end(), [](const T& point)
                                       { return std::isnan(point.z); }),
                        m_storage.end());
        //sort the points
        std::sort(m_storage.begin(), m_storage.end(), compZ);
        return true;
    }

    return false;
}

template <class T>
bool PointCloud<T>::filterDataZ(double minZ, double maxZ)
{
    if constexpr (has_member_z<T>::value)
    {
        m_storage.erase(
            std::remove_if(m_storage.begin(), m_storage.end(), [minZ, maxZ](const T& data) {
                return data.z < minZ || data.z > maxZ;
            }),
            m_storage.end()
        );
        return true;
    }

    return false;
}

template <class T>
void PointCloud<T>::setPointType()
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

template <class T>
int PointCloud<T>::getBottleTag() const
{
    return BottleTagMap<T>();
}

template <class T>
PointCloud<T>& PointCloud<T>::operator+=(const PointCloud<T>& rhs)
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

template <class T>
const PointCloud<T> PointCloud<T>::operator+(const PointCloud<T>& rhs)
{
    return (PointCloud<T>(*this) += rhs);
}

template <class T>
void PointCloud<T>::push_back(const T& pt)
{
    m_storage.push_back(pt);
    header.width = m_storage.size();
    header.height = 1;
}

} //namespace yarp::sig

template class yarp::sig::PointCloud<yarp::sig::DataXY>;
template class yarp::sig::PointCloud<yarp::sig::DataXYZ>;
template class yarp::sig::PointCloud<yarp::sig::DataXYZI>;
template class yarp::sig::PointCloud<yarp::sig::DataNormal>;
template class yarp::sig::PointCloud<yarp::sig::DataXYZRGBA>;
template class yarp::sig::PointCloud<yarp::sig::DataXYZNormal>;
template class yarp::sig::PointCloud<yarp::sig::DataXYZNormalRGBA>;
template class yarp::sig::PointCloud<yarp::sig::DataInterestPointXYZ>;
