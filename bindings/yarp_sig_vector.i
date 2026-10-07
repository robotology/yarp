// SPDX-FileCopyrightText: 2006-2026 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// SWIG wrapping for yarp::sig::Vector and yarp::sig::VectorInt.
//
// This file is not a standalone module: it is meant to be included by
// yarp.i, after the std::vector<double> and std::vector<int> templates
// (DVector, IVector) have been instantiated.

// yarp::sig::VectorOf<T> is an alias template (not supported by SWIG) which selects
// one of the concrete classes yarp::sig::VectorOfDouble, yarp::sig::VectorOfInt, etc.
// Only the double and int versions are wrapped, with the names Vector and VectorInt.
// Their methods come from the CRTP base class yarp::sig::VectorBase, which is
// instantiated with %template before including Vector.h (see below).
%ignore vector_selector;
%ignore yarp::sig::VectorOfFloat;
%ignore yarp::sig::VectorOfString;
%ignore yarp::sig::VectorOfSizet;
%rename(Vector) yarp::sig::VectorOfDouble;
%rename(VectorInt) yarp::sig::VectorOfInt;

// Vector.h must be included after the std::vector<double> and std::vector<int> templates,
// so that the constructors Vector(const std::vector<double>&) and VectorInt(const std::vector<int>&)
// (defined below with %extend) can accept native lists.
// VectorBase is a CRTP base class: it must not be instantiated directly
%ignore yarp::sig::VectorBase::VectorBase;
%include <yarp/sig/VectorBase.h>
%template(VectorBaseDouble) yarp::sig::VectorBase<yarp::sig::VectorOfDouble, double>;
%template(VectorBaseInt) yarp::sig::VectorBase<yarp::sig::VectorOfInt, int>;
%include <yarp/sig/Vector.h>

typedef yarp::sig::VectorOfDouble Vector;
typedef yarp::sig::VectorOfInt VectorInt;

%{
typedef yarp::sig::VectorOf<double> Vector;
typedef yarp::sig::VectorOf<int> VectorInt;
%}

#if SWIG_VERSION < 0x030012
%rename(VectorIterator) yarp::sig::VectorOfDouble::iterator;
%rename(VectorConstIterator) yarp::sig::VectorOfDouble::const_iterator;
#endif

MAKE_COMMS  (Vector, yarp::sig::VectorOfDouble)
MAKE_COMMS  (VectorInt, yarp::sig::VectorOfInt)

%extend yarp::sig::VectorOfDouble {

    // SWIG does not handle the constructors inherited from yarp::sig::VectorBase
    // (using VectorBase::VectorBase), so they are re-declared here.
    // All the other methods are inherited from VectorBase.

    VectorOfDouble()
    {
        return new yarp::sig::VectorOfDouble();
    }

    VectorOfDouble(size_t size)
    {
        return new yarp::sig::VectorOfDouble(size);
    }

    VectorOfDouble(size_t size, double def)
    {
        return new yarp::sig::VectorOfDouble(size, def);
    }

    VectorOfDouble(const yarp::sig::VectorOfDouble& other)
    {
        return new yarp::sig::VectorOfDouble(other);
    }

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOfDouble(const std::vector<double>& values)
    {
        yarp::sig::VectorOfDouble* newVec = new yarp::sig::VectorOfDouble();
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

    // toString() comes from VectorOf*Data and operator== is a friend function of VectorBase:
    // neither is visible to SWIG.
    std::string toString()
    {
        return self->toString();
    }

    bool isEqual(const yarp::sig::VectorOfDouble& other) const
    {
        return *self == other;
    }

    double get(int j)
    {
        return self->operator [](j);
    }

    void set(int j, double v)
    {
        self->operator [](j) = v;
    }

#ifdef SWIGPYTHON
    void __setitem__(int key, double value) {
        self->operator[](key) = value;
    }

    double __getitem__(int key) {
        return self->operator[](key);
    }

    size_t __len__() {
        return self->size();
    }
#endif
}

%extend yarp::sig::VectorOfInt {

    // SWIG does not handle the constructors inherited from yarp::sig::VectorBase
    // (using VectorBase::VectorBase), so they are re-declared here.
    // All the other methods are inherited from VectorBase.

    VectorOfInt()
    {
        return new yarp::sig::VectorOfInt();
    }

    VectorOfInt(size_t size)
    {
        return new yarp::sig::VectorOfInt(size);
    }

    VectorOfInt(size_t size, int def)
    {
        return new yarp::sig::VectorOfInt(size, def);
    }

    VectorOfInt(const yarp::sig::VectorOfInt& other)
    {
        return new yarp::sig::VectorOfInt(other);
    }

    // This in not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    VectorOfInt(const std::vector<int>& values)
    {
        yarp::sig::VectorOfInt* newVec = new yarp::sig::VectorOfInt();
        newVec->reserve(values.size());
        for (const auto& element : values) {
            newVec->push_back(element);
        }
        return newVec;
    }

    // toString() comes from VectorOf*Data and operator== is a friend function of VectorBase:
    // neither is visible to SWIG.
    std::string toString()
    {
        return self->toString();
    }

    bool isEqual(const yarp::sig::VectorOfInt& other) const
    {
        return *self == other;
    }

    int get(int j)
    {
        return self->operator [](j);
    }

    void set(int j, int v)
    {
        self->operator [](j) = v;
    }

#ifdef SWIGPYTHON
    void __setitem__(int key, int value) {
        self->operator[](key) = value;
    }

    int __getitem__(int key) {
        return self->operator[](key);
    }

    size_t __len__() {
        return self->size();
    }
#endif
}
