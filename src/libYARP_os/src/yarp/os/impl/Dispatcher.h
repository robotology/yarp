/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_DISPATCHER_H
#define YARP_OS_IMPL_DISPATCHER_H

#include <yarp/os/impl/LogComponent.h>

#include <cstdio>
#include <map>
#include <string>
#include <vector>

YARP_DECLARE_LOG_COMPONENT(DISPATCHER)

namespace yarp {
namespace os {
namespace impl {

/**
 * Dispatch to named methods based on string input.
 */
template <class T, class RET>
class Dispatcher
{
private:
    class Entry
    {
    public:
        std::string name;
        RET (T::*fn)(int argc, char* argv[]);

        Entry(const char* name, RET (T::*fn)(int argc, char* argv[])) :
                name(name),
                fn(fn)
        {
        }

        Entry() :
                fn(nullptr)
        {
        }
    };

    std::map<std::string, Entry> action;
    std::vector<std::string> names;

public:
    void add(const char* name, RET (T::*fn)(int argc, char* argv[]))
    {
        Entry e(name, fn);
        action[std::string(name)] = e;
        // maintain a record of order of keys
        names.push_back(std::string(name));
    }

    RET dispatch(T* owner, const char* name, int argc, char* argv[])
    {
        std::string sname(name);
        typename std::map<std::string, Entry>::const_iterator it = action.find(sname);
        if (it != action.end()) {
            return (owner->*(it->second.fn))(argc, argv);
        } else {
            yCError(DISPATCHER, "Could not find command \"%s\"", name);
        }
        return RET();
    }

    std::vector<std::string> getNames()
    {
        return names;
    }
};

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_DISPATCHER_H
