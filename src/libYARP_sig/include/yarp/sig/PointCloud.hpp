
#ifndef YARP_SIG_POINTCLOUD_H
#define YARP_SIG_POINTCLOUD_H

#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/PointCloudTypes.hpp>
#include <yarp/sig/PointCloud_NetworkHeader.hpp>



namespace yarp {
    namespace sig {
        class FlexPointCloud;
        class PointCloud_NetworkHeader;
        template <class T> class PointCloud;
    }
}

// TBD: Copy constructor (needed by compilers such as Intel C++)


template <class T>
class yarp::sig::PointCloud: public yarp::os::Portable
{
public:
    // Usage stuff
    virtual void resize(int width, int height)
    {
        header.width = width;
        header.height = height;
        data.resize(width * height);
    };

    int wireSizeBytes()
    {
        return sizeof(header) + header.width*header.height*(sizeof(XYZ_RGBA_DATA::_xyz)+sizeof(XYZ_RGBA_DATA::rgba));
    }

    int dataSizeBytes() const
    {
        return /*sizeof(header) +*/ header.width*header.height*(sizeof(T));
    }

    // Portable interface
    PointCloud<T>();
    virtual bool read(yarp::os::ConnectionReader& reader)
    {
        yTrace();
        data.read(reader);
        return true;
    };

    virtual bool write(yarp::os::ConnectionWriter& writer)
    {
        writer.appendBlock((char*)&header, sizeof(header));
        return data.write(writer);
    };
    virtual yarp::os::Type getType()                        { return yarp::os::Type::byName("yarp/pointCloud"); };

    virtual yarp::os::ConstString toString(int precision=-1, int width=-1)
    {
        yTrace();
        //yarp::os::ConstString("ciaoooo");
        return PointCloud< T >::toString(precision, width);
    };



    // Internal conversions
    template<class X1> bool convertTo(yarp::sig::PointCloud<X1> &out);

    yarp::sig::VectorOf<T> data;

// private:
    yarp::sig::PointCloud_NetworkHeader    header;
};


// Maybe do a Flex type in the same way as Flex Image / ImageOf is doing?
// template <class T>
// class yarp::sig::Flex: public yarp::sig::PointCloud
// {
// public:
//
//     virtual FlexPointCloud()  {};
//     // Portable interface
// //     virtual bool read(yarp::os::ConnectionReader& reader)   { return data.read(reader);};
// //     virtual bool write(yarp::os::ConnectionWriter& writer)  { return data.write(writer);};
// //     virtual yarp::os::Type getType()                        { return yarp::os::Type::byName("yarp/pointCloud"); };
//     virtual yarp::os::ConstString toString()                { yTrace(); return yarp::os::ConstString("ciaoooo");};
//
//     virtual void *convertFromType(int newType) { return NULL;};
//     virtual void *convertToType(int newType) { return NULL;};
//
// private:
//  allocate pointCloudData dynamically;
// };



namespace yarp {
    namespace sig {
        template<> yarp::sig::PointCloud<XYZ_RGBA_DATA>::PointCloud()   { PointCloud::header.pointType=PCL_POINT_XYZ_RGBA; };
        template<> yarp::sig::PointCloud<XYZ_DATA>::PointCloud()        { PointCloud::header.pointType=PCL_POINT_XYZ; };

        template<> yarp::os::ConstString yarp::sig::PointCloud <XYZ_RGBA_DATA> ::toString(int precision, int width);
        template<> yarp::os::ConstString yarp::sig::PointCloud <XYZ_DATA> ::toString(int precision, int width);

        template<> bool VectorOf <XYZ_RGBA_DATA> ::read(yarp::os::ConnectionReader& connection);
        template<> bool VectorOf <XYZ_RGBA_DATA> ::write(yarp::os::ConnectionWriter& connection);

        template<> bool VectorOf <XYZ_DATA> ::read(yarp::os::ConnectionReader& connection);
        template<> bool VectorOf <XYZ_DATA> ::write(yarp::os::ConnectionWriter& connection);
    }
}


template<>
inline int BottleTagMap <XYZ_RGBA_DATA> ()
{
    return BOTTLE_TAG_DOUBLE;
}


template<>
bool yarp::sig::VectorOf <XYZ_RGBA_DATA> ::read(yarp::os::ConnectionReader& connection)
{
    yTrace();
    connection.convertTextMode();
    PointCloud_NetworkHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;

    if (header.width > 0 && header.height > 0)    // if there are data
    {
        len = header.width * header.height;
        resize(len);

        XYZ_RGBA_DATA *tmp = getFirst();


        // Verify what to do depending on source type
        switch(header.pointType)
        {
            case PCL_POINT_XYZ_RGBA:    // types do match, easy case.
            {
                yInfo() << "Types match";
                for(uint i=0; i<len; i++)
                {
                    // Copy data stripping out padding bytes ( remove unused memory to optimize size for transmission over network)
                    // --> if both sender and receiver are on the same machine, can I leverage on IPC to just copy stuff
                    ok = connection.expectBlock((char*) &tmp[i]._xyz, sizeof(XYZ_RGBA_DATA::_xyz));
                    ok &= connection.expectBlock((char*) &tmp[i].rgba, sizeof(XYZ_RGBA_DATA::rgba));
                    if (!ok)
                        return false;
//                     yDebug() << "x: " << tmp[i].x << "y: " << tmp[i].y << "z: " << tmp[i].z;
//                     yDebug() << "r: " << (u_int8_t)tmp[i].r << "g: " << (u_int8_t)tmp[i].g << "b: " << (u_int8_t)tmp[i].b << "\n";
                }
                break;
            }

            case PCL_POINT_XYZ:         // types does not match exactly, but conversion is possible.
            {
                yInfo() << "Types does not match, but conversion is possible - NOT YET IMPLEMENTED";
                break;
            }

            case PCL_NORMAL:
            case PCL_BOUNDARY:
//             case bla bla bla (put here all types supported which are not convertible
            {
                yError() << "Requested type cannot be read from received message. Source type is " << header.pointType << \
                " while client is trying to read PCL_POINT_XYZ_RGBA (" << PCL_POINT_XYZ_RGBA << ")";
                // read the memory, also if type does not match, to clean the buffer for next read... really needed?
                // Vector isn't doing this. skip at the beginning and do some tests to verify
//                 const char *ptr = getMemoryBlock();
//                 yAssert(ptr != 0);
//                 ok = connection.expectBlock(ptr, sizeof(len));
//                 if (!ok) return false;
                break;
            }

            case PC_USER_DEFINED:
            {
                yError() << "Source type is a custom user defined one, unknown conversion to requested type PCL_POINT_XYZ_RGBA";
                // read the memory, also if type does not match, to clean the buffer for next read... really needed?
                // Vector isn't doing this. skip at the beginning and do some tests to verify
//                 const char *ptr = getMemoryBlock();
//                 yAssert(ptr != 0);
//                 ok = connection.expectBlock(ptr, sizeof(len));
//                 if (!ok) return false;
                break;
            }
            default:                    // types do not match and conversion is not possible
            {
                yError() << "Requested type cannot be read from received message. Source type is " << header.pointType << \
                " while client is trying to read PCL_POINT_XYZ_RGBA (" << PCL_POINT_XYZ_RGBA << ")";
                break;
            }
        }

    }
    else
    {
        yError() << "Header not valid";
        return false;
    }

    return !connection.isError();
}

template<>
bool yarp::sig::VectorOf <XYZ_RGBA_DATA> ::write(yarp::os::ConnectionWriter& connection)
{
    yTrace();
    // Header is already set by the generic PointCloud
    const char *ptr = getMemoryBlock();
//     int elemSize=getElementSize();
    yAssert(ptr != NULL);

    XYZ_RGBA_DATA *tmp = getFirst();
    for(uint i=0; i<len; i++)
    {
        // Copy data stripping out padding bytes ( remove unused memory to optimize size for transmission over network)
        // --> if both sender and receiver are on the same machine, can I leverage on IPC to just copy stuff
        connection.appendExternalBlock((char*) &tmp[i]._xyz, sizeof(XYZ_RGBA_DATA::_xyz));
        connection.appendExternalBlock((char*) &tmp[i].rgba, sizeof(XYZ_RGBA_DATA::rgba));

//         yDebug() << "x: " << tmp[i].x << "y: " << tmp[i].y << "z: " << tmp[i].z;
//         yDebug() << "r: " << (u_int8_t)tmp[i].r << "g: " << (u_int8_t)tmp[i].g << "b: " << (u_int8_t)tmp[i].b << "\n";
    }
    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}


template<>
bool yarp::sig::VectorOf <XYZ_DATA> ::read(yarp::os::ConnectionReader& connection)
{
    yTrace();
    return true;
}

template<>
bool yarp::sig::VectorOf <XYZ_DATA> ::write(yarp::os::ConnectionWriter& connection)
{
    yTrace();
//     VectorPortContentHeader header;  // maybe not needed, since the PC will have an header on his own
//    header.listTag = (BOTTLE_TAG_LIST | getBottleTag());
//    header.listLen = (int)getListSize();
//    connection.appendBlock((char*)&header, sizeof(header));
    const char *ptr = getMemoryBlock();
    int elemSize=getElementSize();
    yAssert(ptr != NULL);

    connection.appendExternalBlock(ptr, elemSize*getListSize());

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

namespace yarp{
    namespace sig {

    template<> yarp::os::ConstString yarp::sig::PointCloud <XYZ_RGBA_DATA> ::toString(int precision, int width)
    {
        yTrace();
        yarp::os::ConstString out;
        const size_t pointsNum = this->header.width * this->header.height;

        yInfo() << "memSize is " << dataSizeBytes() << "; data on wire are " << wireSizeBytes();
        char tmp[350];
        if(width<0)
        {
            for(size_t i=0; i<pointsNum; i++)
            {
                snprintf(tmp, 350, "% .*lf\t% .*lf\t% .*lf\n", precision, data[i].x, precision, data[i].y, precision, data[i].z);
                out+=tmp;
                snprintf(tmp, 350, "%d\t%u\t%u\t%u\n", (u_int8_t)data[i].r, (u_int8_t)data[i].g, (u_int8_t)data[i].b, (u_int8_t)data[i].a);
                out+=tmp;
            }
        }
        else
        {
            for(size_t i=0; i<pointsNum; i++)
            {
                sprintf(tmp, "% *.*lf ", width, precision, data[i].x);
                out+=tmp;
            }
        }

        if(pointsNum >= 1)
            return out.substr(0, out.length()-1);
        return out;
    }

    template<> yarp::os::ConstString yarp::sig::PointCloud <XYZ_DATA> ::toString(int precision, int width)
    {
        yTrace();
        return yarp::os::ConstString("XYZ_DATA");
    }

//     template<> bool yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo( int ciao);
//     template<> template<class T2> bool yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo(yarp::sig::PointCloud<T2> *out);
//     template<class X1> bool  yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo(yarp::sig::PointCloud<X1>);

    template<>
    template<class XYZ_DATA> bool  yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo(yarp::sig::PointCloud<XYZ_DATA> &out)
    {
        yTrace() << "\n\tConverting from XYZ_RGBA_DATA to XYZ_DATA";
        return true;
    }

}
}



// template<>
// bool yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo( int ciao)
// {
//     yTrace();
//     return true;
// }

// template<> template<>
// bool yarp::sig::PointCloud<XYZ_RGBA_DATA>::convertTo(yarp::sig::PointCloud<XYZ_DATA> *out)
// {
//     yTrace();
//     return true;
// }


#endif // YARP_SIG_POINTCLOUD_H
