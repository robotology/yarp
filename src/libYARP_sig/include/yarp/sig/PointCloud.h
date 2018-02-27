/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_POINTCLOUD_H
#define YARP_SIG_POINTCLOUD_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/PointCloudNetworkHeader.h>

#include <map>
#include <vector>


#include <cstring>


namespace yarp {
    namespace sig {
        class FlexPointCloud;
        template <class T> class PointCloud;
        class PointCloudBase;
    }
}

// TBD: Copy constructor (needed by compilers such as Intel C++)

namespace yarp {
    namespace sig {

    class YARP_sig_API PointCloudBase :public yarp::os::Portable
    {
    public:
        virtual void resize(size_t width, size_t height);

        virtual size_t wireSizeBytes() const = 0;

        virtual size_t dataSizeBytes() const = 0;

        virtual size_t size() const = 0;

        virtual const char* getRawData() const = 0;

        virtual bool read(yarp::os::ConnectionReader& connection) override = 0;

        virtual bool write(yarp::os::ConnectionWriter& writer) override = 0;

        virtual size_t height() const;

        virtual size_t width() const;

        virtual int getPointType() const;

        virtual yarp::os::Type getType() override ;

        virtual void fromExternalPC(const char* source, int type, size_t width, size_t height, bool isDense = true);

        virtual bool isOrganized();


        virtual inline void clear()
        {
            header.width = 0;
            header.height = 0;
        }

        virtual inline bool isDense() const
        {
            return header.isDense;
        }

    protected:

        PointCloudBase();

        virtual void copyFromRawData(const char* dst, const char* source, std::vector<int> &recipe);

        virtual std::vector<int> getComposition(int type_composite) const;

        virtual size_t pointType2Size(int type) const;

        virtual size_t getOffset(int type_composite, int type_basic) const;

        yarp::sig::PointCloudNetworkHeader    header;

    };

    template <class T>
    class PointCloud: public PointCloudBase
    {
    public:

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

        // Usage stuff
        virtual void resize(size_t width, size_t height) override
        {
            header.width = width;
            header.height = height;
            data.resize(width * height);
        }

        virtual const char* getRawData() const override
        {
            return data.getMemoryBlock();
        }

        virtual size_t wireSizeBytes() const override
        {
            return sizeof(header) + dataSizeBytes();
        }

        virtual size_t dataSizeBytes() const override
        {
            return header.width*header.height*(sizeof(T));
        }

        virtual size_t size() const override
        {
            return data.size();
        }

        inline T& operator()(size_t u, size_t v) {
            yAssert(isOrganized());
            if (u > width() || v > height())
            {
                return nulldata;
            }
            return data[u + v*width()];
        }

        inline T& operator()(size_t i) {
            if (i > data.size())
            {
                return nulldata;
            }
            return data[i];
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


        /** \brief Concatenate a point cloud to the current cloud.
          * \param[in] rhs the cloud to add to the current cloud
          * \return the new cloud as a concatenation of the current cloud and the new given cloud
          */
        inline PointCloud<T>&
        operator += (const PointCloud<T>& rhs)
        {
            //TODO: take the newest timestamp

            yAssert(getPointType() == rhs.getPointType());

            size_t nr_points = data.size();
            data.resize (nr_points + rhs.size());
            for (size_t i = nr_points; i < data.size (); ++i)
            {
                data[i] = rhs.data[i - nr_points];
            }

            header.width = data.size();
            header.height   = 1;
            if (rhs.isDense() && isDense())
            {
                header.isDense = true;
            }
            else
            {
                header.isDense = false;
            }
            return (*this);
        }

        /** \brief Concatenate a point cloud to another cloud.
          * \param[in] rhs the cloud to add to the current cloud
          * \return the new cloud as a concatenation of the current cloud and the new given cloud
          */
        inline const PointCloud<T>
        operator + (const PointCloud<T>& rhs)
        {
          return (PointCloud<T> (*this) += rhs);
        }

        inline void push_back (const T& pt)
           {
             data.push_back(pt);
             header.width = data.size();
             header.height = 1;
           }

        virtual inline void clear() override
        {
            data.clear();
            header.width = 0;
            header.height = 0;
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
                memcpy(const_cast<char*> (getRawData()), alt.getRawData(), dataSizeBytes());
            }
            else
            {
                std::vector<int> recipe = getComposition(alt.getPointType());
                copyFromRawData(getRawData(), alt.getRawData(), recipe);
            }

        }

        virtual bool read(yarp::os::ConnectionReader& connection) override
        {
            connection.convertTextMode();
            yarp::sig::PointCloudNetworkHeader _header;
            bool ok = connection.expectBlock((char*)&_header, sizeof(_header));
            if (!ok) return false;

            data.resize(_header.height * _header.width);
            std::memset((void *) data.getFirst(), 0, data.size() * sizeof(T));

            header.height = _header.height;
            header.width = _header.width;
            header.isDense = _header.isDense;

            if (header.pointType == _header.pointType) //Working
            {
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
                    size_t sizeToRead = pointType2Size(recipe[j]);
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

        virtual bool write(yarp::os::ConnectionWriter& writer) override
        {
            writer.appendBlock((char*)&header, sizeof(PointCloudNetworkHeader));
            return data.write(writer);
        }

        virtual yarp::os::ConstString toString(int precision=-1, int width=-1)
        {
            return "not implemented, sorry man :/";
        }

     private:
        yarp::sig::VectorOf<T> data;
        T nulldata;

        // TODO to be tested with getRawData

        void setPointType()
        {
            if (std::is_same<T, XY_DATA>::value)
            {
                header.pointType = PCL_POINT2D_XY;
                return;
            }

            if (std::is_same<T, XYZ_DATA>::value)
            {
                header.pointType = PCL_POINT_XYZ;
                return;
            }

            if (std::is_same<T, NORMAL_DATA>::value)
            {
                header.pointType = PCL_NORMAL;
                return;
            }

            if (std::is_same<T, XYZ_RGBA_DATA>::value)
            {
                header.pointType = PCL_POINT_XYZ_RGBA;
                return;
            }

            if (std::is_same<T, XYZ_I_DATA>::value)
            {
                header.pointType = PCL_POINT_XYZ_I;
                return;
            }

            if (std::is_same<T, INTEREST_POINT_XYZ_DATA>::value)
            {
                header.pointType = PCL_INTEREST_POINT_XYZ;
                return;
            }

            if (std::is_same<T, XYZ_NORMAL_DATA>::value)
            {
                header.pointType = PCL_POINT_XYZ_NORMAL;
                return;
            }

            if (std::is_same<T, XYZ_NORMAL_RGBA_DATA>::value)
            {
                header.pointType = PCL_POINT_XYZ_NORMAL_RGBA;
                return;
            }

    //        RGBA_DATA       has sense to implement them?
    //        intensity       has sense to implement them?
    //        VIEWPOINT_DATA  has sense to implement them?

            header.pointType = 0;

            return;
        }

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
    //
    // private:
    //  allocate pointCloudData dynamically;
    // };

    } // sig


} // yarp


#define TagMap(X) \
template<> \
inline int BottleTagMap <X> () \
{ \
    return BOTTLE_TAG_DOUBLE; \
}

TagMap(yarp::sig::XY_DATA)
TagMap(yarp::sig::XYZ_DATA)
TagMap(yarp::sig::NORMAL_DATA)
TagMap(yarp::sig::XYZ_RGBA_DATA)
TagMap(yarp::sig::XYZ_I_DATA)
TagMap(yarp::sig::INTEREST_POINT_XYZ_DATA)
TagMap(yarp::sig::XYZ_NORMAL_DATA)
TagMap(yarp::sig::XYZ_NORMAL_RGBA_DATA)



// TODO implement toString incrementally



#endif // YARP_SIG_POINTCLOUD_H
