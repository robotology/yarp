// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONSTSTRING_
#define _YARP2_CONSTSTRING_

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
class yarp::os::ConstString {
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
    operator const char *() const { 
        return c_str(); 
    }

    const ConstString& operator = (const ConstString& alt);

    bool operator ==(const ConstString& alt) const;
    bool operator !=(const ConstString& alt) const;

    bool operator ==(const char *str) const;
    bool operator !=(const char *str) const;

    ConstString operator + (char ch) const;
    ConstString operator + (const char *str) const;
    ConstString operator + (const ConstString& alt) const;

    int length() const;

    int find(const char *needle) const;
    int find(const char *needle, int start) const;

    ConstString substr(int start = 0, int n = -1) const;

    static int npos;

    typedef int size_type;

    static ConstString toString(int x);
private:

    void *implementation;
};

#endif

