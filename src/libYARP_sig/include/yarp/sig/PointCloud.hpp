
#ifndef YARP_SIG_POINTCLOUD_H
#define YARP_SIG_POINTCLOUD_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/PointCloudTypes.hpp>
#include <yarp/sig/PointCloud_NetworkHeader.hpp>



namespace yarp {
    namespace sig {
        class PointCloud;
        template <class T> class PointCloud2;
        class PointCloud_NetworkHeader;
    }
}

class yarp::sig::PointCloud : public yarp::os::Portable
{
private:
    int pointType;
    VectorOf<int> rgba;
    VectorOf<RGBA_DATA> xyzColor;


public:
    PointCloud();
    virtual ~PointCloud();

    // Portable interface
    virtual bool read(yarp::os::ConnectionReader& reader);
    virtual bool write(yarp::os::ConnectionWriter& writer);
    virtual yarp::os::Type getType();

    // Yarp PointCloud stuff
    virtual bool setPointType(int type);
    virtual bool getPointType(int *type);

private:
    yarp::sig::PointCloud_NetworkHeader    header;

};

template <class T>
class yarp::sig::PointCloud2: public yarp::os::Portable
{
public:
    // Portable interface
    virtual bool read(yarp::os::ConnectionReader& reader)   { return data.read(reader);};
    virtual bool write(yarp::os::ConnectionWriter& writer)  { return data.write(writer);};
    virtual yarp::os::Type getType();

    yarp::sig::VectorOf<T> data;

private:
    yarp::sig::PointCloud_NetworkHeader    header;

};

#endif // YARP_SIG_POINTCLOUD_H
