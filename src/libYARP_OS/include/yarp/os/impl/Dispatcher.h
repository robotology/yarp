/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_DISPATCHER_H
#define YARP_OS_IMPL_DISPATCHER_H

#include <yarp/os/ConstString.h>

#include <yarp/os/impl/Logger.h>

#include <map>
#include <vector>
#include <cstdio>

namespace yarp {
    namespace os {
        namespace impl {
            template <class T, class RET> class Dispatcher;
        }
    }
}

/**
 * Dispatch to named methods based on string input.
 */
template <class T, class RET>
class yarp::os::impl::Dispatcher
{
private:
    class Entry
    {
    public:
        ConstString name;
        RET (T::*fn)(int argc, char *argv[]);

        Entry(const char *name, RET (T::*fn)(int argc, char *argv[])) :
            name(name),
            fn(fn)
        {
        }

        Entry()
        {
        }
    };

    std::map<ConstString, Entry> action;
    std::vector<ConstString> names;

public:
    void add(const char *name, RET (T::*fn)(int argc, char *argv[]))
    {
        Entry e(name, fn);
        action[ConstString(name)] = e;
        // maintain a record of order of keys
        names.push_back(ConstString(name));
    }

    RET dispatch(T *owner, const char *name, int argc, char *argv[])
    {
        ConstString sname(name);
        typename std::map<ConstString, Entry>::const_iterator it = action.find(sname);
        if (it != action.end()) {
            return (owner->*(it->second.fn))(argc, argv);
        } else {
            YARP_SPRINTF1(Logger::get(), error, "Could not find command \"%s\"", name);
        }
        return RET();
    }

    std::vector<ConstString> getNames()
    {
        return names;
    }
};

#endif // YARP_OS_IMPL_DISPATCHER_H
