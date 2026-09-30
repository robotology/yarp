/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_VECTOR_H
#define YARP_SIG_VECTOR_H

#include <cstddef> //defines size_t
#include <memory>
#include <string>
#include <algorithm>

#include <yarp/sig/api.h>
#include <yarp/sig/VectorOfDoubleData.h>
#include <yarp/sig/VectorOfInt32Data.h>
#include <yarp/sig/VectorOfSizetData.h>
#include <yarp/sig/VectorOfFloatData.h>
#include <yarp/sig/VectorOfStringData.h>

/**
* \file Vector.h contains the definition of a Vector type
*/

template <typename T>
class YARP_sig_API vector_selector
{
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, size_t> |
        std::is_same_v<T, double> || std::is_same_v<T, std::string> || std::is_same_v<T, float>,
        "VectorOf<T>: T must be int, size_t or double"
    );
};

namespace yarp::sig
{
    template <typename Derived, typename Scalar>
    class VectorBase;
    class VectorOfDouble;
    class VectorOfInt;
    class VectorOfSizet;
    class VectorOfString;
    class VectorOfFloat;
    class VectorOfSizetData;

    typedef VectorOfDouble Vector;
} // namespace yarp::sig

template <typename Derived, typename Scalar>
class yarp::sig::VectorBase
{
    public:

    using value_type     =  Scalar;
    using iterator       =  typename std::vector<Scalar>::iterator;
    using const_iterator =  typename std::vector<Scalar>::const_iterator;

    VectorBase() = default;

    VectorBase(size_t size)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().resize(size);
    }

    /**
     * @brief Initializer list constructor.
     * @param[in] values, list of values with which initialize the Vector.
     */
    VectorBase(std::initializer_list<Scalar> values)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().assign(values.begin(), values.end());
    }


    /**
    * Build a vector and initialize it with def.
    * @param s the size
    * @param def a default value used to fill the vector
    */
    VectorBase(size_t s, const Scalar& def)
    {
      auto& v = static_cast<Derived&>(*this);
        v.privVec().assign(s, def);
    }

    /**
    * Builds a vector and initialize it with values from 'p'. Copies memory.
    * @param s the size of the data to be copied
    * @param T* the pointer to the data
    */
    VectorBase(size_t s, const Scalar *p)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().assign(p, p + s);
    }

    VectorBase(const VectorBase& r) = default;
    VectorBase<Derived, Scalar> &operator=(const VectorBase<Derived,Scalar>& r) = default;
    VectorBase(VectorBase<Derived, Scalar>&& other) noexcept = default;
    VectorBase& operator=(VectorBase<Derived, Scalar>&& other) noexcept = default;
    ~VectorBase() = default;


    public:
    /**
     * @brief Returns the capacity of the vector.
     * @return the capacity of the vector.
     */
    inline size_t capacity() const
    {
        const auto& v = static_cast<const Derived&>(*this);
        return v.privdata.capacity();
    }

    /**
     * @brief Reserve space in the vector.
     * @param size the new capacity of the vector.
     */
    void reserve(size_t size)
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privdata.reserve(size);
    }

    /**
     * @brief Resize the vector.
     * @param size the new size of the vector.
     */
    void resize(size_t size) {
         auto& v = static_cast<Derived&>(*this);
         v.privdata.resize(size);
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    inline void pop_back()
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().pop_back();
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline Scalar &operator[](size_t i)
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privVec()[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const Scalar  &operator[](size_t i) const
    {
        const auto& v = static_cast<const Derived&>(*this);
        return v.privVec()[i];
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline Scalar &operator()(size_t i)
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privVec()[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const Scalar &operator()(size_t i) const
    {
        const auto& v = static_cast<const Derived&>(*this);
        return v.privVec()[i];
    }

    /**
     * Returns the number of elements in the vector.
    */
    inline size_t size() const
    {
        const auto& v = static_cast<const Derived&>(*this);
        return v.privVec().size();
    }

    Derived& operator=(const Scalar& val)
    {
        auto& v = static_cast<Derived&>(*this);
        if (!v.privVec().empty()) {
            std::fill(v.privVec().begin(), v.privVec().end(), val);
        }
        return v;
    }

    // Compare element-wise equality with another Derived
    /* bool operator==(const Derived& r) const
    {
        const auto& self = static_cast<const Derived&>(*this);
        if (self.size() != r.size()) return false;
        for (size_t i = 0; i < self.size(); ++i) {
            if (self[i] != r[i]) return false;
        }
        return true;
    }*/

    friend bool operator==(const Derived& l, const Derived& r)
    {
        if (l.size() != r.size()) return false;
        for (size_t i = 0; i < l.size(); ++i) {
            if (l[i] != r[i]) return false;
        }
        return true;
    }

    /**
    * Clear (removes all elements) the vector.
    */
    void clear()
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().clear();
    }

    /**
    * Push a new element in the vector: size is changed
    */
    inline void push_back (const Scalar &elem)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().push_back(elem);
    }

    /**
     * @brief Move a new element in the vector: size is changed
     * @param elem, element to be moved.
     */
    inline void push_back (Scalar&& elem)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().push_back(std::move(elem));
    }

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double (or nullptr if the vector is of zero length)
    */
    inline Scalar* data()
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privVec().empty() ? nullptr : &(v.privVec().at(0));
    }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double (or nullptr if the vector is of zero length)
    */
    inline const Scalar* data() const
    {
        auto& v = static_cast<const Derived&>(*this);
        return v.privVec().empty() ? nullptr : &(v.privVec().at(0));
    }

    /**
    * Creates and returns a new vector, being the portion of the original
    * vector defined by the first and last indexes of the items to be included
    * in the subvector. The indexes are checked: if wrong, a null vector is
    * returned.
    */
    Derived subVector(unsigned int first, unsigned int last) const
    {
        Derived ret;
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
    * Resize the vector and initialize the element to a default value.
    * @param s the new size
    * @param def the default value
    */
    void resize(size_t size, const Scalar&def)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().resize(size);
        std::fill(v.privVec().begin(), v.privVec().end(), def);
    }

    /**
    * Set to zero the elements of the vector.
    */
    void zero()
    {
        auto& v = static_cast<Derived&>(*this);
        std::fill(v.privVec().begin(), v.privVec().end(), Scalar(0));
    }

    /**
     * @brief Returns an iterator to the beginning of the Vector
     */
    iterator begin() noexcept
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privVec().begin();
    }

    iterator end() noexcept
    {
        auto& v = static_cast<Derived&>(*this);
        return v.privVec().end();
    }

    const_iterator begin() const noexcept
    {
        auto& v = static_cast<const Derived&>(*this);
        return v.privVec().begin();
    }

    const_iterator end() const noexcept
    {
        auto& v = static_cast<const Derived&>(*this);
        return v.privVec().end();
    }

    const_iterator cbegin() const noexcept
    {
        auto& v = static_cast<const Derived&>(*this);
        return v.privVec().cbegin();
    }

    /**
    * Remove an element from the vector.
    * @param pos iterator pointing at the element to remove
    */
    void erase(iterator pos)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().erase(pos);
    }

    /**
    * Remove one or more elements from the vector.
    * @param first iterator pointing at the first element to remove
    * @param last iterator pointing at the last element to remove
    */
    void erase(iterator first, iterator last)
    {
        auto& v = static_cast<Derived&>(*this);
        v.privVec().erase(first, last);
    }

    size_t getElementSize() const
    {
        return sizeof(Scalar);
    }
};


class YARP_sig_API yarp::sig::VectorOfDouble : public yarp::sig::VectorOfDoubleData, public yarp::sig::VectorBase<VectorOfDouble, double>
{
    friend class VectorBase<VectorOfDouble, double>;

    public:
    using yarp::sig::VectorOfDoubleData::VectorOfDoubleData;
    using yarp::sig::VectorBase<VectorOfDouble, double>::VectorBase;
    protected:
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
};

class YARP_sig_API yarp::sig::VectorOfInt : public yarp::sig::VectorOfInt32Data, public yarp::sig::VectorBase<VectorOfInt, int>
{
    friend class VectorBase<VectorOfInt, int>;

    public:
    using yarp::sig::VectorOfInt32Data::VectorOfInt32Data;
    using yarp::sig::VectorBase<VectorOfInt, int>::VectorBase;
    protected:
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
};

class YARP_sig_API yarp::sig::VectorOfFloat : public yarp::sig::VectorOfFloatData, public yarp::sig::VectorBase<VectorOfFloat, float>
{
    friend class VectorBase<VectorOfFloat, float>;

    public:
    using yarp::sig::VectorOfFloatData::VectorOfFloatData;
    using yarp::sig::VectorBase<VectorOfFloat, float>::VectorBase;
    protected:
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
};

class YARP_sig_API yarp::sig::VectorOfString : public yarp::sig::VectorOfStringData, public yarp::sig::VectorBase<VectorOfString, std::string>
{
    friend class VectorBase<VectorOfString, std::string>;

    public:
    using yarp::sig::VectorOfStringData::VectorOfStringData;
    using yarp::sig::VectorBase<VectorOfString, std::string>::VectorBase;
    protected:
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
};

class YARP_sig_API yarp::sig::VectorOfSizet : public yarp::sig::VectorOfSizetData, public yarp::sig::VectorBase<VectorOfSizet, size_t>
{
    friend class VectorBase<VectorOfSizet, size_t>;

    public:
    using yarp::sig::VectorOfSizetData::VectorOfSizetData;
    using yarp::sig::VectorBase<VectorOfSizet, size_t>::VectorBase;
    protected:
    auto& privVec() { return privdata; }
    const auto& privVec() const { return privdata; }
};

template <>
struct YARP_sig_API  vector_selector<int> {
    using type = yarp::sig::VectorOfInt;
};

template <>
struct YARP_sig_API  vector_selector<double> {
    using type = yarp::sig::VectorOfDouble;
};

template <>
struct YARP_sig_API  vector_selector<float> {
    using type = yarp::sig::VectorOfFloat;
};

template <>
struct YARP_sig_API  vector_selector<std::string> {
    using type = yarp::sig::VectorOfString;
};

template <>
struct YARP_sig_API  vector_selector<size_t> {
    using type = yarp::sig::VectorOfSizet;
};

namespace yarp::sig {
template <typename T>
using VectorOf = typename vector_selector<T>::type;
}

#endif // YARP_SIG_VECTOR_H
