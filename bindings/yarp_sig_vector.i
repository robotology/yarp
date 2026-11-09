// SPDX-FileCopyrightText: 2006-2026 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// SWIG wrapping for some yarp::sig::VectorBase subtypes.
//
// This file is not a standalone module: it is meant to be included by
// yarp.i, after the std::vector<(double|int|std::string)>  templates
// (DVector, IVector, SVector) have been instantiated.

// yarp::sig::VectorOf<T> is an alias template (not supported by SWIG) which selects
// one of the concrete classes yarp::sig::VectorOfDouble, yarp::sig::VectorOfInt, etc.
// Only the double, int and std::string versions are wrapped, with the names Vector,
// VectorInt and VectorString. Their methods come from the CRTP base class
// yarp::sig::VectorBase, which is instantiated with %template before including
// Vector.h (see below).
%ignore vector_selector;
%ignore yarp::sig::VectorOfFloat;
%ignore yarp::sig::VectorOfSizet;
%rename(Vector) yarp::sig::VectorOfDouble;
%rename(VectorInt) yarp::sig::VectorOfInt;
%rename(VectorString) yarp::sig::VectorOfString;

// Vector.h must be included after the corresponding std::vector<T> templates, so that the
// constructors Vector(const std::vector<double>&), VectorInt(const std::vector<int>&)
// and VectorString(const std::vector<std::string>&) (defined below with %extend) can accept
// native lists. VectorBase is a CRTP base class: it must not be instantiated directly.
%ignore yarp::sig::VectorBase::VectorBase;
%include <yarp/sig/VectorBase.h>
%template(VectorBaseDouble) yarp::sig::VectorBase<yarp::sig::VectorOfDouble, double>;
%template(VectorBaseInt) yarp::sig::VectorBase<yarp::sig::VectorOfInt, int>;
%template(VectorBaseString) yarp::sig::VectorBase<yarp::sig::VectorOfString, std::string>;
%include <yarp/sig/Vector.h>

typedef yarp::sig::VectorOfDouble Vector;
typedef yarp::sig::VectorOfInt VectorInt;
typedef yarp::sig::VectorOfString VectorString;


%{
typedef yarp::sig::VectorOf<double> Vector;
typedef yarp::sig::VectorOf<int> VectorInt;
typedef yarp::sig::VectorOf<std::string> VectorString;
%}

#if SWIG_VERSION < 0x030012
%rename(VectorIterator) yarp::sig::VectorOfDouble::iterator;
%rename(VectorConstIterator) yarp::sig::VectorOfDouble::const_iterator;
#endif

MAKE_COMMS(Vector, yarp::sig::VectorOfDouble)
MAKE_COMMS(VectorInt, yarp::sig::VectorOfInt)
MAKE_COMMS(VectorString, yarp::sig::VectorOfString)

%define EXTEND_VECTOR_SUBTYPE(classname, type)

%extend yarp::sig::classname {

    // SWIG does not handle the constructors inherited from yarp::sig::VectorBase
    // (using VectorBase::VectorBase), so they are re-declared here.
    // All the other methods are inherited from VectorBase.

    classname()
    {
        return new yarp::sig::classname();
    }

    classname(size_t size)
    {
        return new yarp::sig::classname(size);
    }

    classname(size_t size, const type& def)
    {
        return new yarp::sig::classname(size, def);
    }

    classname(const yarp::sig::classname& other)
    {
        return new yarp::sig::classname(other);
    }

    // This is not a real constructor actually, it is converted by swig to a function returning a pointer.
    // See: http://www.swig.org/Doc3.0/CPlusPlus11.html#CPlusPlus11_initializer_lists
    classname(const std::vector<type>& values)
    {
        yarp::sig::classname* newVec = new yarp::sig::classname();
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

    bool isEqual(const yarp::sig::classname& other) const
    {
        return *self == other;
    }

    type get(int j) const
    {
        return self->operator [](j);
    }

    void set(int j, type v)
    {
        self->operator [](j) = v;
    }

#ifdef SWIGPYTHON
    void __setitem__(int key, type value) {
        self->operator[](key) = value;
    }

    type __getitem__(int key) const {
        return self->operator[](key);
    }

    size_t __len__() const {
        return self->size();
    }
#endif
}

%enddef

EXTEND_VECTOR_SUBTYPE(VectorOfDouble, double)
EXTEND_VECTOR_SUBTYPE(VectorOfInt, int)
EXTEND_VECTOR_SUBTYPE(VectorOfString, std::string)
