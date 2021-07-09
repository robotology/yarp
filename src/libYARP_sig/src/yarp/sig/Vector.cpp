/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Vector.h>

#include <yarp/conf/system.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

#include <yarp/sig/Matrix.h>

#include <vector>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <map>

using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(VECTOR, "yarp.sig.Vector")
}

///////////////////

YARP_BEGIN_PACK
struct VectorPortContentHeader
{
    yarp::os::NetInt32 listTag{0};
    yarp::os::NetInt32 listLen{0};
};
YARP_END_PACK

const std::map<int, std::string> tag2FormatStr = {
    {BOTTLE_TAG_INT32, PRId32},
    {BOTTLE_TAG_INT64, PRId64},
    {BOTTLE_TAG_VOCAB32, "c"},
    {BOTTLE_TAG_STRING, "s"},
    {BOTTLE_TAG_FLOAT64, "lf"},
};

bool VectorBase::read(yarp::os::ConnectionReader& connection) {
    // auto-convert text mode interaction
    connection.convertTextMode();
    VectorPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;
    if (header.listLen > 0 &&
      header.listTag == (BOTTLE_TAG_LIST | getBottleTag()))
    {
        if ((size_t)getListSize() != (size_t)(header.listLen))
            resize(header.listLen);
        char* ptr = getMemoryBlock();
        yCAssert(VECTOR, ptr != nullptr);
        int elemSize=getElementSize();
        ok = connection.expectBlock(ptr, elemSize*header.listLen);
        if (!ok) return false;
    } else {
        return false;
    }

    return !connection.isError();
}

bool VectorBase::write(yarp::os::ConnectionWriter& connection) const {
    VectorPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.listTag = (BOTTLE_TAG_LIST | getBottleTag());
    header.listLen = (int)getListSize();

    connection.appendBlock((char*)&header, sizeof(header));
    const char *ptr = getMemoryBlock();
    int elemSize=getElementSize();
    yCAssert(VECTOR, ptr != nullptr);

    connection.appendExternalBlock(ptr, elemSize*header.listLen);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

std::string VectorBase::getFormatStr(int tag) const
{
    std::string ret;
    auto it = tag2FormatStr.find(tag);

    if (it != tag2FormatStr.end())
    {
        ret = it->second;
    }

    return ret;
}
