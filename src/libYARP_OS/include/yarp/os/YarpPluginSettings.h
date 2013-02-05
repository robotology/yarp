// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPPLUGINSETTINGS_
#define _YARP2_YARPPLUGINSETTINGS_

#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/SharedLibraryClass.h>

namespace yarp {
    namespace os {
        class YarpPluginSettings;
    }
}


/**
 *
 * Collect hints for finding a particular plugin.
 *
 */
class YARP_OS_API yarp::os::YarpPluginSettings {
public:
    ConstString name;
    ConstString dll_name;
    ConstString fn_name;
    ConstString fn_ext;
    YarpPluginSelector *selector;
    bool verbose;

    YarpPluginSettings() {
        verbose = false;
        selector = 0 /*NULL*/;
    }

    YarpPluginSettings(YarpPluginSelector& selector,
                       Searchable& options,
                       const ConstString& xname) {
        this->selector = &selector;
        readFromSearchable(options,xname);
    }

    bool readFromSelector(const char *name) {
        if (!selector) return false;
        Bottle plugins = selector->getSelectedPlugins();
        Bottle group = plugins.findGroup(name).tail();
        readFromSearchable(group,name);
        return true;
    }

    bool readFromSearchable(Searchable& options, const char *name) {
        ConstString iname = options.find("library").toString().c_str();
        ConstString pname = options.find("part").toString().c_str();
        if (iname=="") iname = name;
        if (pname=="") pname = name;
        this->name = iname.c_str();
        this->dll_name = iname;
        this->fn_name = pname;
        verbose = false;
        return true;
    }

    bool open(SharedLibraryFactory& factory);
    void reportStatus(SharedLibraryFactory& factory) const;
    void reportFailure() const;

private:
    bool subopen(SharedLibraryFactory& factory, const ConstString& dll_name,
                 const ConstString& fn_name);

    bool open(SharedLibraryFactory& factory, const ConstString& dll_name,
              const ConstString& fn_name);
};

#endif
