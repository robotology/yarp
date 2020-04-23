/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetType.h>

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>

#include <clocale>
#include <cstdlib>
#include <cstring>
#include <limits>


/*
 * The maximum string length for a 'double' printed as a string using ("%.*g", DECIMAL_DIG) will be:
 *  Initial +/- sign                        1 char
 *  First digit for exponential notation    1 char
 * '.' decimal separator char               1 char
 *  DECIMAL_DIG digits for the mantissa     DECIMAL_DIG chars
 * 'e+/-'                                   2 chars
 * YARP_DBL_EXP_DIG  for the exponential    YARP_DBL_EXP_DIG chars
 * string terminator                        1 char
 * FILLER                                   10 chars  (you know, for safety)
 * -----------------------------------------------------
 * TOTAL is                                 16 + DECIMAL_DIG + YARP_DBL_EXP_DIG
 */
#define YARP_DOUBLE_TO_STRING_MAX_LENGTH (16 + DECIMAL_DIG + YARP_DBL_EXP_DIG)


using namespace yarp::os::impl;
using namespace yarp::os;

namespace {

/*
 * Converts a floating point number to a string, dealing with locale issues
 */
template <typename T>
inline std::string fp_to_string(T x)
{
    char buf[YARP_DOUBLE_TO_STRING_MAX_LENGTH]; // -> see comment at the top of the file
    std::snprintf(buf, YARP_DOUBLE_TO_STRING_MAX_LENGTH, "%.*g", DECIMAL_DIG, x);
    std::string str(buf);

    // If locale is set, the locale version of the decimal point is used.
    // In this case we change it to the standard "."
    // If there is no decimal point, and it is not being used the exponential
    // notation (i.e. the number is in integer form, for example 100000 and not
    // 1e5) we add ".0" to ensure that it will be interpreted as a double.
    struct lconv* lc = localeconv();
    size_t offset = str.find(lc->decimal_point);
    if (offset != std::string::npos) {
        str[offset] = '.';
    } else if (str.find('e') == std::string::npos && str != "inf" && str != "-inf" && str != "nan") {
        str += ".0";
    }
    return str;
}

/*
 * Converts a string to a floating point number, dealing with locale issues
 */
template <typename T>
inline T fp_from_string(std::string src)
{
    if (src == "inf") {
        return std::numeric_limits<T>::infinity();
    }
    if (src == "-inf") {
        return -std::numeric_limits<T>::infinity();
    }
    if (src == "nan") {
        return std::numeric_limits<T>::quiet_NaN();
    }
    // YARP Bug 2526259: Locale settings influence YARP behavior
    // Need to deal with alternate versions of the decimal point.
    size_t offset = src.find('.');
    if (offset != std::string::npos) {
        struct lconv* lc = localeconv();
        src[offset] = lc->decimal_point[0];
    }
    return static_cast<T>(strtod(src.c_str(), nullptr));
}

} // namespace

int NetType::netInt(const yarp::os::Bytes& code)
{
    yAssert(code.length() == sizeof(NetInt32));
    NetInt32 tmp;
    memcpy((char*)(&tmp), code.get(), code.length());
    return tmp;
}

bool NetType::netInt(int data, yarp::os::Bytes& code)
{
    NetInt32 i = data;
    yarp::os::Bytes b((char*)(&i), sizeof(i));
    if (code.length() != sizeof(i)) {
        YARP_ERROR(Logger::get(), "not enough room for integer");
        return false;
    }
    memcpy(code.get(), b.get(), code.length());
    return true;
}

std::string NetType::toHexString(int x)
{
    char buf[256];
    sprintf(buf, "%x", x);
    return buf;
}

std::string NetType::toHexString(long x)
{
    char buf[256];
    sprintf(buf, "%lx", x);
    return buf;
}

std::string NetType::toHexString(unsigned int x)
{
    char buf[256];
    sprintf(buf, "%x", x);
    return buf;
}


std::string NetType::toString(int x)
{
    char buf[256];
    sprintf(buf, "%d", x);
    return buf;
}

std::string NetType::toString(long x)
{
    char buf[256];
    sprintf(buf, "%ld", x);
    return buf;
}

std::string NetType::toString(unsigned int x)
{
    char buf[256];
    sprintf(buf, "%u", x);
    return buf;
}


int NetType::toInt(const std::string& x)
{
    return atoi(x.c_str());
}


std::string NetType::toString(yarp::conf::float32_t x)
{
    return fp_to_string(x);
}

std::string NetType::toString(yarp::conf::float64_t x)
{
    return fp_to_string(x);
}

yarp::conf::float32_t NetType::toFloat32(const std::string& s)
{
    return fp_from_string<yarp::conf::float32_t>(s);
}

yarp::conf::float64_t NetType::toFloat64(const std::string& s)
{
    return fp_from_string<yarp::conf::float64_t>(s);
}

yarp::conf::float32_t NetType::toFloat32(std::string&& s)
{
    return fp_from_string<yarp::conf::float32_t>(std::move(s));
}

yarp::conf::float64_t NetType::toFloat64(std::string&& s)
{
    return fp_from_string<yarp::conf::float64_t>(std::move(s));
}


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
