/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetType.h>

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/LogComponent.h>

#include <clocale>
#include <cstdlib>
#include <cstring>
#include <limits>




using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(NETTYPE, "yarp.os.NetType")
} // namespace

int NetType::netInt(const yarp::os::Bytes& code)
{
    yCAssert(NETTYPE, code.length() == sizeof(NetInt32));
    NetInt32 tmp;
    memcpy((char*)(&tmp), code.get(), code.length());
    return tmp;
}

bool NetType::netInt(int data, yarp::os::Bytes& code)
{
    NetInt32 i = data;
    yarp::os::Bytes b((char*)(&i), sizeof(i));
    if (code.length() != sizeof(i)) {
        yCError(NETTYPE, "not enough room for integer");
        return false;
    }
    memcpy(code.get(), b.get(), code.length());
    return true;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
std::string NetType::toHexString(int x)
{
    return yarp::conf::numeric::to_hex_string(x);
}

std::string NetType::toHexString(long x)
{
    return yarp::conf::numeric::to_hex_string(x);
}

std::string NetType::toHexString(unsigned int x)
{
    return yarp::conf::numeric::to_hex_string(x);
}

std::string NetType::toString(int x)
{
    return yarp::conf::numeric::to_string(x);
}

std::string NetType::toString(long x)
{
    return yarp::conf::numeric::to_string(x);
}

std::string NetType::toString(unsigned int x)
{
    return yarp::conf::numeric::to_string(x);
}


int NetType::toInt(const std::string& x)
{
    return yarp::conf::numeric::from_string<int>(x);
}


std::string NetType::toString(yarp::conf::float32_t x)
{
    return yarp::conf::numeric::to_string(x);
}

std::string NetType::toString(yarp::conf::float64_t x)
{
    return yarp::conf::numeric::to_string(x);
}

yarp::conf::float32_t NetType::toFloat32(const std::string& s)
{
    return yarp::conf::numeric::from_string<yarp::conf::float32_t>(s);
}

yarp::conf::float64_t NetType::toFloat64(const std::string& s)
{
    return yarp::conf::numeric::from_string<yarp::conf::float64_t>(s);
}

yarp::conf::float32_t NetType::toFloat32(std::string&& s)
{
    return yarp::conf::numeric::from_string<yarp::conf::float32_t>(s);
}

yarp::conf::float64_t NetType::toFloat64(std::string&& s)
{
    return yarp::conf::numeric::from_string<yarp::conf::float64_t>(s);
}
#endif

/*
  PNG's nice and simple CRC code
  (from http://www.w3.org/TR/PNG-CRCAppendix.html)
*/

/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
static int crc_table_computed = 0;

/* Make the table for a fast CRC. */
static void make_crc_table()
{
    unsigned long c;
    int n;
    int k;

    for (n = 0; n < 256; n++) {
        c = (unsigned long)n;
        for (k = 0; k < 8; k++) {
            if ((c & 1) != 0) {
                c = 0xedb88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */

static unsigned long update_crc(unsigned long crc, unsigned char* buf, size_t len)
{

    unsigned long c = crc;
    size_t n;

    if (crc_table_computed == 0) {
        make_crc_table();
    }
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long NetType::getCrc(char* buf, size_t len)
{
    return update_crc(0xffffffffL, (unsigned char*)buf, len) ^ 0xffffffffL;
}
