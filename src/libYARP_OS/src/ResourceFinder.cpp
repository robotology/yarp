// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/NetInt32.h>
#include <yarp/Logger.h>
#include <yarp/String.h>

#include <ace/OS.h>

using namespace yarp::os;
using namespace yarp;

#define RTARGET stderr

class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString policyName;
    yarp::os::Property config;
    bool verbose;
public:
    yarp::os::Property& getConfig() { return config; }

    ResourceFinderHelper() {
        verbose = false;
    }

    bool addAppName(const char *appName) {
        apps.addString(appName);
        return true;
    }
    
    bool clearAppNames() {
        apps.clear();
        return true;
    }
    
    bool configureFromPolicy(const char *policyName) {
        this->policyName = policyName;
        if (verbose) {
            fprintf(RTARGET,"||| policy set to %s\n", policyName);
        }
        String rootVar = policyName;
        const char *result = 
            ACE_OS::getenv(rootVar.c_str());
        bool needEnv = false;
#ifdef YARP2_WINDOWS
        needEnv = true;
#endif
        if (result==NULL) {
            root = "";
        } else {
            root = result;
        }
        root = config.check(policyName,Value(root)).asString().c_str();
        if (root == "") {
            if (verbose||needEnv) {
                fprintf(RTARGET,"||| environment variable %s not set\n", 
                        rootVar.c_str());
            }
            if (needEnv) {
                return false;
            }
        } else {
            if (verbose) {
                fprintf(RTARGET,"||| %s: %s\n", 
                        rootVar.c_str(),root.c_str());
            }
        }
        String checked = "";
        String rootConfig = String(root.c_str()) + "/" + policyName + ".ini";
        String altConfig = String("/etc/") + policyName + ".ini";
        bool ok = false;
        if (root!="") {
            if (verbose) {
                fprintf(RTARGET,"||| loading policy from %s\n", 
                        rootConfig.c_str());
            }
            checked += " " + rootConfig;
            ok = config.fromConfigFile(rootConfig.c_str());
        }
        if (!needEnv) {
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n", 
                            altConfig.c_str());
                }
                checked += " " + altConfig;
                ok = config.fromConfigFile(altConfig.c_str());
            }
        }
        if (!ok) {
            if (verbose) {
                fprintf(RTARGET,"||| in desperation, loading policy from %s\n",
                        policyName);
            }
            checked += " ";
            checked += policyName;
            ok = config.fromConfigFile(policyName);
        }
        if (!ok) {
            fprintf(RTARGET,"||| failed to load policy from%s\n", 
                    checked.c_str());
            return false;
        }

        // currently only support "capability" style configuration
        if (config.check("style",Value("")).asString()!="capability") {
            fprintf(RTARGET,"||| policy \"style\" can currently only be \"capability\"\n");
            return false;
        }

        return true;
    }

    bool configure(const char *policyName, int argc, char *argv[], bool skip) {
        Property p;
        p.fromCommand(argc,argv,skip);

        //printf("SETTINGS: %s\n", p.toString().c_str());

        if (p.check("verbose")) {
            setVerbose(p.check("verbose",Value(1)).asInt());
        }

        ConstString name = "";
        if (policyName!=NULL) {
            name = policyName;
        }
        name = p.check("policy",Value(name.c_str())).asString();
        if (name=="") {
            fprintf(RTARGET,"||| no policy found\n");
            return false;
        }

        config = p;
        bool result = configureFromPolicy(name.c_str());
        if (!result) return result;

        if (p.check("context")) {
            addAppName(p.check("context",Value("default")).asString().c_str());
        }

        config.fromCommand(argc,argv,skip,false);
        if (config.check("from")) {
            ConstString from = config.check("from",
                                            Value("config.ini")).toString();
            ConstString corrected = findFile(from.c_str());
            if (corrected!="") {
                from = corrected;
            }
            config.fromConfigFile(from,false);
        }
        return true;
    }

    bool setDefault(const char *key, const char *val) {
        if (!config.check(key)) {
            config.put(key,val);
        }
        return true;
    }


    yarp::os::ConstString check(const char *base1, 
                                const char *base2, 
                                const char *base3, 
                                const char *name) {
        String s = "";
        if (base1!=NULL) {
            s = base1;
            if (String(base1)!="") {
                s = s + "/";
            }
        }
        if (base2!=NULL) {
            s = s + base2;
            if (String(base2)!="") {
                s = s + "/";
            }
        }
        if (base3!=NULL) {
            s = s + base3;
            if (String(base3)!="") {
                s = s + "/";
            }
        }
        s = s + name;
        if (verbose) {
            fprintf(RTARGET,"||| checking %s\n", s.c_str());
        }
        if (exists(s.c_str())) {
            fprintf(RTARGET,"||| found %s\n", s.c_str());
            return s.c_str();
        }
        return "";
    }

    yarp::os::ConstString findFile(const char *name) {
        // name is now a key
        //printf("Status %s\n", config.toString().c_str());
        //printf("name %s\n", name);
        ConstString fname = config.check(name,Value(name)).asString();
        //printf("fname %s\n", fname.c_str());
        ConstString result = findFileBase(fname);
        return result;
    }

    yarp::os::ConstString findFileBase(const char *name) {

        ConstString cap = 
            config.check("capability_directory",Value("app")).asString();
        ConstString defCap = 
            config.check("default_capability",Value("default")).asString();

        // check current directory
        ConstString str = check("","","",name);
        if (str!="") return str;

        // check ROOT/app/default/
        str = check(root.c_str(),cap,defCap,name);
        if (str!="") return str;

        // check app dirs
        for (int i=0; i<apps.size(); i++) {
            str = check(root.c_str(),cap,apps.get(i).asString().c_str(),
                        name);
            if (str!="") return str;
        }

        fprintf(RTARGET,"||| did not find %s\n", name);
        return "";
    }

    bool setVerbose(bool verbose) {
        this->verbose = verbose;
        return this->verbose;
    }

    bool exists(const char *fname) {
        ACE_stat s;
        int result = ACE_OS::stat(fname,&s);
        return (result==0);
	}
};

#define HELPER(x) (*((ResourceFinderHelper*)(x)))
#define CONFIG(x) ((*((ResourceFinderHelper*)(x))).getConfig())

ResourceFinder::ResourceFinder() {
    implementation = new ResourceFinderHelper;
    YARP_ASSERT(implementation!=NULL);
}

ResourceFinder::~ResourceFinder() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


bool ResourceFinder::configure(const char *policyName, int argc, char *argv[],
                               bool skipFirstArgument) {
    return HELPER(implementation).configure(policyName,argc,argv,
                                            skipFirstArgument);
}

bool ResourceFinder::addContext(const char *appName) {
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearContext() {
    return HELPER(implementation).clearAppNames();
}

bool ResourceFinder::setDefault(const char *key, const char *val) {
    return HELPER(implementation).setDefault(key,val);
}


yarp::os::ConstString ResourceFinder::findFile(const char *name) {
    return HELPER(implementation).findFile(name);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}



bool ResourceFinder::check(const char *key) {
    return CONFIG(implementation).check(key);
}


Value& ResourceFinder::find(const char *key) {
    return CONFIG(implementation).find(key);
}


Bottle& ResourceFinder::findGroup(const char *key) {
    return CONFIG(implementation).findGroup(key);
}


bool ResourceFinder::isNull() const {
    return CONFIG(implementation).isNull();
}


ConstString ResourceFinder::toString() const {
    return CONFIG(implementation).toString();
}


