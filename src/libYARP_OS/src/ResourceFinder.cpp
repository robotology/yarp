/// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/Os.h>
#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::os::impl;

#define RTARGET stderr


static ConstString expandUserFileName(const char *fname) {
    ConstString root = NetworkBase::getEnvironment("YARP_CONF");
    ConstString home = NetworkBase::getEnvironment("HOME");
    ConstString homepath = NetworkBase::getEnvironment("HOMEPATH");
    ConstString conf = "";
    if (root!="") {
        conf = root + "/" + fname;
    } else if (homepath!="") {
        conf = NetworkBase::getEnvironment("HOMEDRIVE") + homepath + "\\yarp\\" + fname;
    } else if (home!="") {
        conf = home + "/.yarp/" + fname;
    } else {
        YARP_ERROR(Logger::get(),"Cannot read configuration - please set YARP_CONF or HOME or HOMEPATH");
        ACE_OS::exit(1);
    }
    YARP_DEBUG(Logger::get(),(ConstString("Configuration file: ") + conf).c_str());
    return conf;
}


class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString configFilePath;
    yarp::os::ConstString policyName;
    bool verbose;
public:
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

    static ConstString extractPath(const char *fname) {
        String s = fname;
        YARP_STRING_INDEX n = s.rfind('/');
        if (n == String::npos) {
            n = s.rfind('\\');
        }
        if (n != String::npos) {
            s[n] = '\0';
            return ConstString(s.c_str());
        }
        return "";
    }

    bool configureFromPolicy(Property& config, const char *policyName) {
        this->policyName = policyName;
        if (verbose) {
            fprintf(RTARGET,"||| policy set to %s\n", policyName);
        }
        String rootVar = policyName;
        const char *result =
            yarp::os::getenv(rootVar.c_str());
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
        String userConfig = expandUserFileName(ConstString(policyName) + ".ini").c_str();
        String rootConfig = String(root.c_str()) + "/" + policyName + ".ini";
        String altConfig = String("/etc/yarp/policies/") + policyName + ".ini";
#ifndef YARP_NO_DEPRECATED
        String deprecatedConfig = String("/etc/") + policyName + ".ini"; // FIXME Deprecated
#endif // YARP_NO_DEPRECATED
        bool ok = false;
        if (!ok) {
            if (root!="") {
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n",
                            rootConfig.c_str());
                }
                checked += " " + rootConfig;
                ok = config.fromConfigFile(rootConfig.c_str(),false);
            }
        }
        if (!needEnv) {
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n",
                            userConfig.c_str());
                }
                checked += " " + userConfig;
                ok = config.fromConfigFile(userConfig.c_str(),false);
            }
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n",
                            altConfig.c_str());
                }
                checked += " " + altConfig;
                ok = config.fromConfigFile(altConfig.c_str(),false);
            }
#ifndef YARP_NO_DEPRECATED
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n",
                            deprecatedConfig.c_str());
                }
                checked += " " + deprecatedConfig;
                ok = config.fromConfigFile(deprecatedConfig.c_str(),false);
                if (ok) {
                    fprintf(RTARGET, "||| WARNING: Loading policies from /etc/ is deprecated,\n"
                                     "|||          you should move them in /etc/yarp/policies/ .\n");
                }
            }
#endif // YARP_NO_DEPRECATED
            if (!ok) {
                altConfig = String("/usr/local/etc/") + policyName + ".ini";
                if (verbose) {
                    fprintf(RTARGET,"||| loading policy from %s\n",
                            altConfig.c_str());
                }
                checked += " " + altConfig;
                ok = config.fromConfigFile(altConfig.c_str(),false);
            }
        }
        /*
          // this would violate the spec
        if (!ok) {
            if (verbose) {
                fprintf(RTARGET,"||| in desperation, loading policy from %s\n",
                        policyName);
            }
            checked += " ";
            checked += policyName;
            ok = config.fromConfigFile(policyName);
        }
        */
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

    bool configure(Property& config, const char *policyName, int argc,
                   char *argv[], bool skip) {
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
            const char *result =
                yarp::os::getenv("YARP_POLICY");
            if (result!=NULL) {
                if (verbose) {
                    fprintf(RTARGET,"||| Read policy from YARP_POLICY\n");
                }
                name = result;
            }
        }
        bool skip_policy = false;
        if (name=="") {
            if (verbose) {
                fprintf(RTARGET,"||| no policy found\n");
            }
            skip_policy = true;
        }
        if (name=="none") {
            skip_policy = true;
        }

        if (!skip_policy) {
            config.fromString(p.toString().c_str(),false);
            bool result = configureFromPolicy(config,name.c_str());
            if (!result) return result;
        }

        if (p.check("context")) {
            clearAppNames();
            ConstString c = p.check("context",Value("default")).asString();
            addAppName(c.c_str());
            if (verbose) {
                fprintf(RTARGET,"||| added context %s\n",
                        c.c_str());
            }
        }

        config.fromCommand(argc,argv,skip,false);
        if (config.check("from")) {
            ConstString from = config.check("from",
                                            Value("config.ini")).toString();
            if (verbose) {
                fprintf(RTARGET,"||| default config file specified as %s\n",
                        from.c_str());
            }
            ConstString corrected = findFile(config,from.c_str());
            if (corrected!="") {
                from = corrected;
            }
            ConstString fromPath = extractPath(from.c_str());
            configFilePath = fromPath;
            config.fromConfigFile(from,false);
            config.fromCommand(argc,argv,skip,false);
        }
        return true;
    }

    bool setDefault(Property& config, const char *key, const char *val) {
        if (!config.check(key)) {
            config.put(key,val);
        }
        return true;
    }

    bool isAbsolute(const char *path) {
        if (path[0]=='/'||path[0]=='\\') {
            return true;
        }
        ConstString str(path);
        if (str.length()>1) {
            if (str[1]==':') {
                return true;
            }
        }
        return false;
    }

    bool isRooted(const char *path) {
        if (isAbsolute(path)) return true;
        ConstString str(path);
        if (str.length()>=2) {
            if (str[0]=='.'&&(str[1]=='/'||str[1]=='\\')) {
                return true;
            }
        } else if (str==".") {
            return true;
        }
        return false;
    }

    yarp::os::ConstString getPath(const char *base1,
                                  const char *base2,
                                  const char *base3,
                                  const char *name) {
        ConstString s = "";
        if (base1!=NULL) {
            s = base1;
            if (ConstString(base1)!="") {
                s = s + "/";
            }
        }
        if (base2!=NULL) {
            if (isRooted(base2)) {
                s = base2;
            } else {
                s = s + base2;
            }
            if (ConstString(base2)!="") {
                s = s + "/";
            }
        }
        if (base3!=NULL) {
            if (isRooted(base3)) {
                s = base3;
            } else {
                s = s + base3;
            }
            if (ConstString(base3)!="") {
                s = s + "/";
            }
        }

        s = s + name;

        return s;
    }

    yarp::os::ConstString check(const char *base1,
                                const char *base2,
                                const char *base3,
                                const char *name,
                                bool isDir) {
        ConstString s = getPath(base1,base2,base3,name);

        if (verbose) {
            fprintf(RTARGET,"||| checking %s\n", s.c_str());
        }
        if (exists(s.c_str(),isDir)) {
            if (verbose) {
                fprintf(RTARGET,"||| found %s\n", s.c_str());
            }
            return s.c_str();
        }
        return "";
    }

    yarp::os::ConstString findPath(Property& config, const char *name) {
        ConstString fname = config.check(name,Value(name)).asString();
        ConstString result = findFileBase(config,fname,true);
        return result;
    }

    yarp::os::ConstString findPath(Property& config) {
        ConstString result = findFileBase(config,"",true);
		if (result=="") result = ".";
        return result;
    }

    yarp::os::ConstString findFile(Property& config, const char *name) {
        // name is now a key
        //printf("Status %s\n", config.toString().c_str());
        //printf("name %s\n", name);
        ConstString fname = config.check(name,Value(name)).asString();
        //printf("fname %s\n", fname.c_str());
        ConstString result = findFileBase(config,fname,false);
        return result;
    }

    yarp::os::ConstString findFileBase(Property& config, const char *name,
                                       bool isDir) {

        ConstString cap =
            config.check("capability_directory",Value("app")).asString();
        Bottle defCaps =
            config.findGroup("default_capability").tail();

        // check current directory
		if (ConstString(name)==""&&isDir) return ".";
        ConstString str = check("","","",name,isDir);
        if (str!="") return str;

        if (configFilePath!="") {
            ConstString str = check(configFilePath.c_str(),"","",name,isDir);
            if (str!="") return str;
        }

        // check app dirs
        for (int i=0; i<apps.size(); i++) {
            str = check(root.c_str(),cap,apps.get(i).asString().c_str(),
                        name,isDir);
            if (str!="") return str;
        }

        // check ROOT/app/default/
        for (int i=0; i<defCaps.size(); i++) {
            str = check(root.c_str(),cap,defCaps.get(i).asString().c_str(),
                        name,isDir);
            if (str!="") return str;
        }

        fprintf(RTARGET,"||| did not find %s\n", name);
        return "";
    }

    bool setVerbose(bool verbose) {
        this->verbose = verbose;
        return this->verbose;
    }

    bool exists(const char *fname, bool isDir) {
        ACE_stat s;
        int result = ACE_OS::stat(fname,&s);
        if (result!=0) {
            return false;
        }
        if (!isDir) {
            // if not required to be a directory, pass anything.
            return true;
        }

        // ACE doesn't seem to help us interpret the results of stat
        // in a portable fashion.

        // ACE on Ubuntu 9.10 has issues.
        // Suppressing check for file here since it isn't really needed
        // and causes a lot of problems.
        /*
        ACE_DIR *dir = ACE_OS::opendir(fname);
        if (dir!=NULL) {
            ACE_OS::closedir(dir);
            dir = NULL;
            return true;
        }
        return false;
        */
        return true;
	}


    ConstString getContext() {
        return apps.get(0).asString();
    }

    Bottle getContexts() {
        return apps;
    }

    ConstString context2path(Property& config, const ConstString& context ) {
        ConstString cap =
            config.check("capability_directory",Value("app")).asString();
        ConstString path = getPath(root,cap,context,"");
        if (path.length()>1) {
            if (path[path.length()-1]=='/') {
                path = path.substr(0,path.length()-1);
            }
        }
        return path;
    }
};

#define HELPER(x) (*((ResourceFinderHelper*)(x)))

ResourceFinder::ResourceFinder() {
    implementation = new ResourceFinderHelper();
    YARP_ASSERT(implementation!=NULL);
    owned = true;
    nullConfig = false;
    isConfiguredFlag = false;
}

ResourceFinder::ResourceFinder(Searchable& data, void *implementation) {
    this->implementation = implementation;
    if (!data.isNull()) {
        config.fromString(data.toString());
    }
    nullConfig = data.isNull();
    owned = false;
    isConfiguredFlag = true;
}

ResourceFinder::~ResourceFinder() {
    if (implementation!=NULL) {
        if (owned) {
            delete &HELPER(implementation);
        }
        implementation = NULL;
    }
}


bool ResourceFinder::configure(const char *policyName, int argc, char *argv[],
                               bool skipFirstArgument) {
    isConfiguredFlag = true;
    return HELPER(implementation).configure(config,policyName,argc,argv,
                                            skipFirstArgument);
}

bool ResourceFinder::addContext(const char *appName) {
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearContext() {
    return HELPER(implementation).clearAppNames();
}

bool ResourceFinder::setDefault(const char *key, const char *val) {
    return HELPER(implementation).setDefault(config,key,val);
}


yarp::os::ConstString ResourceFinder::findFile(const char *name) {
    return HELPER(implementation).findFile(config,name);
}

yarp::os::ConstString ResourceFinder::findPath(const char *name) {
    return HELPER(implementation).findPath(config,name);
}

yarp::os::ConstString ResourceFinder::findPath() {
    return HELPER(implementation).findPath(config);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}



bool ResourceFinder::check(const char *key) {
    return config.check(key);
}


Value& ResourceFinder::find(const char *key) {
    return config.find(key);
}


Bottle& ResourceFinder::findGroup(const char *key) {
    return config.findGroup(key);
}


bool ResourceFinder::isNull() const {
    return nullConfig||config.isNull();
}


ConstString ResourceFinder::toString() const {
    return config.toString();
}

ConstString ResourceFinder::getContext() {
    return HELPER(implementation).getContext();
}

ConstString ResourceFinder::getContextPath() {
    return HELPER(implementation).context2path(config,
                                               HELPER(implementation).getContext());
}

Bottle ResourceFinder::getContexts() {
    return HELPER(implementation).getContexts();
}


ResourceFinder ResourceFinder::findNestedResourceFinder(const char *key) {
    return ResourceFinder(findGroup(key),implementation);
}


ResourceFinder& ResourceFinder::getResourceFinderSingleton() {
    return NameClient::getNameClient().getResourceFinder();
}
