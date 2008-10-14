// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// $Id: Vector.h,v 1.26 2008-10-14 15:56:49 eshuy Exp $

#ifndef _YARP2_VECTOR_
#define _YARP2_VECTOR_

//#include <stdlib.h> //defines size_t
#include <stddef.h> //defines size_t
#include <yarp/os/Portable.h>
#include <yarp/os/ConstString.h>

/**
* \file Vector.h contains the definition of a Vector type 
*/
namespace yarp {

    namespace sig {
        namespace impl {
            class VectorBase;
            template<class T> class VectorImpl;
            template<class T> class IteratorOf;
        }

        //class Vector
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
class yarp::sig::impl::VectorBase : public yarp::os::Portable
{
public:
    virtual int getElementSize() const =0;
    virtual int getListSize() const =0;
    virtual const char *getMemoryBlock() const =0;
    virtual void resize(size_t size)=0;

    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection);
};

/**
* \ingroup sig_class
*
* A simple Vector class derived from ACE and with Portable functionality.
* Things you can do on Vector:
* - push_back(), pop_back() to add/remove an element at the end of the vector
* - resize(), to create an array of elements
* - clear(), to clean the array (remove all elements)
* - use [] to access single elements without range checking
* - use set()/get() to access single elements with range checking
* - use size() to get the current size of the Vector
* - use operator= to copy Vectors
*/
template<class T> 
class yarp::sig::impl::VectorImpl
{
public:
    /**
    * Default constructor.
    */
    VectorImpl();

    /**
    * Destructor.
    */
    ~VectorImpl();

    /**
    * Create a Vector object of a given size and fill the content.
    * @param size is the number of elements of the vector.
    */
    VectorImpl(size_t size);

    VectorImpl(const VectorImpl &l);

    /**
    * Copies a Vector object into the current one.
    * @param l is a reference to an object of type Vector.
    * @return a reference to the current object.
    */
    const VectorImpl &operator=(const VectorImpl &l);

    const T& operator=(const T&v);

    /**
    * Resize a Vector placing def at each index.
    * @param size is the size of the vector.
    * @param def is the initial value of the vector.
    */
    void resize(size_t size, const T &def);

    /**
    * Access the el-th element of the vector, no range check.
    * @param el index of the element to be accessed.
    * @return a reference to the requested element.
    */
    T &operator[](int el);

    /**
    * Access the el-th element of the vector, const version, 
    * no range check.
    * @param el index of the element to be accessed.
    * @return a reference to the requested element.
    */
    const T &operator[](int el) const;

    /**
    * Return the size of the array (number of elements).
    * @return the current size of the array.
    */
    int size() const;

    /**
    * Push an element at the back of the array.
    * @param elem a reference to the object to be added.
    */
    void push_back (const T& elem);

    /**
    * Remove an element at the end of the vector.
    */
    void pop_back (void);

    /**
    * Set the value of the "slot" element, range check.
    * @param new_item a reference of the new element to set
    * @param slot the position in which the new element should be set
    * @return true/false on success/failure.
    */
    int set (T const &new_item, size_t slot);

    /**
    * Get the value of the "slot" element, range check.
    * @param item return value
    * @param slot the position of the element to get
    * @return true/false on success/failure.
    */
    int get (T &item, size_t slot) const;

    /**
    * Clears out the vector, it does not reallocate
    * the buffer, but just sets the dynamic size to 0.
    */
    void clear ();

    /**
    * Be friendly with your iterator.
    */
    friend class yarp::sig::impl::IteratorOf<T>;

private:
    void *aceVector;
};

/**
* \ingroup sig_class
*
* Derive from VectorImpl<T> to provide:
* - inline, efficient access to elements (operator [])
* - read/write network methods (see VectorBase)
* Warning: the class is designed to work with simple types (i.e. types
* that do not allocate internal memory). Template instantiation needs to 
* be checked to avoid unresolved externals. Network communication assumes
* same data representation (endianess) between machines.
*/
template<class T>
class yarp::sig::VectorOf : public yarp::sig::impl::VectorImpl<T>, 
            public yarp::sig::impl::VectorBase
{
    T *first;
    T *last;

    inline void _updatePointers()
    {
        int lastIndex=yarp::sig::impl::VectorImpl<T>::size()-1;
        if (lastIndex<0)
            lastIndex=0;

        first=&yarp::sig::impl::VectorImpl<T>::operator[](0);
        last=&yarp::sig::impl::VectorImpl<T>::operator[](lastIndex);
    }

public:
    VectorOf():yarp::sig::impl::VectorImpl<T>()
    {}

    VectorOf(size_t size):yarp::sig::impl::VectorImpl<T>(size)
    {
        _updatePointers();
    }

    VectorOf(const VectorOf &r):yarp::sig::impl::VectorImpl<T>(r)
    {
        _updatePointers();
    }

    const VectorOf<T> &operator=(const VectorOf<T> &r)
    {
        yarp::sig::impl::VectorImpl<T>::operator =(r);
        _updatePointers();
        return *this;
    }

    virtual int getElementSize() const
    {
        return sizeof(T);
    }

    virtual int getListSize() const
    {
        return yarp::sig::impl::VectorImpl<T>::size();
    }

    virtual const char *getMemoryBlock() const
    {
        return (char *) first;
    }

    inline const T *getFirst() const
    {
        return first;
    }

    inline T *getFirst()
    {
        return first;
    }

    virtual void resize(size_t size)
    {
        T def;
        yarp::sig::impl::VectorImpl<T>::resize(size, def);
        _updatePointers();
    }

    void resize(size_t size, const T&def)
    {
        yarp::sig::impl::VectorImpl<T>::resize(size, def);
        _updatePointers();
    }

    inline void push_back (const T &elem)
    {
        yarp::sig::impl::VectorImpl<T>::push_back(elem);
        _updatePointers();
    }

    inline void pop_back (void)
    {
        yarp::sig::impl::VectorImpl<T>::pop_back();
        _updatePointers();
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator[](int i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator[](int i) const
    {
        return first[i];
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator()(int i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator()(int i) const
    {
        return first[i];
    }
};

/**
* \ingroup sig_class
* Implement an iterator over a VectorImpl<T>.
* @param i the index of the element to access.
* @return a reference to the requested element.
*/
template<class T>
class yarp::sig::impl::IteratorOf
{
private:
    void *aceVectorIterator;

public:
    /**
    * Constructor.
    */
    IteratorOf(const VectorImpl<T> &l);

    /**
    * Destructor.
    */
    ~IteratorOf();

    /**
    * Get a pointer to the next item that has not been read 
    * in the Array, returns 0 when all items have been seen, else 1.
    */
    int next(T *&);

    /**
    * Move forward by one element in the vector, returns 0 
    * when all the items in the vector have been seen, else 1.
    */
    int advance();

    /**
    * Returns 1 when all items have been seen, else 0.
    */
    int done();
};

/**
* \ingroup sig_class
*
* A class for a Vector. A Vector can be sent/read to/from 
* a port. Use the [] and () operator for single element 
* access.
*/
class yarp::sig::Vector : public yarp::os::Portable
{
    void *gslData;
    VectorOf<double> storage;

    void allocGslData();
    void freeGslData();
    void updateGslData();
    
public:
    Vector()
    {
        allocGslData();
    }

    Vector(size_t s):storage(s)
    {
        allocGslData();
        updateGslData();
    }

    ~Vector()
    {
        freeGslData();
    }

    /**
    * Builds a vector and initialize it with 
    * values from 'p'. Copies memory.
    */
    Vector(size_t s, const double *p);

    /**
    * Copy constructor.
    */
    Vector(const Vector &r): storage(r.storage)
    {
        allocGslData();
        updateGslData();
    }

    /**
    * Copy operator;
    */
    const Vector &operator=(const Vector &r);

    /**
    * Resize the vector, (warning: deprecated, use resize) here
    * to maintain compatibility with the old Vector class.
    * @param s the new size
    */
    void size(size_t s)
    {
        Vector::resize(s);
        updateGslData();
    }

    /**
    * Resize the vector.
    * @param s the new size
    */
    void resize(size_t s)
    {
        storage.resize(s,0.0);
        allocGslData();
        updateGslData();
    }

    /**
    * Resize the vector.
    * @param s the new size
    * @param def a default value used to fill the vector
    */
    void resize(size_t size, const double &def)
    {
        storage.resize(size, def);
        allocGslData();
        updateGslData();
    }

    inline int size() const
    { return storage.size();}

    /**
    * Get the length of the vector.
    * @return the length of the vector.
    */
    inline int length() const
    { return storage.size();}

    /**
    * Zero the elements of the vector.
    */
    void zero();

    /**
    * Creates a string object containing a text 
    * representation of the object. Useful for printing.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    */
    yarp::os::ConstString toString();

    /**
    * Set all elements of the vector to a scalar.
    */
    const Vector &operator=(double v);

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double.
    */
    inline double *data()
    { return storage.getFirst(); }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double.
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
        updateGslData();
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    inline void pop_back (void)
    {
        storage.pop_back();
        updateGslData();
    }

    /**
    * Return a void pointer to a gsl compatible structure.
    * This pointer can be safely cast to a gsl_vector*.
    */
    void *getGslVector();
    
    /**
    * Return a void pointer to a gsl compatible structure.
    * This pointer can be safely cast to a gsl_vector*.
    */
    const void *getGslVector() const;

        /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator[](int i)
    {return storage[i];}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator[](int i) const
    {return storage[i];}

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator()(int i)
    {return storage(i);}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator()(int i) const
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
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection);
};

#endif

