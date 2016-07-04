/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_DISPATCHER
#define YARP2_DISPATCHER

#include <yarp/os/ConstString.h>

#include <yarp/os/impl/PlatformMap.h>
#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/Logger.h>


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
class yarp::os::impl::Dispatcher {
private:
    class Entry {
    public:
        ConstString name;
        RET (T::*fn)(int argc, char *argv[]);

        Entry(const char *name, RET (T::*fn)(int argc, char *argv[])) :
            name(name),
            fn(fn)
        {}

        Entry() {
        }
    };

    PLATFORM_MAP(ConstString,Entry) action;
    PlatformVector<ConstString> names;

public:
    void add(const char *name, RET (T::*fn)(int argc, char *argv[])) {
        Entry e(name,fn);
        PLATFORM_MAP_SET(action,ConstString(name),e);
        // maintain a record of order of keys
        names.push_back(ConstString(name));
    }

    RET dispatch(T *owner, const char *name, int argc, char *argv[]) {
        ConstString sname(name);
        Entry e;
        int result = PLATFORM_MAP_FIND_RAW(action,sname,e);
        if (result!=-1) {
            return (owner->*(e.fn))(argc,argv);
        } else {
            YARP_SPRINTF1(Logger::get(),error,"Could not find command \"%s\"",name);
        }
        return RET();
    }

    PlatformVector<ConstString> getNames() {
        return names;
    }
};

#endif

