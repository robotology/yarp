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

// yarp::os::ConstString == opaque/inline wrapped version of std::string
#include <yarp/conf/numeric.h>
#include <string>
#include <iostream>

namespace yarp {
    namespace os {
        class ConstString;
    }
}

/**
 *
 * A string with almost the same api as std::string.  It adds a hash
 * function for compatibility with certain versions of the ACE library.
 *
 */
class YARP_OS_API yarp::os::ConstString {
public:

#ifndef YARP_WRAP_STL_STRING_INLINE

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

    const char *data() const;

    ConstString& assign(const char *s, size_t n);

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

    bool operator <=(const ConstString& alt) const;
    bool operator >=(const ConstString& alt) const;

    ConstString operator + (char ch) const;
    ConstString operator + (const char *str) const;
    ConstString operator + (const ConstString& alt) const;

    const ConstString& operator += (char ch);
    const ConstString& operator += (const char *str);
    const ConstString& operator += (const ConstString& alt);

    size_t length() const;

    size_t size() const { return length(); }

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

    inline ConstString(const std::string& str) {
        init(str.c_str(),str.length());
    }

private:
    void init(const char *str, size_t len);

    void *implementation;

#else

    inline ConstString() {}
    
    inline ConstString(const char *str) : s(str) {}
    
    inline ConstString(const char *str, int len) : s(str,len) {}
    
    inline ConstString(size_t len, char v) : s(len,v) {}

    inline ~ConstString() {}
    
    inline ConstString(const ConstString& alt) : s(alt.s) {}
    
    inline const char *c_str() const { return s.c_str(); }
    
    inline const char *data() const { return s.data(); }
    
    inline ConstString& assign(const char *s, size_t n) { 
        this->s.assign(s,n);
        return *this;
    }
    
    inline char& operator[](size_t idx) { return s[idx]; }
    
    inline const char& operator[](size_t idx) const { return s[idx]; }
    
    inline const ConstString& operator = (const ConstString& alt) {
        s = alt.s;
        return *this;
    }
    
    inline bool operator < (const ConstString& alt) const { return (s<alt.s); }
    inline bool operator > (const ConstString& alt) const { return (s>alt.s); }

    inline bool operator == (const ConstString& alt) const { return (s==alt.s); }
    inline bool operator != (const ConstString& alt) const { return (s!=alt.s); }

    inline bool operator == (const char *str) const { return (s==str); }
    inline bool operator != (const char *str) const { return (s!=str); }

    inline bool operator <= (const ConstString& alt) const { return (s<=alt.s); }
    inline bool operator >= (const ConstString& alt) const { return (s>=alt.s); }

    inline ConstString operator + (char ch) const { return s + ch; }
    inline ConstString operator + (const char *str) const { return s + str; }
    inline ConstString operator + (const ConstString& alt) const { return s + alt.s; }

    inline const ConstString& operator += (char ch) {
        s += ch;
        return *this;
    }

    inline const ConstString& operator += (const char *str) {
        s += str;
        return *this;
    }

    inline const ConstString& operator += (const ConstString& alt) {
        s += alt.s;
        return *this;
    }

    inline size_t length() const { return s.length(); }

    inline size_t size() const { return length(); }

    inline size_t find(const ConstString& needle) const { return s.find(needle.s); }
    inline size_t find(const char *needle) const        { return s.find(needle); }
    inline size_t find(const char *needle, size_t start) const { return s.find(needle,start); }
    inline size_t find(char needle, size_t start) const { return s.find(needle,start); }
    inline size_t rfind(char needle) const { return s.rfind(needle); }

    inline ConstString substr(size_t start = 0, size_t n = (size_t)(-1)) const {
        return s.substr(start,n);
    }

    static size_t npos;

    typedef size_t size_type;
    
    unsigned long hash() const;
    
    inline void clear() { s.clear(); }

    inline operator std::string() const
    { return s; }

    inline ConstString(const std::string& str) {
        s = str;
    }
    
private:
    std::string s;

#endif
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

