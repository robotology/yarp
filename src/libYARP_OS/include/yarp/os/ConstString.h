// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONSTSTRING_
#define _YARP2_CONSTSTRING_

#include <yarp/os/api.h>
#include <yarp/conf/system.h>


// yarp::os::ConstString == std::string
#ifndef YARP_WRAP_STL_STRING
#include <string>
#define YARP_CONSTSTRING_IS_STD_STRING 1
namespace yarp {
    namespace os {
        typedef std::string ConstString;
    }
}


#else

// yarp::os::ConstString == opaque
#include <yarp/conf/numeric.h>
#include <string>
#include <iostream>

namespace yarp {
    namespace os {
        class ConstString;
    }
}

/**
 * A constant string.  The yarp::os library returns strings in this form,
 * to avoid forcing you to use the same string implementation
 * we do.
 */
class YARP_OS_API yarp::os::ConstString {
public:

    /**
     * Constructor.  Creates an empty string.
     */
    ConstString();

    /**
     * Constructor.  Stores a copy of the null-terminated specified string.
     * @param str the string to copy
     */
    ConstString(const char *str);

    /**
     * Constructor.  Stores a copy of the specified string.
     * @param str the string to copy
     * @param len number of bytes to copy
     */
    ConstString(const char *str, int len);

    ConstString(size_t len, char v);

    /**
     * Destructor.
     */
    ~ConstString();

    /**
     * Copy constructor.
     */
    ConstString(const ConstString& alt);

    /**
     * Accesses the character sequence stored in this object.
     */
    const char *c_str() const;

    /**
     * Typecast operator to C-style string.
     */
    //operator const char *() const {
    //return c_str();
    //}

    char& operator[](size_t idx);
    const char& operator[](size_t idx) const;

    const ConstString& operator = (const ConstString& alt);

    bool operator <(const ConstString& alt) const;
    bool operator >(const ConstString& alt) const;

    bool operator ==(const ConstString& alt) const;
    bool operator !=(const ConstString& alt) const;

    bool operator ==(const char *str) const;
    bool operator !=(const char *str) const;

    ConstString operator + (char ch) const;
    ConstString operator + (const char *str) const;
    ConstString operator + (const ConstString& alt) const;

    const ConstString& operator += (char ch);
    const ConstString& operator += (const char *str);
    const ConstString& operator += (const ConstString& alt);

    size_t length() const;

    size_t find(const ConstString& needle) const;
    size_t find(const char *needle) const;
    size_t find(const char *needle, size_t start) const;
    size_t find(char needle, size_t start) const;
    size_t rfind(char needle) const;

    ConstString substr(size_t start = 0, size_t n = (size_t)(-1)) const;

    static size_t npos;

    typedef size_t size_type;

    unsigned long hash() const;

    void clear();

    // ConstString is implemented internally as a std::string
    // We cannot expose that implementation without coupling
    // the user's compiler family/version too much with that
    // used to compile YARP.
    //
    // However, we can provide some convenience inlines that use 
    // the definition of std::string in user code.  Precedent:
    //   QString::toStdString()

    inline operator std::string() const
    { return std::string(c_str(), length()); }

private:

    void *implementation;
};

yarp::os::ConstString operator + (const char *txt, 
                                  const yarp::os::ConstString& alt);

inline std::ostream& operator<<(std::ostream& stream, 
                                const yarp::os::ConstString& alt) {
    stream << (std::string)alt;
    return stream;
}

#endif

#endif

