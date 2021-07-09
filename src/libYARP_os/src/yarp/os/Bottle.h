/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_BOTTLE_H
#define YARP_OS_BOTTLE_H

#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <string>

#define BOTTLE_TAG_INT8 32         // 0000 0000 0010 0000
#define BOTTLE_TAG_INT16 64        // 0000 0000 0100 0000
#define BOTTLE_TAG_INT32 1         // 0000 0000 0000 0001
#define BOTTLE_TAG_INT64 (1 + 16)  // 0000 0000 0001 0001
#define BOTTLE_TAG_VOCAB32 (1 + 8) // 0000 0000 0000 1001
#define BOTTLE_TAG_FLOAT32 128     // 0000 0000 1000 0000
#define BOTTLE_TAG_FLOAT64 (2 + 8) // 0000 0000 0000 1010
#define BOTTLE_TAG_STRING (4)      // 0000 0000 0000 0100
#define BOTTLE_TAG_BLOB (4 + 8)    // 0000 0000 0000 1100
#define BOTTLE_TAG_LIST 256        // 0000 0001 0000 0000
#define BOTTLE_TAG_DICT 512        // 0000 0010 0000 0000

YARP_DEPRECATED_MSG("Use BOTTLE_TAG_INT32 instead") // Since YARP 3.5.0
constexpr std::int32_t BOTTLE_TAG_DOUBLE = BOTTLE_TAG_FLOAT64;

YARP_DEPRECATED_MSG("Use BOTTLE_TAG_FLOAT64 instead") // Since YARP 3.5.0
constexpr std::int32_t BOTTLE_TAG_INT = BOTTLE_TAG_INT32;

YARP_DEPRECATED_MSG("Use BOTTLE_TAG_VOCAB32 instead") // Since YARP 3.5.0
constexpr std::int32_t BOTTLE_TAG_VOCAB = BOTTLE_TAG_VOCAB32;

namespace yarp {
namespace os {

class NetworkBase;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace impl {
class BottleImpl;
class Storable;
class StoreList;
} // namespace impl
#endif // DOXYGEN_SHOULD_SKIP_THIS


/**
 * \ingroup key_class
 *
 * \brief A simple collection of objects that can be described and transmitted
 * in a portable way.
 *
 * Objects are stored in a list, which you can add to and access.
 * It is handy to use until you feel the need to make your own more
 * efficient formats for transmission.
 *
 * Here's an example of using a Bottle:
 * \include example/os/bottle_add/bottle_add.cpp
 *
 * This class has a well-defined, documented representation in both
 * binary and text form.  The name of this class comes from the idea
 * of throwing a "message in a bottle" into the network and hoping it
 * will eventually wash ashore somewhere else.  In the very early days
 * of YARP, that is what communication felt like.
 */
class YARP_os_API Bottle : public Portable, public Searchable
{
public:
#if defined(SWIG) && (SWIG_VERSION < 0x300011)
    typedef size_t size_type;
#else
    using size_type = size_t;
#endif
    // FIXME this can be constexpr, but swig 3.0.8 is not happy
    static const size_type npos;

    using Searchable::check;
    using Searchable::findGroup;

    /**
     * Constructor.  The bottle is initially empty.
     */
    Bottle();

    /**
     * Creates a bottle from a string.
     *
     * The string which should contain a textual form of the bottle,
     * e.g. text = "10 -5.3 1.0 \"hello there\"" would give a bottle
     * with 4 elements: an integer, two floating point numbers, and a
     * string.
     *
     * @param text the textual form of the bottle to be interpreted.
     */
    explicit Bottle(const std::string& text);

    /**
     * Copy constructor.
     *
     * @param rhs The object to copy.
     */
    Bottle(const Bottle& rhs);

    /**
     * Move constructor.
     *
     * @param rhs The object to move.
     */
    Bottle(Bottle&& rhs) noexcept;

    /**
     * @brief Initializer list constructor.
     * @param[in] values, list of Value with which initialize the Bottle.
     */
    Bottle(std::initializer_list<yarp::os::Value> values);

    /**
     * Copy assignment operator.
     *
     * @param rhs The object to copy.
     * @return the Bottle itself.
     */
    Bottle& operator=(const Bottle& rhs);

    /**
     * Move assignment operator.
     *
     * @param rhs The object to move.
     * @return the Bottle itself.
     */
    Bottle& operator=(Bottle&& rhs) noexcept;

    /**
     * Destructor.
     */
    ~Bottle() override;

    /**
     * Empties the bottle of any objects it contains.
     */
    void clear();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
    /**
     * Places an integer in the bottle, at the end of the list.
     *
     * @param x the integer to add.
     * @warning Unsafe, sizeof(int) is platform dependent. Use addInt32 instead.
     * @deprecated Since YARP 3.5.0. Use addInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use addInt32 instead")
    inline void addInt(int x)
    {
        addInt32(static_cast<std::int32_t>(x));
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Places a 8-bit integer in the bottle, at the end of the list.
     *
     * @param x the 8-bit integer to add.
     */
    void addInt8(std::int8_t x);

    /**
     * Places a 16-bit integer in the bottle, at the end of the list.
     *
     * @param x the 16-bit integer to add.
     */
    void addInt16(std::int16_t x);

    /**
     * Places a 32-bit integer in the bottle, at the end of the list.
     *
     * @param x the 32-bit integer to add.
     */
    void addInt32(std::int32_t x);

    /**
     * Places a 64-bit integer in the bottle, at the end of the list.
     *
     * @param x the 64-bit integer to add.
     */
    void addInt64(std::int64_t x);

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
    /**
     * Places a vocabulary item in the bottle, at the end of the list.
     *
     * @param x the item to add.
     * @deprecated Since YARP 3.5.0. Use addVocab32 instead.
     */
    YARP_DEPRECATED_MSG("Use addVocab32 instead")
    void addVocab(yarp::conf::vocab32_t x)
    {
        return addVocab32(x);
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Places a vocabulary item in the bottle, at the end of the list.
     *
     * @param x the item to add.
     */
    void addVocab32(yarp::conf::vocab32_t x);

    /**
     * Places a vocabulary item in the bottle, at the end of the list.
     *
     * @param a first character of the vocab
     * @param b second character of the vocab
     * @param c third character of the vocab
     * @param d fourth character of the vocab
     */
    void addVocab32(char a, char b = 0, char c = 0, char d = 0)
    {
        addVocab32(yarp::os::createVocab32(a, b, c, d));
    }

    /**
     * Places a vocabulary item in the bottle, at the end of the list.
     * If the string is longer than 4 characters, only the first 4 are used.
     * @param str The string to encode and add.
     */
    void addVocab32(const std::string& str)
    {
        addVocab32(yarp::os::Vocab32::encode(str));
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
    /**
     * Places a floating point number in the bottle, at the end of the
     * list.
     *
     * @param x the number to add.
     * @warning Unsafe, sizeof(double) is platform dependent. Use addFloat64 instead.
     * @deprecated Since YARP 3.5.0. Use addFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use addFloat64 instead")
    inline void addDouble(double x)
    {
        addFloat64(static_cast<yarp::conf::float64_t>(x));
    }
#endif // YARP_NO_DEPRECATED

    /**
     * Places a 32-bit floating point number in the bottle, at the end of the
     * list.
     *
     * @param x the number to add.
     */
    void addFloat32(yarp::conf::float32_t x);

    /**
     * Places a 64-bit floating point number in the bottle, at the end of the
     * list.
     *
     * @param x the number to add.
     */
    void addFloat64(yarp::conf::float64_t x);

    /**
     * Places a string in the bottle, at the end of the list.
     *
     * @param str the string to add.
     */
    void addString(const char* str);

    /**
     * Places a string in the bottle, at the end of the list.
     *
     * @param str the string to add.
     */
    void addString(const std::string& str);

    /**
     * Add a Value to the bottle, at the end of the list.
     *
     * @param value the Value to add.
     */
    void add(const Value& value);

    /**
     * Add a Value to the bottle, at the end of the list.
     *
     * The object passed will be placed directly into the list, without
     * copying.  The Bottle will be responsible for deallocating it
     * when appropriate.
     *
     * @param value the Value to add.
     */
    void add(Value* value);

    /**
     * Places an empty nested list in the bottle, at the end of the
     * list.
     *
     * The list itself is represented as a bottle.
     *
     * @return a reference to the newly added list.
     */
    Bottle& addList();

    /**
     * Places an empty key/value object in the bottle, at the end of the
     * list.
     *
     * @return a reference to the newly added property.
     */
    Property& addDict();

    /**
     * Removes a Value v from the end of the list and returns this
     * value.
     *
     * @return the value popped from the end of the bottle.
     */
    Value pop();

    /**
     * Reads a Value v from a certain part of the list.
     *
     * Methods like v.isInt32() or v.isString() can be used to check the
     * type of the result.
     * Methods like v.asInt32() or v.asString() can be used to access the
     * result as a particular type.
     *
     * @param index the part of the list to read from.
     * @return the Value v; if the index lies outside the range of
     *         elements present, then v.isNull() will be true.
     */
    Value& get(size_type index) const;

    /**
     * Gets the number of elements in the bottle.
     *
     * @return number of elements in the bottle.
     */
    size_type size() const;

    /**
     * Initializes bottle from a string.
     *
     * The string should contain a textual form of the bottle,
     * e.g. text = "10 -5.3 1.0 \"hello there\"" would give a bottle
     * with 4 elements: an integer, two floating point numbers, and a
     * string.
     *
     * @param text the textual form of the bottle to be interpreted.
     */
    void fromString(const std::string& text);

    /**
     * Initializes bottle from a binary representation.
     *
     * @param buf the binary form of the bottle to be interpreted.
     * @param len the length of the binary form.
     */
    void fromBinary(const char* buf, size_t len);

    /**
     * Returns binary representation of bottle.
     *
     * Lifetime is until next modification of bottle.
     *
     * @param size if non-null, the referenced variable is set to the
     *             buffer size.
     * @return pointer to a buffer holding the binary representation of
     *         the bottle.
     */
    const char* toBinary(size_t* size = nullptr);

    /**
     * Gives a human-readable textual representation of the bottle.
     *
     * This representation is suitable for passing to Bottle::fromString
     * (see that method for examples).
     *
     * @return a textual representation of the bottle.
     */
    std::string toString() const override;

    /**
     * Output a representation of the bottle to a network connection.
     *
     * @param writer the interface to the network connection for writing
     * @result true iff the representation was written successfully.
     */
    bool write(ConnectionWriter& writer) const override;

    /**
     * Set the bottle's value based on input from a network connection.
     *
     * @param reader the interface to the network connection for reading
     * @return true iff the bottle was read successfully.
     */
    bool read(ConnectionReader& reader) override;

    /**
     * Copy the bottle's value to a object that can read a
     * serialization.
     *
     * Must be serialized in standard Bottle-compatible format.
     *
     * @param reader the serializable object.
     * @param textMode true if text serialization should be used.
     * @return true iff the bottle was written successfully.
     */
    bool write(PortReader& reader, bool textMode = false);

    /**
     * Set the bottle's value based on input from a serializable object.
     *
     * Must be serialized in standard Bottle-compatible format.
     *
     * @param writer the serializable object.
     * @param textMode true if text serialization should be used.
     * @return true iff the bottle was read successfully.
     */
    bool read(const PortWriter& writer, bool textMode = false);

    void onCommencement() const override;

    bool check(const std::string& key) const override;

    Value& find(const std::string& key) const override;

    Bottle& findGroup(const std::string& key) const override;

    bool isNull() const override;

    /**
     * Copy all or part of another Bottle.
     *
     * @param alt The object to copy.
     * @param first The index of the first element to copy.
     * @param len The number of elements to copy (-1 for all).
     */
    void copy(const Bottle& alt, size_type first = 0, size_type len = npos);

    /**
     * A special Bottle with no content.
     *
     * @return the special invalid "null" Bottle.
     */
    static Bottle& getNullBottle();

    /**
     * Equality test.
     *
     * @param alt the value to compare against.
     * @result true iff the values are equal.
     */
    bool operator==(const Bottle& alt) const;

    /**
     * Inequality test.
     *
     * @param alt the value to compare against.
     * @result true iff the values are not equal.
     */
    bool operator!=(const Bottle& alt) const;

    /**
     * Append the content of the given bottle to the current
     * list.
     *
     * @param alt the content to add.
     */
    void append(const Bottle& alt);

    /**
     * Get all but the first element of a bottle.
     *
     * @return a bottle containing all but the first element of the
     * current bottle.
     */
    Bottle tail() const;

    /**
     * Declare that the content of the Bottle has been changed.
     *
     * It is important to call this if you modify an individual element
     * of the Bottle through assignment, so that serialization happens
     * correctly.
     */
    void hasChanged();

    static std::string toString(int x);

    /**
     * Get numeric bottle code for this bottle.
     *
     * @return the numeric code
     */
    int getSpecialization();

    /**
     *
     * Convert a numeric bottle code to a string.
     * @param code the code to convert
     * @return a string representation of the code's meaning
     *
     */
    static std::string describeBottleCode(int code);


protected:
    void setReadOnly(bool readOnly);


private:
    friend class yarp::os::NetworkBase;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    friend class yarp::os::impl::Storable;
    friend class yarp::os::impl::StoreList;
    friend class yarp::os::impl::BottleImpl;
    yarp::os::impl::BottleImpl* implementation;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_BOTTLE_H
