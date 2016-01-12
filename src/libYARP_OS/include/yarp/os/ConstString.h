// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_CONSTSTRING
#define YARP2_CONSTSTRING

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


#else // YARP_WRAP_STL_STRING

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

    static const size_t npos;

    typedef char value_type;
    typedef std::char_traits<char> traits_type;
    typedef std::allocator<char> allocator_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef char& reference;
    typedef const char& const_reference;
    typedef char* pointer;
    typedef const char* const_pointer;

    typedef std::string::iterator iterator;
    typedef std::string::const_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

#ifndef YARP_WRAP_STL_STRING_INLINE
    /** @{ */

    /**
     * Constructor.
     * Creates an empty string.
     */
    ConstString();

    /**
     * Copy constructor.
     * @param str The string to be copied.
     */
    ConstString(const ConstString& str);

    /**
     * Constructor.
     * Stores a copy of the substring.
     * @param str The string to be copied.
     * @param pos Position of the first character to be copied.
     * @param len Length of the substring to be copied.
     */
    ConstString(const ConstString& str, size_t pos, size_t len = npos);

    /**
     * Constructor.
     * Stores a copy of the null-terminated specified string.
     * @param str The string to be copied.
     */
    ConstString(const char *str);

    /**
     * Constructor.
     * Stores a copy of the specified string.
     * @param str The string to be copied.
     * @param len Number of bytes to copy.
     */
    ConstString(const char *str, size_t len);

    /**
     * Constructor.
     * Fills the string with a character.
     * @param len Number of characters to copy.
     * @param v The character to copy.
     */
    ConstString(size_t len, char c);

    /**
     * Destructor.
     */
    ~ConstString();

    /**
     * Assignment operator.
     * @param str The string to be copied.
     */
    ConstString& operator=(const ConstString& str);

    /**
     * Assignment operator.
     * @param str The string to be copied.
     */
    ConstString& operator=(const char* str);

     /**
      * Assignment operator.
      * @param c The character to be copied.
      */
    ConstString& operator=(char c);


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
    /** @} */


    /**
     * \name Iterators
     * @{
     */
    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;
    /** @} */


    /**
     * \name Capacity
     * @{
     */
    size_t size() const;
    size_t length() const;
    size_t max_size() const;
    void resize(size_t n);
    void resize(size_t n, char c);
    size_t capacity() const;
    void reserve(size_t n = 0);
    void clear();
    bool empty() const;
    /** @} */


    /**
     * \name Element access
     * @{
     */
    char& operator[](size_t idx);
    const char& operator[](size_t idx) const;

    char& at(size_t pos);
    const char& at(size_t pos) const;
    /** @} */


    /**
     * \name Modifiers
     * @{
     */
    ConstString& operator+=(const ConstString& str);
    ConstString& operator+=(const char *str);
    ConstString& operator+=(char ch);

    void push_back (char c);

    ConstString& assign(const char *s, size_t n);

    ConstString& erase(size_t pos = 0, size_t len = npos);
    iterator erase(iterator p);
    iterator erase(iterator first, iterator last);

    void swap(ConstString& str);
    /** @} */


    /**
     * \name String operations
     * @{
     */

    /**
     * Accesses the character sequence stored in this object.
     */
    const char *c_str() const;

    /**
     * Get string data.
     */
    const char *data() const;

    /**
     * Get allocator.
     */
    allocator_type get_allocator() const;

    /**
     * Copy sequence of characters from string.
     */
    size_t copy(char* str, size_t len, size_t pos = 0) const;

    /**
     * Find content in string.
     */
    size_t find(const ConstString& needle, size_t start = 0) const;
    size_t find(const char *needle, size_t start = 0) const;
    size_t find(const char *needle, size_t start, size_t len) const;
    size_t find(char needle, size_t start = 0) const;

    /**
     * Find the last occurrence of content in string.
     */
    size_t rfind(const ConstString& needle, size_t start = npos) const;
    size_t rfind(const char *needle, size_t start = npos) const;
    size_t rfind(const char *needle, size_t start, size_t len) const;
    size_t rfind(char needle, size_t start = npos) const;

    /**
     * Generate a substring.
     */
    ConstString substr(size_t start = 0, size_t n = npos) const;
    /** @} */



    /**
     * \name Non std::string
     * @{
     */
    unsigned long hash() const;
    /** @} */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    void init(const char *str, size_t len);
    void *implementation;
#endif

#else // YARP_WRAP_STL_STRING_INLINE

    inline ConstString() {}

    inline ConstString(const ConstString& str) : s(str.s) {}

    inline ConstString(const ConstString& str, size_t pos, size_t len = npos) : s(str.s, pos, len) {}

    inline ConstString(const char *str) : s(str) {}

    inline ConstString(const char *str, size_t len) : s(str,len) {}

    inline ConstString(size_t len, char c) : s(len, c) {}

    inline ~ConstString() {}


    inline const char *c_str() const { return s.c_str(); }

    inline const char *data() const { return s.data(); }

    inline allocator_type get_allocator() const { return s.get_allocator(); }

    inline size_t copy(char* str, size_t len, size_t pos = 0) const { return s.copy(str, len, pos); }

    inline void push_back (char c) { s.push_back(c); }

    inline ConstString& assign(const char *s, size_t n) {
        this->s.assign(s,n);
        return *this;
    }

    inline char& operator[](size_t idx) { return s[idx]; }
    inline const char& operator[](size_t idx) const { return s[idx]; }

    inline char& at(size_t pos) { return s.at(pos); }
    inline const char& at(size_t pos) const { return s.at(pos); }

    inline const ConstString& operator=(const ConstString& str) {
        s = str.s;
        return *this;
    }

    inline ConstString& operator=(const char* str) {
        s = str;
        return *this;
    }

    inline ConstString& operator=(char c) {
        s = c;
        return *this;
    }

    inline const ConstString& operator+=(char ch) {
        s += ch;
        return *this;
    }

    inline const ConstString& operator+=(const char *str) {
        s += str;
        return *this;
    }

    inline const ConstString& operator+=(const ConstString& str) {
        s += str.s;
        return *this;
    }

    inline ConstString& erase(size_t pos = 0, size_t len = npos) {
        s.erase(pos, len);
        return *this;
    }

    inline iterator erase(iterator p) {
        return s.erase(p);
    }

    inline iterator erase(iterator first, iterator last) {
        return s.erase(first, last);
    }

    inline void swap(ConstString& str) { s.swap(str.s); }

    inline size_t size() const { return s.length(); }

    inline size_t length() const { return s.length(); }

    inline size_t max_size() const { return s.max_size(); }

    inline void resize(size_t n) { s.resize(n); }
    inline void resize(size_t n, char c) { s.resize(n, c); }

    inline size_t capacity() const { return s.capacity(); }

    inline void reserve(size_t n = 0) { s.reserve(n); }

    inline size_t find(const ConstString& needle, size_t start = 0) const { return s.find(needle.s, start); }
    inline size_t find(const char *needle, size_t start = 0) const { return s.find(needle, start); }
    inline size_t find(const char *needle, size_t start, size_t len) const { return s.find(needle, start, len); }
    inline size_t find(char needle, size_t start = 0) const { return s.find(needle, start); }

    inline size_t rfind(const ConstString& needle, size_t start = npos) const { return s.rfind(needle.s, start); }
    inline size_t rfind(const char *needle, size_t start = npos) const { return s.rfind(needle, start); }
    inline size_t rfind(const char *needle, size_t start, size_t len) const { return s.rfind(needle, start, len); }
    inline size_t rfind(char needle, size_t start = npos) const { return s.rfind(needle, start); }

    inline ConstString substr(size_t start = 0, size_t n = npos) const { return s.substr(start,n); }


    unsigned long hash() const;

    inline void clear() { s.clear(); }

    inline bool empty() const { return s.empty(); }

    inline iterator begin() { return s.begin(); }
    inline const_iterator begin() const { return s.begin(); }

    inline iterator end() { return s.end(); }
    inline const_iterator end() const { return s.end(); }

    inline reverse_iterator rbegin() { return s.rbegin(); }
    inline const_reverse_iterator rbegin() const { return s.rbegin(); }

    inline reverse_iterator rend() { return s.rend(); }
    inline const_reverse_iterator rend() const { return s.rend(); }

    inline operator std::string() const
    { return s; }

    inline ConstString(const std::string& str) {
        s = str;
    }

private:
    std::string s;

#endif // YARP_WRAP_STL_STRING_INLINE
};

namespace yarp {
    namespace os {
        /**
         * @{
         * Extract string from stream.
         */
        inline std::istream& operator>>(std::istream& stream,
                                        yarp::os::ConstString& str) {
            stream >> (std::string&)str;
            return stream;
        }
        /** @} */

        /**
         * @{
         * Insert string into stream
         */
        inline std::ostream& operator<<(std::ostream& stream,
                                        const yarp::os::ConstString& str) {
            stream << (const std::string&)str;
            return stream;
        }
        /** @} */

        /**
         * @{
         * Concatenate ConstString
         */
        inline yarp::os::ConstString operator+(const yarp::os::ConstString& lhs,
                                               const yarp::os::ConstString& rhs) {
            return operator+((const std::string&)lhs, (const std::string&)rhs);
        }

        inline yarp::os::ConstString operator+(const yarp::os::ConstString& lhs,
                                               const char* rhs) {
            return operator+((const std::string&)lhs, rhs);
        }

        inline yarp::os::ConstString operator+(const char* lhs,
                                               const yarp::os::ConstString& rhs) {
            return operator+(lhs, (const std::string&)rhs);
        }

        inline yarp::os::ConstString operator+(const yarp::os::ConstString& lhs,
                                               char rhs) {
            return operator+((const std::string&)lhs, rhs);
        }

        inline yarp::os::ConstString operator+(char lhs,
                                               const yarp::os::ConstString& rhs) {
            return operator+(lhs, (const std::string&)rhs);
        }
        /** @} */

        /**
         * @{
         * Relational operations for ConstString
         */
        inline bool operator==(const yarp::os::ConstString& lhs,
                               const yarp::os::ConstString& rhs) {
            return operator==((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator==(const char* lhs,
                               const yarp::os::ConstString& rhs) {
            return operator==(lhs, (const std::string&)rhs);
        }

        inline bool operator==(const yarp::os::ConstString& lhs,
                               const char* rhs) {
            return operator==((const std::string&)lhs, rhs);
        }

        inline bool operator!=(const yarp::os::ConstString& lhs,
                               const yarp::os::ConstString& rhs) {
            return operator!=((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator!=(const char* lhs,
                               const yarp::os::ConstString& rhs) {
            return operator!=(lhs, (const std::string&)rhs);
        }

        inline bool operator!=(const yarp::os::ConstString& lhs,
                               const char* rhs) {
            return operator!=((const std::string&)lhs, rhs);
        }

        inline bool operator<(const yarp::os::ConstString& lhs,
                              const yarp::os::ConstString& rhs) {
            return operator<((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator<(const char* lhs,
                              const yarp::os::ConstString& rhs) {
            return operator<(lhs, (const std::string&)rhs);
        }

        inline bool operator<(const yarp::os::ConstString& lhs,
                              const char* rhs) {
            return operator<((const std::string&)lhs, rhs);
        }

        inline bool operator<=(const yarp::os::ConstString& lhs,
                               const yarp::os::ConstString& rhs) {
            return operator<=((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator<=(const char* lhs,
                               const yarp::os::ConstString& rhs) {
            return operator<=(lhs, (const std::string&)rhs);
        }

        inline bool operator<=(const yarp::os::ConstString& lhs,
                               const char* rhs) {
            return operator<=((const std::string&)lhs, rhs);
        }

        inline bool operator>(const yarp::os::ConstString& lhs,
                              const yarp::os::ConstString& rhs) {
            return operator>((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator>(const char* lhs,
                              const yarp::os::ConstString& rhs) {
            return operator>(lhs, (const std::string&)rhs);
        }

        inline bool operator>(const yarp::os::ConstString& lhs,
                              const char* rhs) {
            return operator>((const std::string&)lhs, rhs);
        }

        inline bool operator>=(const yarp::os::ConstString& lhs,
                               const yarp::os::ConstString& rhs) {
            return operator>=((const std::string&)lhs, (const std::string&)rhs);
        }

        inline bool operator>=(const char* lhs,
                               const yarp::os::ConstString& rhs) {
            return operator>=(lhs, (const std::string&)rhs);
        }

        inline bool operator>=(const yarp::os::ConstString& lhs,
                               const char* rhs) {
            return operator>=((const std::string&)lhs, rhs);
        }
        /** @} */

        using std::swap;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace std {
    inline void swap(yarp::os::ConstString& x, yarp::os::ConstString& y) {
        swap((std::string&)x, (std::string&)y);
    }

    inline std::istream& getline(std::istream& is, yarp::os::ConstString& str, char delim) {
        return getline(is, (std::string&)str, delim);
    }

    inline std::istream& getline(std::istream& is, yarp::os::ConstString& str) {
        return getline(is, (std::string&)str);
    }

}
#endif // DOXYGEN_SHOULD_SKIP_THIS



#endif // YARP_WRAP_STL_STRING

#endif // _YARP2_CONSTSTRING_

