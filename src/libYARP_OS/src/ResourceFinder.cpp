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


static Bottle parsePaths(const ConstString& txt) {
    char sep = NetworkBase::getPathSeparator()[0];
    Bottle result;
    const char *at = txt.c_str();
    int len = 0;
    for (int i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch==sep) {
            result.addString(ConstString(at,len));
            at += len+1;
            len = 0;
            continue;
        }
        len++;
    }
    if (len>0) {
        result.addString(ConstString(at,len));
    }
    return result;
}


static void appendResourceType(ConstString& path,
                               const ConstString& resourceType) {
    if (resourceType=="") return;
    ConstString slash = NetworkBase::getDirectorySeparator();
    path += NetworkBase::getDirectorySeparator();
    path += resourceType;
}

static void appendResourceType(Bottle& paths,
                               const ConstString& resourceType) {
    if (resourceType=="") return;
    for (int i=0; i<paths.size(); i++) {
        ConstString txt = paths.get(i).asString();
        appendResourceType(txt,resourceType);
        paths.get(i) = Value(txt);
    }
}


class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString configFilePath;
    yarp::os::ConstString policyName;
    bool verbose;
    bool quiet;
public:
    ResourceFinderHelper() {
        verbose = false;
        quiet = false;
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
            if (!quiet) {
                fprintf(RTARGET,"||| failed to load policy from%s\n",
                        checked.c_str());
            }
            return false;
        }

        // currently only support "capability" style configuration
        if (config.check("style",Value("")).asString()!="capability") {
            if (!quiet) {
                fprintf(RTARGET,"||| policy \"style\" can currently only be \"capability\"\n");
            }
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
                                bool isDir,
                                const char *doc) {
        ConstString s = getPath(base1,base2,base3,name);

        if (verbose) {
            fprintf(RTARGET,"||| checking %s (reason: %s)\n", s.c_str(), doc);
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

    yarp::os::Bottle findPaths(Property& config, const char *name,
                               const ResourceFinderOptions *externalOptions) {
        ConstString fname = config.check(name,Value(name)).asString();
        Bottle paths;
        if (externalOptions) {
            findFileBase(config,fname,true,paths,*externalOptions);
        } else {
            ResourceFinderOptions opts;
            opts.duplicateFilesPolicy = ResourceFinderOptions::All;
            findFileBase(config,fname,true,paths,opts);
        }
        return paths;
    }

    yarp::os::ConstString findPath(Property& config) {
        ConstString result = findFileBase(config,"",true);
		if (result=="") result = ".";
        return result;
    }

    yarp::os::ConstString findFile(Property& config, const char *name) {
        ConstString fname = config.check(name,Value(name)).asString();
        ConstString result = findFileBase(config,fname,false);
        return result;
    }

    yarp::os::ConstString findFileBase(Property& config, const char *name,
                                       bool isDir) {
        Bottle output;
        ResourceFinderOptions opts;
        findFileBase(config,name,isDir,output,opts);
        return output.get(0).asString();
    }

    void findFileBase(Property& config, const char *name,
                      bool isDir,
                      Bottle& output, const ResourceFinderOptions& opts) {
        ResourceFinderOptions::SearchLocations locs = opts.searchLocations;
        ResourceFinderOptions::SearchFlavor flavor = opts.searchFlavor;
        ConstString resourceType = opts.resourceType;

        bool justTop = (opts.duplicateFilesPolicy==ResourceFinderOptions::First);

        // check current directory
        if (locs & ResourceFinderOptions::Directory) {
            if (ConstString(name)==""&&isDir) {
                output.addString(".");
                if (justTop) return;
            }
            ConstString str = check("","","",name,isDir,"current directory");
            if (str!="") {
                output.addString(str);
                if (justTop) return;
            }
        }

        if (configFilePath!="") {
            ConstString str = check(configFilePath.c_str(),"","",name,isDir,"'from' path");
            if (str!="") {
                output.addString(str);
                if (justTop) return;
            }
        }

        if (locs & ResourceFinderOptions::ClassicContext) {
            ConstString cap =
                config.check("capability_directory",Value("app")).asString();
            Bottle defCaps =
                config.findGroup("default_capability").tail();

            // check app dirs
            for (int i=0; i<apps.size(); i++) {
                ConstString str = check(root.c_str(),cap,apps.get(i).asString().c_str(),
                                        name,isDir,"old-style context for backwards compatibility");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }

            // check ROOT/app/default/
            for (int i=0; i<defCaps.size(); i++) {
                ConstString str = check(root.c_str(),cap,defCaps.get(i).asString().c_str(),
                                        name,isDir, "old-style default context for backwards compatibility");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }
        }

        if (locs & ResourceFinderOptions::Context) {
            for (int i=0; i<apps.size(); i++) {
                ConstString app = apps.get(i).asString();

                // Nested search to locate context directories
                Bottle paths;
                ResourceFinderOptions opts2;
                opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ~ResourceFinderOptions::Context);
                findFileBase(config,app.c_str(),true,paths,opts2);
                appendResourceType(paths,resourceType);
                for (int j=0; j<paths.size(); j++) {
                    ConstString str = check(paths.get(j).asString().c_str(),"","",
                                            name,isDir,"context");
                    if (str!="") {
                        output.addString(str);
                        if (justTop) return;
                    }
                }
            }
        }

        // check YARP_CONFIG_HOME
        if ((locs & ResourceFinderOptions::User) &&
            (flavor & ResourceFinderOptions::ConfigLike)) {
            ConstString home = ResourceFinder::getConfigHome();
            if (home!="") {
                appendResourceType(home,resourceType);
                ConstString str = check(home.c_str(),"","",name,isDir,
                                        "YARP_CONFIG_HOME");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_DATA_HOME
        if ((locs & ResourceFinderOptions::User) &&
            (flavor & ResourceFinderOptions::DataLike)) {
            ConstString home = ResourceFinder::getDataHome();
            if (home!="") {
                appendResourceType(home,resourceType);
                ConstString str = check(home.c_str(),"","",name,isDir,
                                        "YARP_DATA_HOME");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_CONFIG_DIRS
        if (locs & ResourceFinderOptions::Sysadmin) {
            Bottle dirs = ResourceFinder::getConfigDirs();
            appendResourceType(dirs,resourceType);
            for (int i=0; i<dirs.size(); i++) {
                ConstString str = check(dirs.get(i).asString().c_str(),
                                        "","",name,isDir,
                                        "YARP_CONFIG_DIRS");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_DATA_DIRS
        if (locs & ResourceFinderOptions::Installed) {
            Bottle dirs = ResourceFinder::getDataDirs();
            appendResourceType(dirs,resourceType);
            for (int i=0; i<dirs.size(); i++) {
                ConstString str = check(dirs.get(i).asString().c_str(),
                                        "","",name,isDir,
                                        "YARP_DATA_DIRS");
                if (str!="") {
                    output.addString(str);
                    if (justTop) return;
                }
            }
        }

        if (locs & ResourceFinderOptions::Installed) {
            // Nested search to locate path.d directories
            Bottle pathds;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ~(ResourceFinderOptions::Installed | ResourceFinderOptions::Context));
            findFileBase(config,"path.d",true,pathds,opts2);

            for (int i=0; i<pathds.size(); i++) {
                // check /.../path.d/*
                // this directory is expected to contain *.ini files like this:
                //   [search BUNDLE_NAME]
                //   path /PATH1 /PATH2
                // for example:
                //   [search icub]
                //   path /usr/share/iCub
                Property pathd;
                pathd.fromConfigFile(pathds.get(i).asString());
                Bottle sections = pathd.findGroup("search").tail();
                for (int i=0; i<sections.size(); i++) {
                    ConstString search_name = sections.get(i).asString();
                    Bottle group = pathd.findGroup(search_name);
                    Bottle paths = group.findGroup("path").tail();
                    appendResourceType(paths,resourceType);
                    for (int j=0; j<paths.size(); j++) {
                        ConstString str = check(paths.get(j).asString().c_str(),"","",
                                                name,isDir,"yarp.d");
                        if (str!="") {
                            output.addString(str);
                            if (justTop) return;
                        }
                    }
                }
            }
        }

        if (justTop) {
            if (!quiet) {
                fprintf(RTARGET,"||| did not find %s\n", name);
            }
        }
    }

    bool setVerbose(bool verbose) {
        this->verbose = verbose;
        return this->verbose;
    }

    bool setQuiet(bool quiet) {
        this->quiet = quiet;
        return this->quiet;
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

yarp::os::Bottle ResourceFinder::findPaths(const char *name) {
    return HELPER(implementation).findPaths(config,name,NULL);
}

yarp::os::Bottle ResourceFinder::findPaths(const char *name,
                                           const ResourceFinderOptions& options) {
    return HELPER(implementation).findPaths(config,name,&options);
}

yarp::os::ConstString ResourceFinder::findPath() {
    return HELPER(implementation).findPath(config);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}

bool ResourceFinder::setQuiet(bool quiet) {
    return HELPER(implementation).setQuiet(quiet);
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


ConstString ResourceFinder::getDataHome() {
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    ConstString yarp_version = NetworkBase::getEnvironment("YARP_DATA_HOME",
                                                           &found);
    if (yarp_version != "") return yarp_version;
    ConstString xdg_version = NetworkBase::getEnvironment("XDG_DATA_HOME",
                                                          &found);
    if (found) return xdg_version + slash + "yarp";
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return app_version + slash + "yarp";
    }
#endif
    ConstString home_version = NetworkBase::getEnvironment("HOME");
    if (home_version != "") {
        return home_version
            + slash + ".local"
            + slash + "share"
            + slash + "yarp";
    }
    return "";
}


ConstString ResourceFinder::getConfigHome() {
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    ConstString yarp_version = NetworkBase::getEnvironment("YARP_CONFIG_HOME",
                                                           &found);
    if (found) return yarp_version;
    ConstString xdg_version = NetworkBase::getEnvironment("XDG_CONFIG_HOME",
                                                          &found);
    if (found) return xdg_version + slash + "yarp";
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return app_version + slash + "yarp" + slash + "config";
    }
#endif
    ConstString home_version = NetworkBase::getEnvironment("HOME");
    if (home_version != "") {
        return home_version
            + slash + ".config"
            + slash + "yarp";
    }
    return "";
}


Bottle ResourceFinder::getDataDirs() {
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_DATA_DIRS",
                                                                 &found));
    if (found) return yarp_version;
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_DATA_DIRS",
                                                                &found));
    if (found) {
        for (int i=0; i<xdg_version.size(); i++) {
            xdg_version.get(i) = Value(xdg_version.get(i).asString() +
                                       slash + "yarp");
        }
        return xdg_version;
    }
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("YARP_DIR");
    if (app_version != "") {
        Bottle result;
        result.addString(app_version + slash + "share" + slash + "yarp");
        return result;
    }
#endif
    Bottle result;
    result.addString("/usr/local/share/yarp");
    result.addString("/usr/share/yarp");
    return result;
}


Bottle ResourceFinder::getConfigDirs() {
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_CONFIG_DIRS",
                                                                 &found));
    if (found) return yarp_version;
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_CONFIG_DIRS",
                                                                &found));
    if (found) {
        for (int i=0; i<xdg_version.size(); i++) {
            xdg_version.get(i) = Value(xdg_version.get(i).asString() +
                                       slash + "yarp");
        }
        return xdg_version;
    }
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("ALLUSERSPROFILE");
    if (app_version != "") {
        Bottle result;
        result.addString(app_version + slash + "yarp");
        return result;
    }
#endif
    Bottle result;
    result.addString("/etc/yarp");
    return result;
}

