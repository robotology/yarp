
#ifndef YARP_SIG_POINTCLOUD_H
#define YARP_SIG_POINTCLOUD_H

#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/PointCloud_NetworkHeader.hpp>
#include <cstring>



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
    }

    const char* getRawData()
    {
        return data.getMemoryBlock();
    }

    int wireSizeBytes()
    {
        return sizeof(header) + header.width*header.height*(sizeof(XYZ_RGBA_DATA::_xyz)+sizeof(XYZ_RGBA_DATA::rgba));
    }

    int dataSizeBytes() const
    {
        return /*sizeof(header) +*/ header.width*header.height*(sizeof(T));
    int height() const
    {
        return header.height;
    }

    int width() const
    {
        return header.width;
    }

    int getPointType() const
    {
        return header.pointType;

    }


    // Portable interface

    PointCloud()
    {
        data.clear();
        setPointType();
    }

    /**
     * Copy constructor.
     * Clones the content of another point cloud.
     * @param alt the point cloud to clone
     */
    template<class T1>
    PointCloud(const PointCloud<T1>& alt)
    {
        setPointType();
        copy<T1>(alt);
        return;

    }

    template<class T1>
    /**
     * Copy operator
     * @brief clones the content of another point cloud
     * @param alt the point cloud to clone
     */
    void copy(const PointCloud<T1> &alt)
    {
        resize(alt.width(),alt.height());
        if (std::is_same<T, T1>::value)
        {
            yAssert(dataSizeBytes()==alt.dataSizeBytes());
            memcpy(const_cast<char*> (data.getMemoryBlock()), alt.data.getMemoryBlock(), dataSizeBytes());
        }
        else
        {
            std::vector<int> recipe = getComposition(alt.getPointType());
            copyFromRawData(data.getMemoryBlock(), alt.data.getMemoryBlock(), recipe);
        }

    }

    template<class T1>
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
    virtual bool read(yarp::os::ConnectionReader& connection)
    {
        yTrace();
        connection.convertTextMode();
        yarp::sig::PointCloud_NetworkHeader _header;
        bool ok = connection.expectBlock((char*)&_header, sizeof(_header));
        if (!ok) return false;

        data.resize(_header.height * _header.width);
        std::memset((void *) data.getFirst(), 0, data.size() * sizeof(T));

        header.height = _header.height;
        header.width = _header.width;
        header.isDense = _header.isDense;

        if (header.pointType == _header.pointType) //Working
        {
            yInfo("IS MATCHIIIIING BITCHESSSS\n");
            return data.read(connection);
        }

        T *tmp = data.getFirst();

        yAssert(tmp != nullptr);

        // Skip the vector header....
        connection.expectInt(); // Code auto-generated do not remove
        connection.expectInt(); // Code auto-generated do not remove

        std::vector<int> recipe = getComposition(_header.pointType);

        yarp::os::ManagedBytes dummy;
        for (uint i=0; i<data.size(); i++)
        {
            for (size_t j = 0; j<recipe.size(); j++)
            {
                size_t sizeToRead = sizeMap.find(recipe[j])->second;
                if ((header.pointType & recipe[j]))
                {
                    size_t offset = getOffset(header.pointType, recipe[j]);
                    connection.expectBlock((char*) &tmp[i]+offset, sizeToRead);
                }
                else
                {
                    dummy.allocateOnNeed(sizeToRead, sizeToRead);
                    connection.expectBlock(dummy.bytes().get(), sizeToRead);
                }
            }
        }

        connection.convertTextMode();
        return true;
    }

    virtual bool write(yarp::os::ConnectionWriter& writer)
    {
        writer.appendBlock((char*)&header, sizeof(header));
        return data.write(writer);
    }
    virtual yarp::os::Type getType()                        { return yarp::os::Type::byName("yarp/pointCloud"); }

    virtual yarp::os::ConstString toString(int precision=-1, int width=-1)
    {
        yTrace();
        //yarp::os::ConstString("ciaoooo");
//        return PointCloud< T >::toString(precision, width);
        return "not implemented, sorry man :/ \n";
    }


    // Internal conversions
    template<class X1> bool convertTo(yarp::sig::PointCloud<X1> &out);

    yarp::sig::VectorOf<T> data;

 private:
    yarp::sig::PointCloud_NetworkHeader    header;

    std::vector<int> getComposition(int type_composite)
    {
        //todo probably
        std::vector<int> ret;
        auto it = compositionMap.find(type_composite);
        if (it != compositionMap.end())
        {
            ret = it->second;
        }
        return ret;
    }
    size_t getOffset(int type_composite, int type_basic)
    {
        size_t offset = 0;
        auto it = offsetMap.find(std::make_pair(type_composite, type_basic));
        if (it != offsetMap.end())
        {
            offset = it->second;
        }
        return offset;

    }
    void copyFromRawData(const char* dst, const char* source, std::vector<int> &recipe)
    {
        char* tmpSrc = const_cast<char*> (source);
        char* tmpDst = const_cast<char*> (dst);
        if (recipe.empty())
        {
            return;
        }
        yAssert(tmpSrc && tmpDst);


        size_t sizeDst = sizeMap.find(getPointType())->second;
        const size_t numPts  = height()*width();
        for (uint i=0; i < numPts; i++)
        {
            for (size_t j = 0; j < recipe.size(); j++)
            {
                size_t sizeToRead = sizeMap.find(recipe[j])->second;
                if ((header.pointType & recipe[j]))
                {
                    size_t offset = getOffset(header.pointType, recipe[j]);
                    std::memcpy(tmpDst + i*sizeDst + offset, tmpSrc, sizeToRead);
                }

                // increment anyways, if the field is missing in the destination, simply skip it
                tmpSrc += sizeToRead;
            }
        }
    }

    void setPointType();
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
    template<> void PointCloud<XY_DATA>::setPointType()                       { header.pointType=PCL_POINT2D_XY; }
    template<> void PointCloud<XYZ_DATA>::setPointType()                      { header.pointType=PCL_POINT_XYZ; }
//    template<> void PointCloud<RGBA_DATA>::setPointType()                    { header.pointType=PC_RGBA_DATA; } //has it sense to support them?
//    template<> void PointCloud<intensity>::setPointType()                    { header.pointType=PC_INTENSITY_DATA; } //has it sense to support them?
//    template<> void PointCloud<VIEWPOINT_DATA>::setPointType()               { header.pointType=PC_VIEWPOINT_DATA; } //has it sense to support them?
    template<> void PointCloud<NORMAL_DATA>::setPointType()                   { header.pointType=PCL_NORMAL; }
    template<> void PointCloud<XYZ_RGBA_DATA>::setPointType()                 { header.pointType=PCL_POINT_XYZ_RGBA; }
    template<> void PointCloud<XYZ_I_DATA>::setPointType()                    { header.pointType=PCL_POINT_XYZ_I; }
    template<> void PointCloud<INTEREST_POINT_XYZ_DATA>::setPointType()       { header.pointType=PCL_INTEREST_POINT_XYZ; }
    template<> void PointCloud<XYZ_NORMAL_DATA>::setPointType()               { header.pointType=PCL_POINT_XYZ_NORMAL; }
    template<> void PointCloud<XYZ_NORMAL_RGBA_DATA>::setPointType()          { header.pointType=PCL_POINT_XYZ_NORMAL_RGBA; }
    // TODO extend the constructors...

        template<> yarp::os::ConstString PointCloud <XYZ_RGBA_DATA> ::toString(int precision, int width);
        template<> yarp::os::ConstString PointCloud <XYZ_DATA> ::toString(int precision, int width);
    }
}

#define TagMap(X) \
template<> \
inline int BottleTagMap <X> () \
{ \
    return BOTTLE_TAG_DOUBLE; \
}

TagMap(XY_DATA)
TagMap(XYZ_DATA)
TagMap(NORMAL_DATA)
TagMap(XYZ_RGBA_DATA)
TagMap(XYZ_I_DATA)
TagMap(INTEREST_POINT_XYZ_DATA)
TagMap(XYZ_NORMAL_DATA)


// TODO implement toString incrementally


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
