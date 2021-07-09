/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_VALUE_H
#define YARP_OS_VALUE_H

#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Vocab.h>

#include <cstddef> // defines size_t
#include <string>

namespace yarp {
namespace os {
class Property;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace impl {
class Storable;
}
#endif // DOXYGEN_SHOULD_SKIP_THIS
} // namespace os
} // namespace yarp

namespace yarp {
namespace os {

/**
 * A single value (typically within a Bottle). Values can be integers, strings,
 * doubles (floating-point numbers), lists, vocabulary, or blobs
 * (unformatted binary data).  This set is carefully chosen to have
 * good text and binary representations both for network transmission
 * and human viewing/generation.  Lists are represented as a nested
 * Bottle object.  Value objects are Searchable - but you won't
 * find anything in them unless they are actually a list.
 *
 */
class YARP_os_API Value : public Portable, public Searchable
{
public:
    using Searchable::check;
    using Searchable::findGroup;

    /**
     * Construct a list Value
     */
    explicit Value();

    /**
     * Construct an integer Value
     * @param x the integer
     * @param isVocab32 set this to true if the integer should be interpreted
     * as a vocabulary identifier
     */
    explicit Value(std::int32_t x, bool isVocab32 = false);

    /**
     * Construct a floating point Value
     * @param x the number
     */
    explicit Value(yarp::conf::float64_t x);

    /**
     * Construct a string Value
     * @param str the string
     * @param isVocab32 set this to true if the string should be interpreted
     * as a vocabulary identifier
     */
    explicit Value(const std::string& str, bool isVocab32 = false);

    /**
     * Construct a binary data Value
     * @param data the data to copy
     * @param length the size of the data in bytes
     */
    Value(void* data, int length);

    /**
     * Copy constructor.
     * @param alt the value to copy
     */
    Value(const Value& alt);

    /**
     * Assignment operator
     * @param alt the value to copy
     * @return this object itself
     */
    const Value& operator=(const Value& alt);

    /**
     * Destructor
     */
    ~Value() override;

    /**
     * Checks if value is a boolean. If so, asBool() will return that boolean.
     * @return true iff value is a boolean
     */
    virtual bool isBool() const;

    /**
     * Checks if value is an integer. If so, asInt() will return that integer.
     * @return true iff value is an integer
     * @warning Unsafe, sizeof(int) is platform dependent. Use isInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use isInt32 instead") // Since YARP 3.5.0
    inline virtual bool isInt() const final
    {
        return isInt32();
    }

    /**
     * Checks if value is a 8-bit integer. If so, asInt8() will
     * return that integer.
     * @return true iff value is a 8-bit integer
     */
    virtual bool isInt8() const;

    /**
     * Checks if value is a 16-bit integer. If so, asInt16() will
     * return that integer.
     * @return true iff value is a 16-bit integer
     */
    virtual bool isInt16() const;

    /**
     * Checks if value is a 32-bit integer. If so, asInt32() will
     * return that integer.
     * @return true iff value is a 32-bit integer
     */
    virtual bool isInt32() const;

    /**
     * Checks if value is a 64-bit integer. If so, asInt64() will
     * return that integer.
     * @return true iff value is a 64-bit integer
     */
    virtual bool isInt64() const;

    /**
     * Checks if value is a floating point number.
     * If so, asDouble() will return that number.
     * @return true iff value is a floating point number
     * @warning Unsafe, sizeof(double) is platform dependent. Use isFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use isFloat64 instead") // Since YARP 3.5.0
    inline virtual bool isDouble() const final
    {
        return isFloat64();
    }

    /**
     * Checks if value is a 32-bit floating point number.
     * If so, asFloat32() will return that number.
     * @return true iff value is a 32-bit floating point number
     */
    virtual bool isFloat32() const;

    /**
     * Checks if value is a 64-bit floating point number.
     * If so, asFloat64() will return that number.
     * @return true iff value is a 64-bit floating point number
     */
    virtual bool isFloat64() const;

    /**
     * Checks if value is a string. If so, asString() will return that string.
     * @return true iff value is a string
     */
    virtual bool isString() const;

    /**
     * Checks if value is a list. If so, asList() will return that list.
     * @return true iff value is a list
     */
    virtual bool isList() const;

    /**
     * Checks if value is a dictionary. If so, asDict() will return that
     * dictionary.
     * @return true iff value is a dictionary
     */
    virtual bool isDict() const;

    /**
     * Checks if value is a vocabulary identifier. If so, asVocab32()
     * will return it.
     * @return true iff value is a vocabulary identifier
     */
    virtual bool isVocab32() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Checks if value is a vocabulary identifier. If so, asVocab()
     * will return it.
     * @return true iff value is a vocabulary identifier
     * @deprecated since YARP 3.5. Use isVocab32 instead.
     */
    YARP_DEPRECATED_MSG("Use isVocab32 instead") // Since YARP 3.5.0
    virtual bool isVocab() const
    {
        return isVocab32();
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Checks if value is a binary object. If so, asBlob() and asBlobLength()
     * will return it.
     * @return true iff value is a binary object.
     */
    virtual bool isBlob() const;

    /**
     * Get boolean value.
     * @return boolean value if value is indeed a boolean.
     * If it is another type, the appropriate cast value is returned.
     * Otherwise returns false.
     */
    virtual bool asBool() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Get integer value.
     * @return integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning Unsafe, sizeof(int) is platform dependent. Use asInt32 instead.
     * @deprecated Since YARP 3.5. Use asInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use asInt32 instead")
    inline virtual int asInt() const final
    {
        return static_cast<int>(asInt32());
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Get 8-bit integer value.
     * @return 8-bit integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual std::int8_t asInt8() const;

    /**
     * Get 16-bit integer value.
     * @return 16-bit integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual std::int16_t asInt16() const;

    /**
     * Get 32-bit integer value.
     * @return 32-bit integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual std::int32_t asInt32() const;

    /**
     * Get 64-bit integer value.
     * @return 64-bit integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual std::int64_t asInt64() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Get floating point value.
     * @return floating point value if value is indeed a floating point.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning Unsafe, sizeof(double) is platform dependent. Use asFloat64 instead.
     * @deprecated Since YARP 3.5. Use asFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use asFloat64 instead")
    inline virtual double asDouble() const final
    {
        return static_cast<double>(asFloat64());
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Get 32-bit floating point value.
     * @return floating point value if value is indeed a 32-bit floating point.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual yarp::conf::float32_t asFloat32() const;

    /**
     * Get 64-bit floating point value.
     * @return floating point value if value is indeed a 64-bit floating point.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     * @warning This method performs casts if the Value is not a Float32 value,
     * therefore it might lead to unexpected behaviours if the type is not
     * properly checked.
     */
    virtual yarp::conf::float64_t asFloat64() const;

    /**
     * Get vocabulary identifier as an integer.
     * @return integer value of vocabulary identifier.
     */
    virtual yarp::conf::vocab32_t asVocab32() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Get vocabulary identifier as an integer.
     * @return integer value of vocabulary identifier.
     * @deprecated Since YARP 3.5. Use asVocab32 instead.
     */
    YARP_DEPRECATED_MSG("Use asVocab32 instead")
    virtual yarp::conf::vocab32_t asVocab() const
    {
        return asVocab32();
    }
#endif

    /**
     * Get string value.
     * @return string value if value is indeed a string.
     * Otherwise returns empty string.
     */
    virtual std::string asString() const;

    /**
     * Get list value.
     * @return pointer to list if value is indeed a list.
     * Otherwise returns nullptr
     */
    virtual Bottle* asList() const;

    /**
     * Get dictionary (hash table) value.
     * @return pointer to dictionary if value is indeed of the right type.
     * Otherwise returns nullptr
     */
    virtual Property* asDict() const;

    /**
     * Get dictionary or list value
     * @return pointer to dictionary or list value if present.
     * Otherwise returns nullptr
     */
    virtual Searchable* asSearchable() const;

    /**
     * Get binary data value.
     * @return pointer to binary data if value is indeed binary data.
     * Otherwise returns nullptr
     */
    virtual const char* asBlob() const;

    /**
     * Get binary data length.
     * @return length of binary data if value is indeed binary data.
     * Otherwise returns 0
     */
    virtual size_t asBlobLength() const;

    // documented in Portable
    bool read(ConnectionReader& connection) override;

    // documented in Portable
    bool write(ConnectionWriter& connection) const override;

    // documented in Searchable
    bool check(const std::string& key) const override;

    // documented in Searchable
    Value& find(const std::string& key) const override;

    // documented in Searchable
    Bottle& findGroup(const std::string& key) const override;

    /**
     * Equality test.
     * @param alt the value to compare against
     * @result true iff the values are equal
     */
    bool operator==(const Value& alt) const;

    /**
     * Inequality test.
     * @param alt the value to compare against
     * @result true iff the values are not equal
     */
    bool operator!=(const Value& alt) const;

    /**
     * Set value to correspond to a textual representation.
     * For example if str="10" then the value will be an integer,
     * if str="(1 2 3)" the value will be a list.
     * @param str the textual representation.
     */
    void fromString(const char* str);

    std::string toString() const override;

    /**
     * Create a new value of the same type.
     * @return the new value.
     */
    virtual Value* create() const;

    /**
     * Create a copy of the value
     * @return the new value, which will equal this.
     */
    virtual Value* clone() const;

    /**
     * Get standard type code of value.
     * @return the standard type code of the value.
     */
    virtual std::int32_t getCode() const;

    bool isNull() const override;

    virtual bool isLeaf() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Create an integer Value
     * @param x the value to take on
     * @return an integer Value
     * @warning Unsafe, sizeof(int) is platform dependent. Use makeInt instead.
     * @deprecated Since YARP 3.5. Use makeInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use makeInt32 instead")
    inline static Value* makeInt(int x)
    {
        return makeInt32(static_cast<std::int32_t>(x));
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Create a 8-bit integer Value
     * @param x the value to take on
     * @return a 8-bit integer Value
     */
    static Value* makeInt8(std::int8_t x);

    /**
     * Create a 16-bit integer Value
     * @param x the value to take on
     * @return a 16-bit integer Value
     */
    static Value* makeInt16(std::int16_t x);

    /**
     * Create a 32-bit integer Value
     * @param x the value to take on
     * @return a 32-bit integer Value
     */
    static Value* makeInt32(std::int32_t x);

    /**
     * Create a 64-bit integer Value
     * @param x the value to take on
     * @return a 64-bit integer Value
     */
    static Value* makeInt64(std::int64_t x);

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Create a floating point Value
     * @param x the value to take on
     * @return a floating point Value
     * @warning Unsafe, sizeof(double) is platform dependent. Use makeFloat64 instead.
     * @deprecated Since YARP 3.5. Use makeFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use makeFloat64 instead")
    inline static Value* makeDouble(double x)
    {
        return makeFloat64(static_cast<yarp::conf::float64_t>(x));
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Create a 32-bit floating point Value
     * @param x the value to take on
     * @return a 32-bit floating point Value
     */
    static Value* makeFloat32(yarp::conf::float32_t x);

    /**
     * Create a 64-bit floating point Value
     * @param x the value to take on
     * @return a 64-bit floating point Value
     */
    static Value* makeFloat64(yarp::conf::float64_t x);

    /**
     * Create a string Value
     * @param str the value to take on
     * @return a string Value
     */
    static Value* makeString(const std::string& str);

    /**
     * Create a vocabulary identifier Value
     * @param v the value to take on
     * @return a vocabulary identifier Value
     */
    static Value* makeVocab32(yarp::conf::vocab32_t v);

    /**
     * Create a vocabulary identifier Value
     * @param a first character of the vocab
     * @param b second character of the vocab
     * @param c third character of the vocab
     * @param d fourth character of the vocab
     * @return a vocabulary identifier Value
     */
    static Value* makeVocab32(char a, char b = 0, char c = 0, char d = 0)
    {
        return makeVocab32(yarp::os::createVocab32(a, b, c, d));
    }

    /**
     * Create a vocabulary identifier Value
     * If the string is longer than 4 characters, only the first 4 are used.
     * @param str the value to take on
     * @return a vocabulary identifier Value
     */
    static Value* makeVocab32(const std::string& str)
    {
        return makeVocab32(yarp::os::Vocab32::encode(str));
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    /**
     * Create a vocabulary identifier Value
     * @param v the value to take on
     * @return a vocabulary identifier Value
     * @deprecated Since YARP 3.5. Use makeVocab32 instead.
     */
    YARP_DEPRECATED_MSG("Use makeVocab32 instead")
    static Value* makeVocab(yarp::conf::vocab32_t v)
    {
        return makeVocab32(v);
    }

    /**
     * Create a vocabulary identifier Value
     * @param str the value to take on
     * @return a vocabulary identifier Value
     * @deprecated Since YARP 3.5. Use makeVocab32 instead.
     */
    YARP_DEPRECATED_MSG("Use makeVocab32 instead")
    static Value* makeVocab(const std::string& str)
    {
        return makeVocab32(str);
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Create a Value containing binary data
     * @param data the data (will be copied)
     * @param length the size of the data in bytes
     * @return a Value containing binary data
     */
    static Value* makeBlob(void* data, int length);

    /**
     * Create a list Value
     * @return a list Value
     */
    static Value* makeList();

    /**
     * Create a list Value and initialize it
     * @param txt the list in text form, e.g. "1 2.0 (4 5) xyz"
     * @return a list Value
     */
    static Value* makeList(const char* txt);

    /**
     * Create a Value from a text description.
     * @param txt the description.  E.g. "10" will create an integer,
     * "(5 6 7)" will create a list.
     * @return the Value to which the text description corresponds
     */
    static Value* makeValue(const std::string& txt);


    /**
     * Return an invalid, "null" Value
     * @return an invalid "null" Value
     */
    static Value& getNullValue();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    yarp::os::impl::Storable* proxy;

    void setProxy(yarp::os::impl::Storable* proxy);
    void ok() const;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_VALUE_H
