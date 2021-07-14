/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_VECTOR_H
#define YARP_SIG_VECTOR_H

#include <cstring>
#include <cstddef> //defines size_t
#include <memory>
#include <string>
#include <vector>

#include <yarp/os/Portable.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Type.h>

#include <yarp/sig/api.h>
#include <yarp/os/Log.h>

/**
* \file Vector.h contains the definition of a Vector type
*/
namespace yarp {
namespace sig {

class VectorBase;
template<class T> class VectorOf;
// Swig(3.0.12) crashes when generating
// ruby bindings without these guards.
// Bindings for Vector are generated
// anyways throught the %template directive
// in the interface file.
#ifndef SWIG
typedef VectorOf<double> Vector;
#endif

} // namespace sig
} // namespace yarp


/**
* \ingroup sig_class
*
* A Base class for a VectorOf<T>, provide default implementation for
* read/write methods. Warning: the current implementation assumes the same
* representation for data type (endianness).
*/
class YARP_sig_API yarp::sig::VectorBase : public yarp::os::Portable
{
public:
    virtual size_t getElementSize() const = 0;
    virtual int getBottleTag() const = 0;

    virtual size_t getListSize() const = 0;
    virtual const char *getMemoryBlock() const = 0;
    virtual char *getMemoryBlock() = 0;
    virtual void resize(size_t size) = 0;

    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    bool write(yarp::os::ConnectionWriter& connection) const override;

protected:
    virtual std::string getFormatStr(int tag) const;

};

/*
* This is a simple function that maps a type into its corresponding BOTTLE tag.
* Used for bottle compatible serialization, called inside getBottleTag().
* Needs to be instantiated for each type T used in VectorOf<T>.
*/
template<class T>
inline int BottleTagMap () {
    /* make sure this is never called unspecified */
    yAssert(0);
    return 0;
  }

template<>
inline int BottleTagMap <double> () {
    return BOTTLE_TAG_FLOAT64;
  }

template<>
inline int BottleTagMap <int> () {
    return BOTTLE_TAG_INT32;
  }

/**
* \ingroup sig_class
*
* Provides:
* - push_back(), pop_back() to add/remove an element at the end of the vector
* - resize(), to create an array of elements
* - clear(), to clean the array (remove all elements)
* - use [] to access single elements without range checking
* - use size() to get the current size of the Vector
* - use operator= to copy Vectors
* - read/write network methods
* Warning: the class is designed to work with simple types (i.e. types
* that do not allocate internal memory). Template instantiation needs to
* be checked to avoid unresolved externals. Network communication assumes
* same data representation (endianness) between machines.
*/
template<class T>
class yarp::sig::VectorOf : public VectorBase
{
private:
    std::vector<T> bytes;

public:
    using iterator       =  typename std::vector<T>::iterator;
    using const_iterator =  typename std::vector<T>::const_iterator;

    VectorOf() = default;

    VectorOf(size_t size) : bytes(size) {
    }

    /**
     * @brief Initializer list constructor.
     * @param[in] values, list of values with which initialize the Vector.
     */
    VectorOf(std::initializer_list<T> values) : bytes(values) {
    }

    /**
    * Build a vector and initialize it with def.
    * @param s the size
    * @param def a default value used to fill the vector
    */
    VectorOf(size_t s, const T& def) : bytes(s, def) {
    }

    /**
    * Builds a vector and initialize it with
    * values from 'p'. Copies memory.
    * @param s the size of the data to be copied
    * @param T* the pointer to the data
    */
    VectorOf(size_t s, const T *p)
    {
        this->resize(s);
        memcpy(this->data(), p, sizeof(T)*s);
    }

    VectorOf(const VectorOf& r) = default;
    VectorOf<T> &operator=(const VectorOf<T>& r) = default;
    VectorOf(VectorOf<T>&& other) noexcept = default;
    VectorOf& operator=(VectorOf<T>&& other) noexcept = default;
    ~VectorOf() override = default;

    size_t getElementSize() const override {
        return sizeof(T);
    }

    int getBottleTag() const override {
        return BottleTagMap <T>();
    }

    size_t getListSize() const override
    {
        return bytes.size();
    }

    const char* getMemoryBlock() const override
    {
        return reinterpret_cast<const char*>(this->data());
    }

    char* getMemoryBlock() override
    {
        return reinterpret_cast<char*>(this->data());
    }
#ifndef YARP_NO_DEPRECATED // since YARP 3.2.0
    YARP_DEPRECATED_MSG("Use either data() if you need the pointer to the first element,"
                        " or cbegin() if you need the iterator")
    inline const T *getFirst() const
    {
        return this->data();
    }

    YARP_DEPRECATED_MSG("Use either data() if you need the pointer to the first element,"
                        " or begin() if you need the iterator")
    inline T *getFirst()
    {
        return this->data();
    }
#endif // YARP_NO_DEPRECATED

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double (or nullptr if the vector is of zero length)
    */
    inline T *data()
    { return bytes.empty() ? nullptr : &(bytes.at(0)); }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double (or nullptr if the vector is of zero length)
    */
    inline const T *data() const
    { return bytes.empty() ? nullptr : &(bytes.at(0)); }

    /**
    * Resize the vector.
    * @param s the new size
    */
    void resize(size_t size) override
    {
        bytes.resize(size);
    }

    /**
    * Resize the vector and initilize the element to a default value.
    * @param s the new size
    * @param def the default value
    */
    void resize(size_t size, const T&def)
    {
        this->resize(size);
        std::fill(bytes.begin(), bytes.end(), def);
    }

    /**
     * @brief reserve, increase the capacity of the vector to a value that's greater or equal to size.
     * If size is greater than the current capacity(), new storage is allocated, otherwise the method does nothing.
     * @param size, new size of the vector.
     */
    void reserve(size_t size) {
        bytes.reserve(size);
    }

    /**
    * Push a new element in the vector: size is changed
    */
    inline void push_back (const T &elem)
    {
        bytes.push_back(elem);
    }

    /**
     * @brief Move a new element in the vector: size is changed
     * @param elem, element to be moved.
     */
    inline void push_back (T&& elem)
    {
        bytes.push_back(std::move(elem));
    }

    /**
     * @brief Construct a new element in the vector: size is changed
     * @param args, arguments to be forwarded for constructing the new element.
     * @return the reference to the new element contructed.
     */
    template<typename... _Args>
    inline T& emplace_back(_Args&&... args)
    {
        bytes.emplace_back(std::forward<_Args>(args)...);
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    inline void pop_back()
    {
        bytes.pop_back();
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator[](size_t i)
    {
        return bytes[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator[](size_t i) const
    {
        return bytes[i];
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator()(size_t i)
    {
        return this->data()[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator()(size_t i) const
    {
        return this->data()[i];
    }

    inline size_t size() const {
        return bytes.size();
    }

    /**
    * Get the length of the vector.
    * @return the length of the vector.
    */
    inline size_t length() const
    { return this->size();}

    /**
     * @brief capacity
     * @return the number of elements that the container has currently allocated space for.
     */
    inline size_t capacity() const {
        return bytes.capacity();
    }

    /**
    * Zero the elements of the vector.
    */
    void zero()
    {
        std::fill(bytes.begin(), bytes.end(), 0);
    }

    /**
    * Creates a string object containing a text representation of the object. Useful for printing.
    * To get a nice format the optional parameters precision and width may be used (same meaning as in printf and cout).
    * @param precision the number of digits to be printed after the decimal point.
    * @param width minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is never truncated.
    * If width is specified the inter-value separator is a blank space, otherwise it is a tab.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    */
    std::string toString(int precision=-1, int width=-1) const
    {
        std::string ret = "";
        size_t c = 0;
        const size_t buffSize = 256;
        char tmp[buffSize];
        std::string formatStr;
        if (getBottleTag() == BOTTLE_TAG_FLOAT64) {
            if (width<0) {
                formatStr = "% .*lf\t";
                for (c=0;c<length();c++) {
                    snprintf(tmp, buffSize, formatStr.c_str(), precision, (*this)[c]);
                    ret+=tmp;
                }
            }
            else{
                formatStr = "% *.*lf ";
                for (c=0;c<length();c++){
                    snprintf(tmp, buffSize, formatStr.c_str(), width, precision, (*this)[c]);
                    ret+=tmp;
                }
            }
        }
        else {
            formatStr = "%" + getFormatStr(getBottleTag()) + " ";
            for (c=0;c<length();c++) {
                snprintf(tmp, buffSize, formatStr.c_str(), (*this)[c]);
                ret+=tmp;
            }
        }

        if (length() >= 1) {
            return ret.substr(0, ret.length() - 1);
        }
        return ret;
    }

    /**
    * Creates and returns a new vector, being the portion of the original
    * vector defined by the first and last indexes of the items to be included
    * in the subvector. The indexes are checked: if wrong, a null vector is
    * returned.
    */
    VectorOf<T> subVector(unsigned int first, unsigned int last) const
    {
        VectorOf<T> ret;
        if ((first<=last)&&((int)last<(int)this->size()))
        {
            ret.resize(last-first+1);
            for (unsigned int k = first; k <= last; k++) {
                ret[k - first] = (*this)[k];
            }
        }
        return ret;
    }

    /**
     * Set a portion of this vector with the values of the specified vector.
     * If the specified vector v is to big the method does not resize the vector,
     * but return false.
     *
     * @param position index of the first value to set
     * @param v vector containing the values to set
     * @return true if the operation succeeded, false otherwise
     */
    bool setSubvector(int position, const VectorOf<T> &v)
    {
        if (position + v.size() > this->size()) {
            return false;
        }
        for (size_t i = 0; i < v.size(); i++) {
            (*this)[position + i] = v(i);
        }
        return true;
    }

    /**
    * Set all elements of the vector to a scalar.
    */
    const VectorOf<T> &operator=(T v)
    {
        std::fill(bytes.begin(), bytes.end(), v);
        return *this;
    }

    /**
    * True iff all elements of 'a' match all element of 'b'.
    */
    bool operator==(const VectorOf<T> &r) const
    {
        return bytes == r.bytes;
    }

    /**
     * @brief Returns an iterator to the beginning of the VectorOf
     */
    iterator begin() noexcept {
        return bytes.begin();
    }

    /**
     * @brief Returns an iterator to the end of the VectorOf
     */
    iterator end() noexcept {
        return bytes.end();
    }

    /**
     * @brief Returns a const iterator to the beginning of the VectorOf
     */
    const_iterator begin() const noexcept {
        return bytes.begin();
    }

    /**
     * @brief Returns a const iterator to the end of the VectorOf.
     */
    const_iterator end() const noexcept {
        return bytes.end();
    }

    /**
     * @brief Returns a const iterator to the beginning of the VectorOf
     */
    const_iterator cbegin() const noexcept {
        return bytes.cbegin();
    }

    /**
     * @brief Returns a const iterator to the end of the VectorOf.
     */
    const_iterator cend() const noexcept {
        return bytes.cend();
    }
    void clear() {
        bytes.clear();
    }

    yarp::os::Type getType() const override {
        return yarp::os::Type::byName("yarp/vector");
    }
};


#ifdef _MSC_VER
/*YARP_sig_EXTERN*/ template class YARP_sig_API yarp::sig::VectorOf<double>;
#endif

#endif // YARP_SIG_VECTOR_H
