// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// 26/01/2012: Changed storage from vector to double *. Resize() now maintain old content.

// $Id: Vector.h,v 1.27 2009-03-26 13:15:44 natta Exp $

#ifndef _YARP2_VECTOR_
#define _YARP2_VECTOR_

//#include <stdlib.h> //defines size_t
#include <stddef.h> //defines size_t
#include <yarp/os/Portable.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Log.h>
#include <yarp/sig/api.h>


/**
* \file Vector.h contains the definition of a Vector type 
*/
namespace yarp {

    namespace sig {
		class VectorBase;
        class Vector;
        template<unsigned int _Size> class VectorN;
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
    virtual size_t getListSize() const = 0;
    virtual const char *getMemoryBlock() const = 0;
    virtual void resize(size_t size) = 0;

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
        first = (T *) bytes.get();
        len = bytes.used()/sizeof(T);
    }

public:
    VectorOf() {
        // Del Prete 3/10/2013: I prefer not to preallocate memory if not necessary
        //bytes.allocate(16*sizeof(T)); // preallocate space for 16 elements
        //bytes.setUsed(0);
        first = 0;
        len = 0;
    }

    VectorOf(size_t size) : bytes(size*sizeof(T)) {
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
    }

    VectorOf(size_t size, const T &def) : bytes(size*sizeof(T)) {
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i=0; i<size; i++) { (*this)[i] = def; }
    }

    VectorOf(size_t size, const T *v) : bytes(size*sizeof(T)) {
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i=0; i<size; i++) { (*this)[i] = v[i]; }
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

    virtual int getElementSize() const {
        return sizeof(T);
    }

    virtual size_t getListSize() const
    {
        return len;
    }

    virtual const char *getMemoryBlock() const
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

    virtual void resize(size_t size)
    {
        // T def = (T)0;
        // resize(size, def);
        bytes.allocateOnNeed(size*sizeof(T),size*sizeof(T));
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
    }

    void resize(size_t size, const T&def)
    {
        resize(size); 
        for (size_t i=0; i<size; i++) { (*this)[i] = def; }
    }

    virtual void assign(size_t size, const T *v)
    {
        resize(size);
        for (size_t i=0; i<size; i++) { (*this)[i] = v[i]; }
    }

    inline void push_back (const T &elem)
    {
        bytes.allocateOnNeed(bytes.used()+sizeof(T),bytes.length()*2+sizeof(T));
        _updatePointers();
        first[len] = elem;
        bytes.setUsed(bytes.used()+sizeof(T));
        len++;
    }

    inline void pop_back (void)
    {
        if (bytes.used()>sizeof(T)) {
            bytes.setUsed(bytes.used()-sizeof(T));
            len--;
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
        first = 0 /*NULL*/;
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
protected:
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

    explicit Vector(size_t s):storage(s)
    {
        allocGslData();
        updateGslData();
    }

    /**
    * Build a vector and initialize it with def.
    * @param s the size
    * @param def a default value used to fill the vector
    */
    explicit Vector(size_t s, const double &def): storage(s, def)
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
    Vector(const Vector &r): storage(r.size(), r.data())    
    {
        // DelPrete 4/10/13
        // Note: since the introduction of VectorN I can no longer do "storage(r.storage)"
        // because if r is a VectorN then the vector data are located in storageStatic.
        allocGslData();
        updateGslData();
    }

    /**
    * Copy operator;
    */
    virtual const Vector &operator=(const Vector &r);

    /**
    * Set all elements of the vector to a scalar.
    */
    virtual const Vector &operator=(double v)
    {
        double *tmp=data();
        for(size_t k=0; k<size(); k++)
            tmp[k]=v;
        return *this;
    }

    /**
    * Resize the vector, (warning: deprecated, use resize) here
    * to maintain compatibility with the old Vector class.
    * @param s the new size
    */
    virtual void size(size_t s){ Vector::resize(s); }

    /**
    * Resize the vector.
    * @param s the new size
    */
    virtual void resize(size_t s)
    {
        storage.resize(s);
        updateGslData();
    }

    /**
    * Resize the vector, if the vector is not empty preserve old content.
    * @param size the new size
    * @param def a default value used to fill the vector
    */
    virtual void resize(size_t size, const double &def)
    {
        storage.resize(size, def);
        updateGslData();
    }

    virtual inline size_t size() const
    { return storage.size();}

    /**
    * Get the length of the vector.
    * @return the length of the vector.
    */
    inline size_t length() const
    { return size();}

    /**
    * Zero the elements of the vector.
    */
    virtual void zero();

    /**
    * Creates a string object containing a text representation of the object. Useful for printing.
    * To get a nice format the optional parameters precision and width may be used (same meaning as in printf and cout).
    * @param precision the number of digits to be printed after the decimal point.
    * @param width minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is never truncated.
    * If width is specified the inter-value separator is a blank space, otherwise it is a tab.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    */
    virtual yarp::os::ConstString toString(int precision=-1, int width=-1) const;


    /**
    * Creates and returns a new vector, being the portion of the original
    * vector defined by the first and last indexes of the items to be included
    * in the subvector. The indexes are checked: if wrong, a null vector is
    * returned.
    */
    virtual Vector subVector(unsigned int first, unsigned int last) const
    {
        Vector ret;
        if((first<=last)&&((int)last<(int)size()))
        {
            ret.resize(last-first+1);
            for(unsigned int k=first; k<=last; k++)
                ret[k-first]=(*this)[k];
        }
        return ret;
    }

    template<unsigned int _size>
    VectorN<_size> subVector(unsigned int first) const
    {
        VectorN<_size> ret;
        if(first+_size < size())
            for(unsigned int k=0; k<_size; k++)
                ret[k]=(*this)[first+k];
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
    virtual bool setSubvector(int position, const Vector &v)
    {    
        if(position+v.size() > size())
            return false;
        for(size_t i=0;i<v.size();i++)
            (*this)[position+i] = v(i);
        return true;
    }

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double.
    */
    virtual inline double *data()
    { return storage.getFirst(); }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double.
    */
    virtual inline const double *data() const
    { return storage.getFirst();}

    /**
    * True iff all elements of 'a' match all element of 'b'.
    */
    virtual bool operator==(const yarp::sig::Vector &r) const
    {
        //check dimensions first
        size_t c=size();
        if (c!=r.size())
            return false;

        const double *tmp1=data();
        const double *tmp2=r.data();

        while(c--)
        {
            if (*tmp1++!=*tmp2++)
                return false;
        }

        return true;
    }

    /**
    * Push a new element in the vector: size is changed
    */
    virtual inline void push_back (const double &elem)
    {
        storage.push_back(elem);
        updateGslData();
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    virtual inline void pop_back (void)
    {
        storage.pop_back();
        updateGslData();
    }

    /**
    * Return a void pointer to a gsl compatible structure.
    * This pointer can be safely cast to a gsl_vector*.
    */
    virtual void *getGslVector(){ return gslData; }
    
    /**
    * Return a void pointer to a gsl compatible structure.
    * This pointer can be safely cast to a gsl_vector*.
    */
    virtual const void *getGslVector() const{ return gslData; }

        /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    virtual inline double &operator[](size_t i)
    {return storage[i];}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    virtual inline const double &operator[](size_t i) const
    {return storage[i];}

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    virtual inline double &operator()(size_t i)
    {return storage(i);}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    virtual inline const double &operator()(size_t i) const
    {return storage(i);}

    /**
    * Clears out the vector, it does not reallocate
    * the buffer, but just sets the dynamic size to 0.
    */
    virtual void clear ()
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

    virtual yarp::os::Type getType() {
        return yarp::os::Type::byName("yarp/vector");
    }
};


/** Vector of constant size (known at compile time). 
*/
template<unsigned int _Size>
class YARP_sig_API yarp::sig::VectorN: public yarp::sig::Vector
{
protected:
    double  storageStatic[_Size];   // static memory of the vector

public:
    VectorN() { updateGslData(); }

    /**
    * Build a vector and initialize it with def.
    * @param def a default value used to fill the vector
    */
    explicit VectorN(const double &def){ *this = def; updateGslData(); }

    /**
    * Copy constructors.
    */
    VectorN(const double *p){           *this = p; updateGslData(); }
    VectorN(const VectorN<_Size> &r) {  *this = r; updateGslData(); }
    VectorN(const Vector  &r) {         *this = r; updateGslData(); }

    /**
    * Set all elements of the vector to the specified scalar value v.
    */
    const VectorN &operator=(double v)
    {
        for(size_t k=0; k<_Size; k++)
            storageStatic[k] = v;
        return *this;
    }

    /**
    * Copy the values of the specified vector v into this vector.
    */
    const VectorN &operator=(double *v)
    {
        for(size_t k=0; k<_Size; k++)
            storageStatic[k] = v[k];
        return *this;
    }

    /** If r has a size different from this vector' size, the program terminates. */
    const VectorN<_Size> &operator=(const Vector &r)
    {
        YARP_ASSERT(_Size == r.size())
        memcpy(data(), r.data(), sizeof(double)*_Size);
        return *this;
    }

    yarp::sig::Vector subVector(unsigned int first, unsigned int last) const
    {
        YARP_ASSERT(last>=first && last<_Size);
        yarp::sig::Vector ret(last-first+1);
        for(unsigned int k=first; k<=last; k++)
            ret[k-first] = storageStatic[k];
        return ret;
    }

    template<unsigned int _s>
    VectorN<_s> subVector(unsigned int first) const
    {
        YARP_ASSERT(first+_s < _Size);
        VectorN<_s> ret;
        for(unsigned int k=0; k<_s; k++)
            ret[k] = (*this)[first+k];
        return ret;
    }

    /**
     * Set a portion of this vector with the values of the specified vector.
     * If the specified vector v is to big the method does not resize the vector,
     * but return false.
     * @param position index of the first value to set
     * @param v vector containing the values to set
     * @return true if the operation succeeded, false otherwise
     */
    bool setSubvector(int position, const Vector &v)
    {    
        if(position+v.size() > _Size)
            return false;
        for(size_t i=0;i<v.size();i++)
            storageStatic[position+i] = v(i);
        return true;
    }

    /**
    * Resize the vector, (warning: deprecated, use resize) here to maintain compatibility with the old Vector class.
    * @param s the new size
    */
    void size(size_t s){ VectorN::resize(s); }

    /**
    * Resize the vector.
    * @param s the new size
    */
    void resize(size_t s){ YARP_ASSERT(s==_Size); }

    /**
    * Resize the vector, if the vector is not empty preserve old content.
    * @param size the new size
    * @param def a default value used to fill the vector
    */
    void resize(size_t size, const double &def){ YARP_ASSERT(size==_Size); *this = def; }

    inline size_t size() const { return _Size;}
    inline size_t length() const { return _Size;}

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double.
    */
    inline double *data()
    { return storageStatic; }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double.
    */
    inline const double *data() const
    { return storageStatic;}

        /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator[](size_t i)
    {return storageStatic[i];}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator[](size_t i) const
    {return storageStatic[i];}

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline double &operator()(size_t i)
    {return storageStatic[i];}

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const double &operator()(size_t i) const
    {return storageStatic[i];}

    /**
    * Operation now allowed on a constant-size vector.
    */
    inline void push_back (const double &elem){ YARP_ASSERT(false); }

    /**
    * Operation now allowed on a constant-size vector.
    */
    inline void pop_back (void){ YARP_ASSERT(false); }

    /**
    * Operation now allowed on a constant-size vector.
    */
    void clear (){ YARP_ASSERT(false); }
};

typedef yarp::sig::VectorN<1> Vector1;
typedef yarp::sig::VectorN<2> Vector2;
typedef yarp::sig::VectorN<3> Vector3;
typedef yarp::sig::VectorN<4> Vector4;
typedef yarp::sig::VectorN<5> Vector5;
typedef yarp::sig::VectorN<6> Vector6;
typedef yarp::sig::VectorN<7> Vector7;

#endif

