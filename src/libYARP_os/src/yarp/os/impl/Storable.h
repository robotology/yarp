/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_STORABLE_H
#define YARP_OS_IMPL_STORABLE_H


#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>


#define UNIT_MASK         \
    (BOTTLE_TAG_INT8    | \
     BOTTLE_TAG_INT16   | \
     BOTTLE_TAG_INT32   | \
     BOTTLE_TAG_INT64   | \
     BOTTLE_TAG_FLOAT32 | \
     BOTTLE_TAG_FLOAT64 | \
     BOTTLE_TAG_VOCAB32 | \
     BOTTLE_TAG_STRING  | \
     BOTTLE_TAG_BLOB)

#define GROUP_MASK     \
    (BOTTLE_TAG_LIST | \
     BOTTLE_TAG_DICT)

YARP_DECLARE_LOG_COMPONENT(STORABLE)

namespace yarp {
namespace os {
namespace impl {

/**
 * A single item in a Bottle.  This extends the public yarp::os::Value
 * interface with some implementation-specific details.
 */
class YARP_os_impl_API Storable : public yarp::os::Value
{
public:
    /**
     * Destructor.
     */
    virtual ~Storable();

    /**
     * Factory method.
     */
    virtual Storable* createStorable() const = 0;

    /**
     * Typed synonym for clone()
     */
    virtual Storable* cloneStorable() const
    {
        Storable* item = createStorable();
        yCAssert(STORABLE, item != nullptr);
        item->copy(*this);
        return item;
    }

    /**
     * Become a copy of the passed item.
     */
    virtual void copy(const Storable& alt) = 0;

    bool operator==(const yarp::os::Value& alt) const;

    yarp::os::Value* create() const override
    {
        return createStorable();
    }

    yarp::os::Value* clone() const override
    {
        return cloneStorable();
    }

    static Storable* createByCode(std::int32_t id);


    bool read(ConnectionReader& connection) override;
    bool write(ConnectionWriter& connection) const override;

    virtual bool readRaw(ConnectionReader& connection) = 0;
    virtual bool writeRaw(ConnectionWriter& connection) const = 0;

    bool isBool() const override
    {
        return false;
    }

    bool asBool() const override
    {
        return false;
    }

    bool isInt8() const override
    {
        return false;
    }

    std::int8_t asInt8() const override
    {
        return 0;
    }

    bool isInt16() const override
    {
        return false;
    }

    std::int16_t asInt16() const override
    {
        return 0;
    }

    bool isInt32() const override
    {
        return false;
    }

    std::int32_t asInt32() const override
    {
        return 0;
    }

    bool isInt64() const override
    {
        return false;
    }

    std::int64_t asInt64() const override
    {
        return 0;
    }

    bool isFloat32() const override
    {
        return false;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return 0.0f;
    }

    bool isFloat64() const override
    {
        return false;
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return 0.0;
    }

    bool isString() const override
    {
        return false;
    }

    std::string asString() const override
    {
        return {};
    }

    bool isList() const override
    {
        return false;
    }

    yarp::os::Bottle* asList() const override
    {
        return nullptr;
    }

    bool isDict() const override
    {
        return false;
    }

    yarp::os::Property* asDict() const override
    {
        return nullptr;
    }

    bool isVocab32() const override
    {
        return false;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return 0;
    }

    bool isBlob() const override
    {
        return false;
    }

    const char* asBlob() const override
    {
        return static_cast<const char*>(nullptr);
    }

    size_t asBlobLength() const override
    {
        return 0;
    }

    bool isNull() const override
    {
        return false;
    }


    Searchable* asSearchable() const override
    {
        if (isDict()) {
            return asDict();
        }
        return asList();
    }
    using yarp::os::Searchable::check;
    bool check(const std::string& key) const override;

    yarp::os::Value& find(const std::string& key) const override;
    yarp::os::Bottle& findGroup(const std::string& key) const override;


    /**
     * Initialize from a string representation, assuming that any
     * syntax around this representation such as braces or
     * parentheses has already been consumed.
     */
    virtual void fromString(const std::string& src) = 0;

    /**
     * Initialize from a string representation.  This should consume
     * any syntax around that representation such as braces or
     * parentheses.
     */
    virtual void fromStringNested(const std::string& src)
    {
        fromString(src);
    }

    std::string toString() const override = 0;

    /**
     * Create string representation, including any syntax that should
     * wrap it such as braces or parentheses.
     */
    virtual std::string toStringNested() const
    {
        return toString();
    }

    /**
     * Return a code describing this item, used in serializing bottles.
     */
    virtual std::int32_t subCode() const
    {
        return 0;
    }

    bool isLeaf() const override
    {
        return true;
    }
};


/**
 * An empty item.
 */
class YARP_os_impl_API StoreNull :
        public Storable
{
public:
    StoreNull() = default;

    Storable* createStorable() const override
    {
        return new StoreNull();
    }

    void copy(const Storable& alt) override
    {
        YARP_UNUSED(alt);
    }

    std::string toString() const override
    {
        return {};
    }

    void fromString(const std::string& src) override
    {
        YARP_UNUSED(src);
    }

    std::int32_t getCode() const override
    {
        return -1;
    }

    bool readRaw(ConnectionReader& connection) override
    {
        YARP_UNUSED(connection);
        return false;
    }

    bool writeRaw(ConnectionWriter& connection) const override
    {
        YARP_UNUSED(connection);
        return false;
    }

    bool isNull() const override
    {
        return true;
    }
};


/**
 * A 8-bit integer item.
 */
class YARP_os_impl_API StoreInt8 :
        public Storable
{
private:
    std::int8_t x{0};

public:
    StoreInt8() = default;

    StoreInt8(std::int8_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreInt8();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asInt8();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isInt8() const override
    {
        return true;
    }

    bool asBool() const override
    {
        return x != 0;
    }

    std::int8_t asInt8() const override
    {
        return x;
    }

    std::int16_t asInt16() const override
    {
        return x;
    }

    std::int32_t asInt32() const override
    {
        return x;
    }

    std::int64_t asInt64() const override
    {
        return x;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return x;
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return x;
    }
};


/**
 * A 16-bit integer item.
 */
class YARP_os_impl_API StoreInt16 :
        public Storable
{
private:
    std::int16_t x{0};

public:
    StoreInt16() = default;

    StoreInt16(std::int16_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreInt16();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asInt16();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isInt16() const override
    {
        return true;
    }

    bool asBool() const override
    {
        return x != 0;
    }

    std::int8_t asInt8() const override
    {
        return static_cast<std::int8_t>(x);
    }

    std::int16_t asInt16() const override
    {
        return x;
    }

    std::int32_t asInt32() const override
    {
        return x;
    }

    std::int64_t asInt64() const override
    {
        return x;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return x;
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return x;
    }
};


/**
 * A 32-bit integer item.
 */
class YARP_os_impl_API StoreInt32 :
        public Storable
{
private:
    std::int32_t x{0};

public:
    StoreInt32() = default;

    StoreInt32(std::int32_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreInt32();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asInt32();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    std::int8_t asInt8() const override
    {
        return static_cast<std::int8_t>(x);
    }

    bool asBool() const override
    {
        return x != 0;
    }

    std::int16_t asInt16() const override
    {
        return static_cast<std::int16_t>(x);
    }

    bool isInt32() const override
    {
        return true;
    }

    std::int32_t asInt32() const override
    {
        return x;
    }

    std::int64_t asInt64() const override
    {
        return x;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return static_cast<yarp::conf::float32_t>(x);
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return x;
    }
};

/**
 * A 64-bit integer item.
 */
class YARP_os_impl_API StoreInt64 :
        public Storable
{
private:
    std::int64_t x{0};

public:
    StoreInt64() = default;

    StoreInt64(std::int64_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreInt64();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asInt64();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isInt64() const override
    {
        return true;
    }

    bool asBool() const override
    {
        return x != 0;
    }

    std::int8_t asInt8() const override
    {
        return static_cast<std::int8_t>(x);
    }

    std::int16_t asInt16() const override
    {
        return static_cast<std::int16_t>(x);
    }

    std::int32_t asInt32() const override
    {
        return static_cast<std::int32_t>(x);
    }

    std::int64_t asInt64() const override
    {
        return x;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return static_cast<yarp::conf::float32_t>(x);
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return static_cast<yarp::conf::float64_t>(x);
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return static_cast<yarp::conf::vocab32_t>(x);
    }
};

/**
 * A 32-bit floating point number item.
 */
class YARP_os_impl_API StoreFloat32 :
        public Storable
{
private:
    yarp::conf::float32_t x{0.0f};

public:
    StoreFloat32() = default;

    StoreFloat32(yarp::conf::float32_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreFloat32();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asFloat32();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isFloat32() const override
    {
        return true;
    }

    std::int8_t asInt8() const override
    {
        return static_cast<std::int8_t>(x);
    }

    std::int16_t asInt16() const override
    {
        return static_cast<std::int16_t>(x);
    }

    std::int32_t asInt32() const override
    {
        return static_cast<std::int32_t>(x);
    }

    std::int64_t asInt64() const override
    {
        return static_cast<std::int64_t>(x);
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return x;
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }
};

/**
 * A 64-bit floating point number item.
 */
class YARP_os_impl_API StoreFloat64 :
        public Storable
{
private:
    yarp::conf::float64_t x{0.0};

public:
    StoreFloat64() = default;

    StoreFloat64(yarp::conf::float64_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreFloat64();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asFloat64();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isFloat64() const override
    {
        return true;
    }

    std::int8_t asInt8() const override
    {
        return static_cast<std::int8_t>(x);
    }

    std::int16_t asInt16() const override
    {
        return static_cast<std::int16_t>(x);
    }

    std::int32_t asInt32() const override
    {
        return static_cast<std::int32_t>(x);
    }

    std::int64_t asInt64() const override
    {
        return static_cast<std::int64_t>(x);
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return static_cast<yarp::conf::float32_t>(x);
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }
};

/**
 * A vocabulary item.
 */
class YARP_os_impl_API StoreVocab32 :
        public Storable
{
    yarp::conf::vocab32_t x{0};

public:
    StoreVocab32() = default;

    StoreVocab32(yarp::conf::vocab32_t x) :
            x(x)
    {
    }

    Storable* createStorable() const override
    {
        return new StoreVocab32();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asVocab32();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;
    std::string toStringNested() const override;
    void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isBool() const override
    {
        return (x == 0 || x == '1');
    }

    bool asBool() const override
    {
        return x != 0;
    }

    std::int32_t asInt32() const override
    {
        return x;
    }

    std::int64_t asInt64() const override
    {
        return x;
    }

    yarp::conf::float32_t asFloat32() const override
    {
        return static_cast<yarp::conf::float32_t>(x);
    }

    yarp::conf::float64_t asFloat64() const override
    {
        return x;
    }

    bool isVocab32() const override
    {
        return true;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return x;
    }

    std::string asString() const override
    {
        return toString();
    }
};

/**
 * A string item.
 */
class YARP_os_impl_API StoreString :
        public Storable
{
private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) x{};

public:
    StoreString() = default;

    StoreString(const std::string& x)
    {
        this->x = x;
    }

    Storable* createStorable() const override
    {
        return new StoreString();
    }

    void copy(const Storable& alt) override
    {
        x = alt.asString();
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;
    std::string toStringNested() const override;
    void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isString() const override
    {
        return true;
    }

    std::string asString() const override
    {
        return x;
    }

    yarp::conf::vocab32_t asVocab32() const override
    {
        return yarp::os::Vocab32::encode(x);
    }

    // Quote and escape a string for printing it nested
    static std::string quotedString(const std::string& x);
};

/**
 * A binary blob item.
 */
class YARP_os_impl_API StoreBlob :
        public Storable
{
private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) x{};

public:
    StoreBlob() = default;

    StoreBlob(const std::string& x)
    {
        this->x = x;
    }

    Storable* createStorable() const override
    {
        return new StoreBlob();
    }

    void copy(const Storable& alt) override
    {
        if (alt.isBlob()) {
            std::string tmp((char*)alt.asBlob(), alt.asBlobLength());
            x = tmp;
        } else {
            x = std::string();
        }
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;
    std::string toStringNested() const override;
    void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isBlob() const override
    {
        return true;
    }

    const char* asBlob() const override
    {
        return x.c_str();
    }

    size_t asBlobLength() const override
    {
        return x.length();
    }
};


/**
 * A nested list of items.
 */
class YARP_os_impl_API StoreList :
        public Storable
{
private:
    yarp::os::Bottle content{};

public:
    StoreList() = default;

    Storable* createStorable() const override
    {
        return new StoreList();
    }

    void copy(const Storable& alt) override
    {
        content = *(alt.asList());
    }

    yarp::os::Bottle& internal()
    {
        return content;
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;
    std::string toStringNested() const override;
    void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code + subCode();
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isList() const override
    {
        return true;
    }

    yarp::os::Bottle* asList() const override
    {
        return (yarp::os::Bottle*)(&content);
    }

    std::int32_t subCode() const override;

    yarp::os::Value& find(const std::string& key) const override
    {
        return content.find(key);
    }

    yarp::os::Bottle& findGroup(const std::string& key) const override
    {
        return content.findGroup(key);
    }
};


/**
 * Key/value pairs
 */
class YARP_os_impl_API StoreDict :
        public Storable
{
private:
    yarp::os::Property content{};

public:
    StoreDict() = default;

    Storable* createStorable() const override
    {
        return new StoreDict();
    }

    void copy(const Storable& alt) override
    {
        content = *(alt.asDict());
    }

    yarp::os::Property& internal()
    {
        return content;
    }

    std::string toString() const override;
    void fromString(const std::string& src) override;
    std::string toStringNested() const override;
    void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    std::int32_t getCode() const override
    {
        return code;
    }

    bool readRaw(ConnectionReader& reader) override;
    bool writeRaw(ConnectionWriter& writer) const override;

    bool isDict() const override
    {
        return true;
    }

    yarp::os::Property* asDict() const override
    {
        return const_cast<yarp::os::Property*>(&content);
    }

    yarp::os::Value& find(const std::string& key) const override
    {
        return content.find(key);
    }

    yarp::os::Bottle& findGroup(const std::string& key) const override
    {
        return content.findGroup(key);
    }
};


template <typename T>
inline std::int32_t subCoder(T& content)
{
    std::int32_t c = -1;
    bool ok = false;
    for (unsigned int i = 0; i < content.size(); ++i) {
        std::int32_t sc = content.get(i).getCode();
        if (c == -1) {
            c = sc;
            ok = true;
        }
        if (sc != c) {
            ok = false;
        }
    }
    // just optimize primitive types
    if ((c & GROUP_MASK) != 0) {
        ok = false;
    }
    c = ok ? c : 0;
    content.specialize(c);
    return c;
}

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_STORABLE_H
