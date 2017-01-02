/*
* Author: Marco Randazzo
* Copyright (C) 2017 iCubFacility - Istituto Italiano di Tecnologia
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/math/Vec2D.h>
#include <yarp/math/Math.h>
#include <math.h>
#include <cstdio>

/// network stuff
#include <yarp/os/NetInt32.h>

using namespace yarp::math;

YARP_BEGIN_PACK
class Vec2DPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
};
YARP_END_PACK

namespace yarp
{
    namespace math 
    {
        template<>
        bool Vec2D<double>::read(yarp::os::ConnectionReader& connection)
        {
            // auto-convert text mode interaction
            connection.convertTextMode();
            Vec2DPortContentHeader header;
            bool ok = connection.expectBlock((char*)&header, sizeof(header));
            if (!ok) return false;

            if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE))
            {
                this->x = connection.expectDouble();
                this->y = connection.expectDouble();
            }
            else
            {
                return false;
            }

            return !connection.isError();
        }

        template<>
        bool Vec2D<int>::read(yarp::os::ConnectionReader& connection)
        {
            // auto-convert text mode interaction
            connection.convertTextMode();
            Vec2DPortContentHeader header;
            bool ok = connection.expectBlock((char*)&header, sizeof(header));
            if (!ok) return false;

            if (header.listLen == 2 && header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_INT))
            {
                this->x = connection.expectInt();
                this->y = connection.expectInt();
            }
            else
            {
                return false;
            }

            return !connection.isError();
        }

        template<>
        bool Vec2D<double>::write(yarp::os::ConnectionWriter& connection)
        {
            Vec2DPortContentHeader header;

            header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE);
            header.listLen = 2;

            connection.appendBlock((char*)&header, sizeof(header));

            connection.appendDouble(this->x);
            connection.appendDouble(this->y);

            connection.convertTextMode();

            return !connection.isError();
        }

        template<>
        bool Vec2D<int>::write(yarp::os::ConnectionWriter& connection)
        {
            Vec2DPortContentHeader header;

            header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_INT);
            header.listLen = 2;

            connection.appendBlock((char*)&header, sizeof(header));

            connection.appendInt(this->x);
            connection.appendInt(this->y);

            connection.convertTextMode();

            return !connection.isError();
        }
    }
}
