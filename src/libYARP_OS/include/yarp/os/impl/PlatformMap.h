// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PLATFORMMAP_
#define _YARP2_PLATFORMMAP_

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <yarp/conf/api.h>
#include <yarp/conf/system.h>
#ifndef YARP_USE_STL
#  include <ace/Hash_Map_Manager.h>
#  include <ace/Null_Mutex.h>
#  include <ace/Functor_String.h>
#  ifndef ACE_HAS_STRING_HASH


template<>
class YARP_OS_impl_API ACE_Equal_To<std::string>
{
public:
    int operator() (const std::string& x, const std::string& y) const {
        return x == y;
    }
};

template<>
class YARP_OS_impl_API ACE_Hash<std::string>
{
public:
    unsigned long operator() (const std::string& x) const {
        unsigned long h = 0;
        for (size_t i=0; i<x.length(); i++) {
            unsigned char ch = x[i];
            h = (h << 4) + (ch * 13);
            unsigned long g = h & 0xf0000000;
            if (g) {
                h ^= (g>>24);
                h ^= g;
            }
        }
        return h;
    }
};

template<>
class YARP_OS_impl_API ACE_Less_Than<std::string>
{
public:
    int operator() (const std::string &x, const std::string &y) const {
        return x<y;
    }
};
#  endif
#  define PLATFORM_MAP(x,y) ACE_Hash_Map_Manager<x,y,ACE_Null_Mutex>
#  define PLATFORM_MAP_ITERATOR(x,y,z) ACE_Hash_Map_Entry<x,y> *z = 0/*NULL*/
#  define PLATFORM_MAP_ITERATOR_IN_TEMPLATE(x,y,z) PLATFORM_MAP_ITERATOR(x,y,z)
#  define PLATFORM_MAP_ITERATOR_FIRST(y) ((*y).ext_id_)
#  define PLATFORM_MAP_ITERATOR_SECOND(y) ((*y).int_id_)
#  define PLATFORM_MAP_SET(x,y,z) x.bind(y,z)
#  define PLATFORM_MAP_UNSET(x,y) x.unbind(y)
#  define PLATFORM_MAP_FIND(x,y,z) x.find(y,z)
#  define PLATFORM_MAP_FIND_RAW(x,y,z) x.find(y,z)
#  define PLATFORM_MAP_CLEAR(x) x.unbind_all()
#else
#  include <map>
#  define PLATFORM_MAP(x,y) std::map<x,y>
#  define PLATFORM_MAP_ITERATOR(x,y,z) std::map<x,y>::iterator z;
#  define PLATFORM_MAP_ITERATOR_IN_TEMPLATE(x,y,z) typename PLATFORM_MAP_ITERATOR(x,y,z)
#  define PLATFORM_MAP_ITERATOR_FIRST(y) (y->first)
#  define PLATFORM_MAP_ITERATOR_SECOND(y) (y->second)
#  define PLATFORM_MAP_SET(x,y,z) x[y] = z
#  define PLATFORM_MAP_UNSET(x,y) x.erase(y)
template <class KEY, class VAL, class IT>
int _platform_map_find(std::map<KEY,VAL>& store,const KEY& key, IT& val) {
    typename std::map<KEY,VAL>::iterator it = store.find(key);
    if (it==store.end()) return -1;
    val = it;
    return 0;
}
template <class KEY, class VAL>
int _platform_map_find_raw(std::map<KEY,VAL>& store,const KEY& key, VAL& val) {
    typename std::map<KEY,VAL>::iterator it = store.find(key);
    if (it==store.end()) return -1;
    val = it->second;
    return 0;
}
#  define PLATFORM_MAP_FIND(x,y,z) _platform_map_find(x,y,z)
#  define PLATFORM_MAP_FIND_RAW(x,y,z) _platform_map_find_raw(x,y,z)
#  define PLATFORM_MAP_CLEAR(x) x.clear()
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif

