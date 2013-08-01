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
#include <yarp/os/Time.h>

#include <errno.h>

using namespace yarp::os;
using namespace yarp::os::impl;

#define RTARGET stderr
#define RESOURCE_FINDER_CACHE_TIME 10

static ConstString expandUserFileName(const ConstString& fname) {
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


static ConstString getPwd() {
    ConstString result;
    int len = 5;
    char *buf = NULL;
    while (true) {
        if (buf!=NULL) delete[] buf;
        buf = new char[len];
        if (!buf) break;
        char *dir = ACE_OS::getcwd(buf,len);
        if (dir) {
            result = dir;
            break;
        }
        if (errno!=ERANGE) break;
        len *= 2;
    }
    if (buf!=NULL) delete[] buf;
    buf = NULL;
    return result;
}


static Bottle parsePaths(const ConstString& txt) {
    char slash = NetworkBase::getDirectorySeparator()[0];
    char sep = NetworkBase::getPathSeparator()[0];
    Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (ConstString::size_type i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch==sep) {
            result.addString(ConstString(at,len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(ConstString(at,len-slash_tweak));
    }
    return result;
}


static void appendResourceType(ConstString& path,
                               const ConstString& resourceType) {
    if (resourceType=="") return;
    ConstString slash = NetworkBase::getDirectorySeparator();
    if (path.length()>0) {
        if (path[path.length()-1] != slash[0]) {
            path += NetworkBase::getDirectorySeparator();
        }
    }
    path += resourceType;
}

static void prependResourceType(ConstString& path,
                                const ConstString& resourceType) {
    if (resourceType=="") return;
    ConstString slash = NetworkBase::getDirectorySeparator();
    path = resourceType + NetworkBase::getDirectorySeparator() + path;
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
    yarp::os::Property cache;
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

    bool isVerbose() const {
        return verbose;
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

        bool configured_normally = true;
        if (!skip_policy) {
            config.fromString(p.toString().c_str(),false);
            configured_normally = configureFromPolicy(config,name.c_str());
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
            ConstString corrected = findFile(config,from.c_str(),NULL);
            if (corrected!="") {
                from = corrected;
            }
            ConstString fromPath = extractPath(from.c_str());
            configFilePath = fromPath;
            config.fromConfigFile(from,false);
            config.fromCommand(argc,argv,skip,false);
        }
        return configured_normally;
    }

    bool setDefault(Property& config, const char *key, const char *val) {
        if (!config.check(key)) {
            config.put(key,val);
        }
        return true;
    }

    bool isAbsolute(const ConstString& str) {
        if (str[0]=='/'||str[0]=='\\') {
            return true;
        }
        if (str.length()>1) {
            if (str[1]==':') {
                return true;
            }
        }
        return false;
    }

    bool isRooted(const ConstString& str) {
        if (isAbsolute(str)) return true;
        if (str.length()>=2) {
            if (str[0]=='.'&&(str[1]=='/'||str[1]=='\\')) {
                return true;
            }
        } else if (str==".") {
            return true;
        }
        return false;
    }

    yarp::os::ConstString getPath(const ConstString& base1,
                                  const ConstString& base2,
                                  const ConstString& base3,
                                  const ConstString& name) {
        if (isAbsolute(name)) {
            return name;
        }

        ConstString s = "";

        if (base1!="") {
            s = base1;
            s = s + "/";
        }

        if (isRooted(base2)) {
            s = base2;
        } else {
            s = s + base2;
        }
        if (base2!="") {
            s = s + "/";
        }

        if (isRooted(base3)) {
            s = base3;
        } else {
            s = s + base3;
        }
        if (base3!="") {
            s = s + "/";
        }

        s = s + name;

        return s;
    }

    yarp::os::ConstString check(const ConstString& base1,
                                const ConstString& base2,
                                const ConstString& base3,
                                const ConstString& name,
                                bool isDir,
                                const Bottle& doc,
                                const char *doc2) {
        ConstString s = getPath(base1,base2,base3,name);

        // check cache first
        Bottle *prev = cache.find(s).asList();
        if (prev!=NULL) {
            double t = prev->get(0).asDouble();
            int flag = prev->get(1).asInt();
            if (Time::now()-t<RESOURCE_FINDER_CACHE_TIME) {
                if (flag) return s;
                return "";
            }
        }

        if (verbose) {
            ConstString base = doc.toString();
            fprintf(RTARGET,"||| checking [%s] (%s%s%s)\n", s.c_str(), 
                    base.c_str(),
                    (base.length()==0) ? "" : " ",
                    doc2);
        }
        bool ok = exists(s.c_str(),isDir);
        Value status;
        YARP_ASSERT(status.asList());
        status.asList()->addDouble(Time::now());
        status.asList()->addInt(ok?1:0);
        cache.put(s,status);
        if (ok) {
            if (verbose) {
                fprintf(RTARGET,"||| found %s\n", s.c_str());
            }
            return s;
        }
        return "";
    }

    yarp::os::ConstString findPath(Property& config, const ConstString& name,
                                   const ResourceFinderOptions *externalOptions) {
        ConstString fname = config.check(name,Value(name)).asString();
        ConstString result = findFileBase(config,fname,true,externalOptions);
        return result;
    }

    yarp::os::Bottle findPaths(Property& config, const ConstString& name,
                               const ResourceFinderOptions *externalOptions,
                               bool enforcePlural = true) {
        ConstString fname = config.check(name,Value(name)).asString();
        Bottle paths;
        if (externalOptions) {
            if (externalOptions->duplicateFilesPolicy == ResourceFinderOptions::All) {
                findFileBase(config,fname,true,paths,*externalOptions);
                return paths;
            }
        }
        ResourceFinderOptions opts;
        if (externalOptions) {
            opts = *externalOptions;
        }
        if (enforcePlural) {
            opts.duplicateFilesPolicy = ResourceFinderOptions::All;
        }
        findFileBase(config,fname,true,paths,opts);
        return paths;
    }

    yarp::os::ConstString findPath(Property& config) {
        ConstString result = findFileBase(config,"",true,NULL);
		if (result=="") result = getPwd();
        return result;
    }

    yarp::os::ConstString findFile(Property& config, const ConstString& name,
                                   const ResourceFinderOptions *externalOptions) {
        ConstString fname = config.check(name,Value(name)).asString();
        ConstString result = findFileBase(config,fname,false,externalOptions);
        return result;
    }

    yarp::os::ConstString findFileBase(Property& config, const ConstString& name,
                                       bool isDir, 
                                       const ResourceFinderOptions *externalOptions) {
        Bottle output;
        ResourceFinderOptions opts;
        if (externalOptions==NULL) externalOptions = &opts;
        findFileBase(config,name,isDir,output,*externalOptions);
        return output.get(0).asString();
    }

    void findFileBase(Property& config, const ConstString& name,
                      bool isDir,
                      Bottle& output, const ResourceFinderOptions& opts) {
        Bottle doc;
        int prelen = output.size();
        findFileBaseInner(config,name,isDir,true,output,opts,doc,NULL);
        if (output.size()!=prelen) return;
        bool justTop = (opts.duplicateFilesPolicy==ResourceFinderOptions::First);
        if (justTop) {
            if (!quiet) {
                fprintf(RTARGET,"||| did not find %s\n", name.c_str());
            }
        }
    }

    void addString(Bottle& output, const ConstString& txt) {
        for (int i=0; i<output.size(); i++) {
            if (txt == output.get(i).asString()) return;
        }
        output.addString(txt);
    }

    void findFileBaseInner(Property& config, const ConstString& name,
                           bool isDir, bool allowPathd,
                           Bottle& output, const ResourceFinderOptions& opts,
                           const Bottle& predoc, const char *reason) {
        Bottle doc;
        if (verbose) {
            doc = predoc;
            if (reason) {
                doc.addString(reason);
            }
        }
        ResourceFinderOptions::SearchLocations locs = opts.searchLocations;
        ResourceFinderOptions::SearchFlavor flavor = opts.searchFlavor;
        ConstString resourceType = opts.resourceType;

        bool justTop = (opts.duplicateFilesPolicy==ResourceFinderOptions::First);

        // check current directory
        if (locs & ResourceFinderOptions::Directory) {
            if (name==""&&isDir) {
                addString(output,getPwd());
                if (justTop) return;
            }
            ConstString str = check(getPwd(),resourceType,"",name,isDir,doc,"pwd");
            if (str!="") {
                addString(output,str);
                if (justTop) return;
            }
        }

        if (locs & ResourceFinderOptions::NearMainConfig) {
            if (configFilePath!="") {
                ConstString str = check(configFilePath.c_str(),resourceType,"",name,isDir,doc,"defaultConfigFile path");
                if (str!="") {
                    addString(output,str);
                    if (justTop) return;
                }
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
                                        name,isDir,doc,"deprecated-old-style-context");
                if (str!="") {
                    addString(output,str);
                    if (justTop) return;
                }
            }

            // check ROOT/app/default/
            for (int i=0; i<defCaps.size(); i++) {
                ConstString str = check(root.c_str(),cap,defCaps.get(i).asString().c_str(),
                                        name,isDir,doc,"deprecated-old-style-context");
                if (str!="") {
                    addString(output,str);
                    if (justTop) return;
                }
            }
        }

        if (locs & ResourceFinderOptions::Context) {
            for (int i=0; i<apps.size(); i++) {
                ConstString app = apps.get(i).asString();

                // New context still apparently applies only to "applications"
                // which means we need to restrict our attention to "app"
                // directories.  

                // Nested search to locate context directory
                Bottle paths;
                ResourceFinderOptions opts2;
                prependResourceType(app,"contexts");
                opts2.searchLocations = (ResourceFinderOptions::SearchLocations)ResourceFinderOptions::Default;
                findFileBaseInner(config,app.c_str(),true,allowPathd,paths,opts2,doc,"context");
                appendResourceType(paths,resourceType);
                for (int j=0; j<paths.size(); j++) {
                    ConstString str = check(paths.get(j).asString().c_str(),"","",
                                            name,isDir,doc,"context");
                    if (str!="") {
                        addString(output,str);
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
                                        doc,"YARP_CONFIG_HOME");
                if (str!="") {
                    addString(output,str);
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
                                        doc,"YARP_DATA_HOME");
                if (str!="") {
                    addString(output,str);
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
                                        doc,"YARP_CONFIG_DIRS");
                if (str!="") {
                    addString(output,str);
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
                                        doc,"YARP_DATA_DIRS");
                if (str!="") {
                    addString(output,str);
                    if (justTop) return;
                }
            }
        }

        if (allowPathd && (locs & ResourceFinderOptions::Installed)) {
            // Nested search to locate path.d directories
            Bottle pathds;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ~(ResourceFinderOptions::Context|ResourceFinderOptions::NearMainConfig));
            opts2.resourceType = "config";
            findFileBaseInner(config,"path.d",true,false,pathds,opts2,doc,"path.d");

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
                                                name,isDir,doc,"yarp.d");
                        if (str!="") {
                            addString(output,str);
                            if (justTop) return;
                        }
                    }
                }
            }
        }
        
        if (locs & ResourceFinderOptions::Robot) {
            ConstString slash = NetworkBase::getDirectorySeparator();
            bool found = false;
            ConstString robot = NetworkBase::getEnvironment("YARP_ROBOT_NAME",
                                                            &found);
            if (!found) robot = "default";

            // Nested search to locate robot directory
            Bottle paths;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(ResourceFinderOptions::User | ResourceFinderOptions::Sysadmin | ResourceFinderOptions::Installed);
            //opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ~(ResourceFinderOptions::Robot|ResourceFinderOptions::Context|ResourceFinderOptions::ClassicContext|ResourceFinderOptions::NearMainConfig));
            opts2.resourceType = "robots";
            findFileBaseInner(config,robot.c_str(),true,allowPathd,paths,opts2,doc,"robot");
            appendResourceType(paths,resourceType);
            for (int j=0; j<paths.size(); j++) {
                ConstString str = check(paths.get(j).asString().c_str(),
                                        "","",
                                        name,isDir,doc,"robot");
                if (str!="") {
                    addString(output,str);
                    if (justTop) return;
                }
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
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| configuring\n");
    }
    return HELPER(implementation).configure(config,policyName,argc,argv,
                                            skipFirstArgument);
}

bool ResourceFinder::addContext(const char *appName) {
    if (appName[0]=='\0') return true;
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| adding context [%s]\n", appName);
    }
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearContext() {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| clearing context\n");
    }
    return HELPER(implementation).clearAppNames();
}

bool ResourceFinder::setDefault(const char *key, const char *val) {
    return HELPER(implementation).setDefault(config,key,val);
}

yarp::os::ConstString ResourceFinder::findFile(const ConstString& name) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding file [%s]\n", name.c_str());
    }
    return HELPER(implementation).findFile(config,name,NULL);
}

yarp::os::ConstString ResourceFinder::findFile(const ConstString& name,
                                               const ResourceFinderOptions& options) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding file [%s]\n", name.c_str());
    }
    return HELPER(implementation).findFile(config,name,&options);
}

yarp::os::ConstString ResourceFinder::findPath(const ConstString& name) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding path [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPath(config,name,NULL);
}

yarp::os::ConstString ResourceFinder::findPath(const ConstString& name,
                                               const ResourceFinderOptions& options) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding path [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPath(config,name,&options);
}

yarp::os::Bottle ResourceFinder::findPaths(const ConstString& name) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding paths [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPaths(config,name,NULL);
}

yarp::os::Bottle ResourceFinder::findPaths(const ConstString& name,
                                           const ResourceFinderOptions& options) {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding paths [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPaths(config,name,&options);
}

yarp::os::ConstString ResourceFinder::findPath() {
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET,"||| finding path\n");
    }
    return HELPER(implementation).findPath(config);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}

bool ResourceFinder::setQuiet(bool quiet) {
    return HELPER(implementation).setQuiet(quiet);
}



bool ResourceFinder::check(const ConstString& key) {
    return config.check(key);
}


Value& ResourceFinder::find(const ConstString& key) {
    return config.find(key);
}


Bottle& ResourceFinder::findGroup(const ConstString& key) {
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
#ifdef __APPLE__
    if (home_version != "") {
        return home_version
            + slash + "Library"
            + slash + "Application Support"
            + slash + "yarp";
    }
#endif
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

#ifdef __APPLE__
    ConstString home_mac_version= getDataHome();
    if (home_mac_version != "") {
        return home_mac_version
            + slash + "config";
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
        appendResourceType(xdg_version,"yarp");
        return xdg_version;
    }
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("YARP_DIR");
    if (app_version != "") {
        appendResourceType(app_version,"share");
        appendResourceType(app_version,"yarp");
        Bottle result;
        result.addString(app_version);
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
        appendResourceType(xdg_version,"yarp");
        return xdg_version;
    }
#ifdef _WIN32
    ConstString app_version = NetworkBase::getEnvironment("ALLUSERSPROFILE");
    if (app_version != "") {
        appendResourceType(app_version,"yarp");
        Bottle result;
        result.addString(app_version);
        return result;
    }
#endif

    Bottle result;
#ifdef __APPLE__
    result.addString("/Library/Preferences/yarp");
#endif
    result.addString("/etc/yarp");
    return result;
}



bool ResourceFinder::readConfig(Property& config,
                                const ConstString& key,
                                const ResourceFinderOptions& options) {
    Bottle bot = HELPER(implementation).findPaths(config,key,&options,false);

    for (int i=bot.size()-1; i>=0; i--) {
        ConstString fname = bot.get(i).asString();
        config.fromConfigFile(fname,false);
    }

    return bot.size()>=1;
}
