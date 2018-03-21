/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_VECTOR_H
#define YARP_SIG_VECTOR_H

//#include <cstdlib> //defines size_t
#include <cstddef> //defines size_t
#include <yarp/os/Portable.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/ManagedBytes.h>

#include <yarp/sig/api.h>
#include <yarp/os/Log.h>

/**
* \file Vector.h contains the definition of a Vector type
*/
namespace yarp {

    namespace sig {
        class VectorBase;
        class Vector;
        template<class T> class VectorOf;
    }
}


/**
* \ingroup sig_class
*
* A Base class for a VectorOf<T>, provide default implementation for
* read/write methods. Warning: the current implementation assumes the same
* representation for data type (endianess).
*/
class YARP_sig_API yarp::sig::VectorBase : public yarp::os::Portable
{
public:
    virtual int getElementSize() const = 0;
    virtual int getBottleTag() const = 0;

    virtual size_t getListSize() const = 0;
    virtual const char *getMemoryBlock() const = 0;
    virtual void resize(size_t size) = 0;

    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection) override;
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
    return BOTTLE_TAG_DOUBLE;
  }

template<>
inline int BottleTagMap <int> () {
    return BOTTLE_TAG_INT;
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
* same data representation (endianess) between machines.
*/
template<class T>
class yarp::sig::VectorOf : public VectorBase
{
private:
    yarp::os::ManagedBytes bytes;
    T *first;
    size_t len;

    inline void _updatePointers() {
        len = bytes.used()/sizeof(T);
        if (len==0) {
            first = nullptr;
        } else {
            first = reinterpret_cast<T*>(bytes.get());
        }
    }

public:
    VectorOf() {
        bytes.allocate(16*sizeof(T)); // preallocate space for 16 elements
        bytes.setUsed(0);
        first = nullptr;
        len = 0;
    }

    VectorOf(size_t size) : bytes(size*sizeof(T)) {
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
    }

    VectorOf(const VectorOf &r) : VectorBase() {
        bytes = r.bytes;
        _updatePointers();
    }

    const VectorOf<T> &operator=(const VectorOf<T> &r) {
        bytes = r.bytes;
        _updatePointers();
        return *this;
    }

    virtual int getElementSize() const override {
        return sizeof(T);
    }

    virtual int getBottleTag() const override {
        return BottleTagMap <T>();
    }

    virtual size_t getListSize() const override
    {
        return len;
    }

    virtual const char *getMemoryBlock() const override
    {
        return (char *) bytes.get();
    }

    inline const T *getFirst() const
    {
        return first;
    }

    inline T *getFirst()
    {
        return first;
    }

    virtual void resize(size_t size) override
    {
        size_t prev_len = len;
        bytes.allocateOnNeed(size*sizeof(T),size*sizeof(T));
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i = prev_len; i < size; i++) {
            new (&((*this)[i])) T(); // non-allocating placement operator new
        }
    }

    void resize(size_t size, const T&def)
    {
        bytes.allocateOnNeed(size*sizeof(T),size*sizeof(T));
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i = 0; i < size; i++) {
            new (&((*this)[i])) T(def); // non-allocating placement operator new
        }
    }

    inline void push_back (const T &elem)
    {
        bytes.allocateOnNeed(bytes.used()+sizeof(T),bytes.length()*2+sizeof(T));
        bytes.setUsed(bytes.used()+sizeof(T));
        _updatePointers();
        first[len-1] = elem;
    }

    inline void pop_back (void)
    {
        if (bytes.used()>sizeof(T)) {
            bytes.setUsed(bytes.used()-sizeof(T));
            len--;
            _updatePointers();
        }
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator[](size_t i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator[](size_t i) const
    {
        return first[i];
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator()(size_t i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator()(size_t i) const
    {
        return first[i];
    }

    inline size_t size() const {
        return len;
    }

    void clear() {
        bytes.clear();
        bytes.setUsed(0);
        len = 0;
        first = nullptr;
    }
};


#ifdef _MSC_VER
/*YARP_sig_EXTERN*/ template class YARP_sig_API yarp::sig::VectorOf<double>;
#endif

/**
* \ingroup sig_class
*
* A class for a Vector. A Vector can be sent/read to/from
* a port. Use the [] and () operator for single element
* access.
*/
class YARP_sig_API yarp::sig::Vector : public yarp::os::Portable
{
    VectorOf<double> storage;

public:
    Vector()
    {}

    explicit Vector(size_t s):storage(s)
    {}

    /**
    * Build a vector and initialize it with def.
    * @param s the size
    * @param def a default value used to fill the vector
    */
    explicit Vector(size_t s, const double &def)
    {
        storage.resize(s,def);
    }

    ~Vector()
    {}

    /**
    * Builds a vector and initialize it with
    * values from 'p'. Copies memory.
    */
    Vector(size_t s, const double *p);

    /**
    * Copy constructor.
    */
    Vector(const Vector &r): yarp::os::Portable(), storage(r.storage)
    {}

    /**
    * Copy operator;
    */
    const Vector &operator=(const Vector &r);

    /**
    * Resize the vector.
    * @param s the new size
    */
    void resize(size_t s)
    {
        storage.resize(s);
    }

    /**
    * Resize the vector, if the vector is not empty preserve old content.
    * @param size the new size
    * @param def a default value used to fill the vector
    */
    void resize(size_t size, const double &def)
    {
        storage.resize(size, def);
    }

    inline size_t size() const
    { return storage.size();}

    /**
    * Get the length of the vector.
    * @return the length of the vector.
    */
    inline size_t length() const
    { return storage.size();}

    /**
    * Zero the elements of the vector.
    */
    void zero();

    /**
    * Creates a string object containing a text representation of the object. Useful for printing.
    * To get a nice format the optional parameters precision and width may be used (same meaning as in printf and cout).
    * @param precision the number of digits to be printed after the decimal point.
    * @param width minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is never truncated.
    * If width is specified the inter-value separator is a blank space, otherwise it is a tab.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    */
    yarp::os::ConstString toString(int precision=-1, int width=-1) const;


    /**
    * Creates and returns a new vector, being the portion of the original
    * vector defined by the first and last indexes of the items to be included
    * in the subvector. The indexes are checked: if wrong, a null vector is
    * returned.
    */
    Vector subVector(unsigned int first, unsigned int last) const;

    /**
     * Set a portion of this vector with the values of the specified vector.
     * If the specified vector v is to big the method does not resize the vector,
     * but return false.
     *
     * @param position index of the first value to set
     * @param v vector containing the values to set
     * @return true if the operation succeeded, false otherwise
     */
    bool setSubvector(int position, const Vector &v);

    /**
    * Set all elements of the vector to a scalar.
    */
    const Vector &operator=(double v);

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double (or nullptr if the vector is of zero length)
    */
    inline double *data()
    { return storage.getFirst(); }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double (or nullptr if the vector is of zero length)
    */
    inline const double *data() const
    { return storage.getFirst();}

    /**
    * True iff all elements of 'a' match all element of 'b'.
    */
    bool operator==(const yarp::sig::Vector &r) const;

    /**
    * Push a new element in the vector: size is changed
    */
    inline void push_back (const double &elem)
    {
        storage.push_back(elem);
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    inline void pop_back (void)
    {
        storage.pop_back();
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator[](size_t i)
    {return storage[i];}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator[](size_t i) const
    {return storage[i];}

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator()(size_t i)
    {return storage(i);}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator()(size_t i) const
    {return storage(i);}

    /**
    * Clears out the vector, it does not reallocate
    * the buffer, but just sets the dynamic size to 0.
    */
    void clear ()
    { storage.clear();}

    ///////// Serialization methods
    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection) override;

    virtual yarp::os::Type getType() override {
        return yarp::os::Type::byName("yarp/vector");
    }
};

#endif // YARP_SIG_VECTOR_H
