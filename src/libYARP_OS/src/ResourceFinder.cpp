/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/ResourceFinder.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformSysStat.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>

using namespace yarp::os;
using namespace yarp::os::impl;

#define RTARGET stderr
#define RESOURCE_FINDER_CACHE_TIME 10

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
YARP_DEPRECATED static ConstString expandUserFileName(const ConstString& fname) {
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
        YARP_ERROR(Logger::get(), "Cannot read configuration - please set YARP_CONF or HOME or HOMEPATH");
        std::exit(1);
    }
    YARP_DEBUG(Logger::get(), (ConstString("Configuration file: ") + conf).c_str());
    return conf;
}
#endif // YARP_NO_DEPRECATED


static ConstString getPwd() {
    ConstString result;
    int len = 5;
    char *buf = YARP_NULLPTR;
    while (true) {
        if (buf!=YARP_NULLPTR) delete[] buf;
        buf = new char[len];
        if (!buf) break;
        char *dir = yarp::os::getcwd(buf, len);
        if (dir) {
            result = dir;
            break;
        }
        if (errno!=ERANGE) break;
        len *= 2;
    }
    if (buf!=YARP_NULLPTR) delete[] buf;
    buf = YARP_NULLPTR;
    return result;
}


static Bottle parsePaths(const ConstString& txt) {
    if (txt.empty()) {
        return Bottle();
    }
    char slash = NetworkBase::getDirectorySeparator()[0];
    char sep = NetworkBase::getPathSeparator()[0];
    Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (ConstString::size_type i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch==sep) {
            result.addString(ConstString(at, len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(ConstString(at, len-slash_tweak));
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
        appendResourceType(txt, resourceType);
        paths.get(i) = Value(txt);
    }
}


class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    yarp::os::ConstString root;
#endif // YARP_NO_DEPRECATED
    yarp::os::ConstString configFilePath;
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    yarp::os::ConstString policyName;
#endif // YARP_NO_DEPRECATED
    yarp::os::Property cache;
    bool verbose;
    bool quiet;
    bool mainActive;
    bool useNearMain;
public:
    ResourceFinderHelper() {
        verbose = false;
        quiet = false;
        mainActive = false;
        useNearMain = false;
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
        ConstString s = fname;
        size_t n = s.rfind('/');
        if (n == ConstString::npos) {
            n = s.rfind('\\');
        }
        if (n != ConstString::npos) {
            s[n] = '\0';
            return ConstString(s.c_str());
        }
        return "";
    }

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    YARP_DEPRECATED bool configureFromPolicy(Property& config, const char *policyName) {
        this->policyName = policyName;
        if (verbose) {
            fprintf(RTARGET, "||| policy set to %s\n", policyName);
        }
        ConstString rootVar = policyName;
        const char *result =
            yarp::os::getenv(rootVar.c_str());
        bool needEnv = false;
#ifdef YARP2_WINDOWS
        needEnv = true;
#endif
        if (result==YARP_NULLPTR) {
            root = "";
        } else {
            root = result;
        }
        root = config.check(policyName, Value(root)).asString().c_str();
        if (root == "") {
            if (verbose||needEnv) {
                fprintf(RTARGET, "||| environment variable %s not set\n",
                        rootVar.c_str());
            }
            if (needEnv) {
                return false;
            }
        } else {
            if (verbose) {
                fprintf(RTARGET, "||| %s: %s\n",
                        rootVar.c_str(), root.c_str());
            }
        }
        ConstString checked = "";
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
        ConstString userConfig = expandUserFileName(ConstString(policyName) + ".ini").c_str();
YARP_WARNING_POP
        ConstString rootConfig = ConstString(root.c_str()) + "/" + policyName + ".ini";
        ConstString altConfig = ConstString("/etc/yarp/policies/") + policyName + ".ini";
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.21
        ConstString deprecatedConfig = ConstString("/etc/") + policyName + ".ini"; // FIXME Deprecated
#endif // YARP_NO_DEPRECATED
        bool ok = false;
        if (!ok) {
            if (root!="") {
                if (verbose) {
                    fprintf(RTARGET, "||| loading policy from %s\n",
                            rootConfig.c_str());
                }
                checked += " " + rootConfig;
                ok = config.fromConfigFile(rootConfig.c_str(), false);
            }
        }
        if (!needEnv) {
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET, "||| loading policy from %s\n",
                            userConfig.c_str());
                }
                checked += " " + userConfig;
                ok = config.fromConfigFile(userConfig.c_str(), false);
            }
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET, "||| loading policy from %s\n",
                            altConfig.c_str());
                }
                checked += " " + altConfig;
                ok = config.fromConfigFile(altConfig.c_str(), false);
            }
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.21
            if (!ok) {
                if (verbose) {
                    fprintf(RTARGET, "||| loading policy from %s\n",
                            deprecatedConfig.c_str());
                }
                checked += " " + deprecatedConfig;
                ok = config.fromConfigFile(deprecatedConfig.c_str(), false);
                if (ok) {
                    fprintf(RTARGET, "||| WARNING: Loading policies from /etc/ is deprecated, \n"
                                     "|||          you should move them in /etc/yarp/policies/ .\n");
                }
            }
#endif // YARP_NO_DEPRECATED
            if (!ok) {
                altConfig = ConstString("/usr/local/etc/") + policyName + ".ini";
                if (verbose) {
                    fprintf(RTARGET, "||| loading policy from %s\n",
                            altConfig.c_str());
                }
                checked += " " + altConfig;
                ok = config.fromConfigFile(altConfig.c_str(), false);
            }
        }
        /*
          // this would violate the spec
        if (!ok) {
            if (verbose) {
                fprintf(RTARGET, "||| in desperation, loading policy from %s\n",
                        policyName);
            }
            checked += " ";
            checked += policyName;
            ok = config.fromConfigFile(policyName);
        }
        */
        if (!ok) {
            if (!quiet) {
                fprintf(RTARGET, "||| failed to load policy from%s\n",
                        checked.c_str());
            }
            return false;
        }

        // currently only support "capability" style configuration
        if (config.check("style", Value("")).asString()!="capability") {
            if (!quiet) {
                fprintf(RTARGET, "||| policy \"style\" can currently only be \"capability\"\n");
            }
            return false;
        }

        return true;
    }
#endif

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    YARP_DEPRECATED bool configure(Property& config, const char *policyName, int argc,
                   char *argv[], bool skip) {
        if (argc>0) {
            if (argv[0]!=YARP_NULLPTR) {
                yarp::os::setprogname(argv[0]);
            }
        }

        Property p;
        p.fromCommand(argc, argv, skip);

        bool user_specified_from = p.check("from");

        if (p.check("verbose")) {
            setVerbose(p.check("verbose", Value(1)).asInt());
        }

        if (isVerbose()) {
            fprintf(RTARGET, "||| configuring\n");
        }

        ConstString name = "";
        if (policyName!=YARP_NULLPTR) {
            name = policyName;
        }
        name = p.check("policy", Value(name.c_str())).asString();
        if (name=="") {
            const char *result =
                yarp::os::getenv("YARP_POLICY");
            if (result!=YARP_NULLPTR) {
                if (verbose) {
                    fprintf(RTARGET, "||| Read policy from YARP_POLICY\n");
                }
                name = result;
            }
        }
        bool skip_policy = false;
        if (name=="") {
            if (verbose) {
                fprintf(RTARGET, "||| no policy found\n");
            }
            skip_policy = true;
        }
        if (name=="none") {
            skip_policy = true;
        }

        bool configured_normally = true;
        if (!skip_policy) {
            config.fromString(p.toString().c_str(), false);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
            configured_normally = configureFromPolicy(config, name.c_str());
YARP_WARNING_POP
        }

        if (p.check("context")) {
            clearAppNames();
            ConstString c = p.check("context", Value("default")).asString();
            addAppName(c.c_str());
            if (verbose) {
                fprintf(RTARGET, "||| added context %s\n",
                        c.c_str());
            }
        }

        config.fromCommand(argc, argv, skip, false);
        if (config.check("from")) {
            ConstString from = config.check("from",
                                            Value("config.ini")).toString();
            if (verbose) {
                fprintf(RTARGET, "||| default config file specified as %s\n",
                        from.c_str());
            }
            mainActive = true;
            ConstString corrected = findFile(config, from.c_str(), YARP_NULLPTR);
            mainActive = false;
            if (corrected!="") {
                from = corrected;
            }
            ConstString fromPath = extractPath(from.c_str());
            configFilePath = fromPath;
            if (!config.fromConfigFile(from, false) && user_specified_from) {
                configured_normally = false;
            }
            config.fromCommand(argc, argv, skip, false);
        }
        return configured_normally;
    }
#endif // YARP_NO_DEPRECATED

    bool configure(Property& config, int argc, char *argv[], bool skip) {
        if (argc>0) {
            if (argv[0]!=YARP_NULLPTR) {
                yarp::os::setprogname(argv[0]);
            }
        }

        Property p;
        p.fromCommand(argc, argv, skip);

        bool user_specified_from = p.check("from");

        if (p.check("verbose")) {
            setVerbose(p.check("verbose", Value(1)).asInt());
        }

        if (isVerbose()) {
            fprintf(RTARGET, "||| configuring\n");
        }

        bool configured_normally = true;

        if (p.check("context")) {
            clearAppNames();
            ConstString c = p.check("context", Value("default")).asString();
            addAppName(c.c_str());
            if (verbose) {
                fprintf(RTARGET, "||| added context %s\n",
                        c.c_str());
            }
        }

        config.fromCommand(argc, argv, skip, false);
        if (config.check("from")) {
            ConstString from = config.check("from",
                                            Value("config.ini")).toString();
            if (verbose) {
                fprintf(RTARGET, "||| default config file specified as %s\n",
                        from.c_str());
            }
            mainActive = true;
            ConstString corrected = findFile(config, from.c_str(), YARP_NULLPTR);
            mainActive = false;
            if (corrected!="") {
                from = corrected;
            }
            ConstString fromPath = extractPath(from.c_str());
            configFilePath = fromPath;
            if (!config.fromConfigFile(from, false) && user_specified_from) {
                configured_normally = false;
            }
            config.fromCommand(argc, argv, skip, false);
        }
        return configured_normally;
    }

    bool setDefault(Property& config, const char *key, const yarp::os::Value &val) {
        if (!config.check(key)) {
            config.put(key, val);
        }
        return true;
    }

    bool isAbsolute(const ConstString& str) {
        if (str.length()>0 && (str[0]=='/'||str[0]=='\\')) {
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
        ConstString slash = NetworkBase::getDirectorySeparator();

        if (base1!="") {
            s = base1;
            s = s + slash;
        }

        if (isRooted(base2)) {
            s = base2;
        } else {
            s = s + base2;
        }
        if (base2!="") {
            s = s + slash;
        }

        if (isRooted(base3)) {
            s = base3;
        } else {
            s = s + base3;
        }
        if (base3!="") {
            s = s + slash;
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
        ConstString s = getPath(base1, base2, base3, name);

        // check cache first
        Bottle *prev = cache.find(s).asList();
        if (prev!=YARP_NULLPTR) {
            double t = prev->get(0).asDouble();
            int flag = prev->get(1).asInt();
            if (SystemClock::nowSystem()-t<RESOURCE_FINDER_CACHE_TIME) {
                if (flag) return s;
                return "";
            }
        }

        if (verbose) {
            ConstString base = doc.toString();
            fprintf(RTARGET, "||| checking [%s] (%s%s%s)\n", s.c_str(),
                    base.c_str(),
                    (base.length()==0) ? "" : " ",
                    doc2);
        }
        bool ok = exists(s.c_str(), isDir);
        Value status;
        yAssert(status.asList());
        status.asList()->addDouble(SystemClock::nowSystem());
        status.asList()->addInt(ok?1:0);
        cache.put(s, status);
        if (ok) {
            if (verbose) {
                fprintf(RTARGET, "||| found %s\n", s.c_str());
            }
            return s;
        }
        return "";
    }

    yarp::os::ConstString findPath(Property& config, const ConstString& name,
                                   const ResourceFinderOptions *externalOptions) {
        ConstString fname = config.check(name, Value(name)).asString();
        ConstString result = findFileBase(config, fname, true, externalOptions);
        return result;
    }

    yarp::os::Bottle findPaths(Property& config, const ConstString& name,
                               const ResourceFinderOptions *externalOptions,
                               bool enforcePlural = true) {
        ConstString fname = config.check(name, Value(name)).asString();
        Bottle paths;
        if (externalOptions) {
            if (externalOptions->duplicateFilesPolicy == ResourceFinderOptions::All) {
                findFileBase(config, fname, true, paths, *externalOptions);
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
        findFileBase(config, fname, true, paths, opts);
        return paths;
    }

    yarp::os::ConstString findPath(Property& config) {
        ConstString result = findFileBase(config, "", true, YARP_NULLPTR);
		if (result=="") result = getPwd();
        return result;
    }

    yarp::os::ConstString findFile(Property& config, const ConstString& name,
                                   const ResourceFinderOptions *externalOptions) {
        ConstString fname = config.check(name, Value(name)).asString();
        ConstString result = findFileBase(config, fname, false, externalOptions);
        return result;
    }

    yarp::os::ConstString findFileByName(Property& config, const ConstString& fname,
                                   const ResourceFinderOptions *externalOptions) {
        ConstString result = findFileBase(config, fname, false, externalOptions);
        return result;
    }

    yarp::os::ConstString findFileBase(Property& config, const ConstString& name,
                                       bool isDir,
                                       const ResourceFinderOptions *externalOptions) {
        Bottle output;
        ResourceFinderOptions opts;
        if (externalOptions==YARP_NULLPTR) externalOptions = &opts;
        findFileBase(config, name, isDir, output, *externalOptions);
        return output.get(0).asString();
    }

    bool canShowErrors(const ResourceFinderOptions& opts) const {
        if (opts.messageFilter & ResourceFinderOptions::ShowErrors) {
            return true;
        }
        if (opts.messageFilter == ResourceFinderOptions::ShowNone) return false;
        return !quiet;
    }

    void findFileBase(Property& config, const ConstString& name,
                      bool isDir,
                      Bottle& output, const ResourceFinderOptions& opts) {
        Bottle doc;
        int prelen = output.size();
        findFileBaseInner(config, name, isDir, true, output, opts, doc, YARP_NULLPTR);
        if (output.size()!=prelen) return;
        bool justTop = (opts.duplicateFilesPolicy==ResourceFinderOptions::First);
        if (justTop) {
            if (canShowErrors(opts)) {
                fprintf(RTARGET, "||| did not find %s\n", name.c_str());
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
                addString(output, getPwd());
                if (justTop) return;
            }
            ConstString str = check(getPwd(), resourceType, "", name, isDir, doc, "pwd");
            if (str!="") {
                if (mainActive) useNearMain = true;
                addString(output, str);
                if (justTop) return;
            }
        }

        if ((locs & ResourceFinderOptions::NearMainConfig) && useNearMain) {
            if (configFilePath!="") {
                ConstString str = check(configFilePath.c_str(), resourceType, "", name, isDir, doc, "defaultConfigFile path");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
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
            opts2.resourceType = "robots";
            opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
            findFileBaseInner(config, robot.c_str(), true, allowPathd, paths, opts2, doc, "robot");
            appendResourceType(paths, resourceType);
            for (int j=0; j<paths.size(); j++) {
                ConstString str = check(paths.get(j).asString().c_str(),
                                        "", "",
                                        name, isDir, doc, "robot");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
        if ((locs & ResourceFinderOptions::ClassicContext) &&
            (!useNearMain) &&
            root != "") {
            ConstString cap =
                config.check("capability_directory", Value("app")).asString();
            Bottle defCaps =
                config.findGroup("default_capability").tail();

            // check app dirs
            for (int i=0; i<apps.size(); i++) {
                ConstString str = check(root.c_str(), cap, apps.get(i).asString().c_str(),
                                        name, isDir, doc, "deprecated-old-style-context");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }

            // check ROOT/app/default/
            for (int i=0; i<defCaps.size(); i++) {
                ConstString str = check(root.c_str(), cap, defCaps.get(i).asString().c_str(),
                                        name, isDir, doc, "deprecated-old-style-context");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }
#endif // YARP_NO_DEPRECATED

        if ((locs & ResourceFinderOptions::Context) && !useNearMain) {
            for (int i=0; i<apps.size(); i++) {
                ConstString app = apps.get(i).asString();

                // New context still apparently applies only to "applications"
                // which means we need to restrict our attention to "app"
                // directories.

                // Nested search to locate context directory
                Bottle paths;
                ResourceFinderOptions opts2;
                prependResourceType(app, "contexts");
                opts2.searchLocations = (ResourceFinderOptions::SearchLocations)ResourceFinderOptions::Default;
                opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
                findFileBaseInner(config, app.c_str(), true, allowPathd, paths, opts2, doc, "context");
                appendResourceType(paths, resourceType);
                for (int j=0; j<paths.size(); j++) {
                    ConstString str = check(paths.get(j).asString().c_str(), "", "",
                                            name, isDir, doc, "context");
                    if (str!="") {
                        addString(output, str);
                        if (justTop) return;
                    }
                }
            }
        }

        // check YARP_CONFIG_HOME
        if ((locs & ResourceFinderOptions::User) &&
            (flavor & ResourceFinderOptions::ConfigLike)) {
            ConstString home = ResourceFinder::getConfigHomeNoCreate();
            if (home!="") {
                appendResourceType(home, resourceType);
                ConstString str = check(home.c_str(), "", "", name, isDir,
                                        doc, "YARP_CONFIG_HOME");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_DATA_HOME
        if ((locs & ResourceFinderOptions::User) &&
            (flavor & ResourceFinderOptions::DataLike)) {
            ConstString home = ResourceFinder::getDataHomeNoCreate();
            if (home!="") {
                appendResourceType(home, resourceType);
                ConstString str = check(home.c_str(), "", "", name, isDir,
                                        doc, "YARP_DATA_HOME");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_CONFIG_DIRS
        if (locs & ResourceFinderOptions::Sysadmin) {
            Bottle dirs = ResourceFinder::getConfigDirs();
            appendResourceType(dirs, resourceType);
            for (int i=0; i<dirs.size(); i++) {
                ConstString str = check(dirs.get(i).asString().c_str(),
                                        "", "", name, isDir,
                                        doc, "YARP_CONFIG_DIRS");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }

        // check YARP_DATA_DIRS
        if (locs & ResourceFinderOptions::Installed) {
            Bottle dirs = ResourceFinder::getDataDirs();
            appendResourceType(dirs, resourceType);
            for (int i=0; i<dirs.size(); i++) {
                ConstString str = check(dirs.get(i).asString().c_str(),
                                        "", "", name, isDir,
                                        doc, "YARP_DATA_DIRS");
                if (str!="") {
                    addString(output, str);
                    if (justTop) return;
                }
            }
        }

        if (allowPathd && (locs & ResourceFinderOptions::Installed)) {
            // Nested search to locate path.d directories
            Bottle pathds;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ResourceFinderOptions::Installed);
            opts2.resourceType = "config";
            findFileBaseInner(config, "path.d", true, false, pathds, opts2, doc, "path.d");

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
                    appendResourceType(paths, resourceType);
                    for (int j=0; j<paths.size(); j++) {
                        ConstString str = check(paths.get(j).asString().c_str(), "", "",
                                                name, isDir, doc, "yarp.d");
                        if (str!="") {
                            addString(output, str);
                            if (justTop) return;
                        }
                    }
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
        int result = yarp::os::stat(fname);
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
        YARP_DIR *dir = ACE_OS::opendir(fname);
        if (dir!=YARP_NULLPTR) {
            ACE_OS::closedir(dir);
            dir = YARP_NULLPTR;
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

    ConstString getHomeContextPath(Property& config, const ConstString& context )
    {
        if (useNearMain)
            return configFilePath;
        else
        {
            ConstString path = getPath(ResourceFinder::getDataHome(), "contexts", context, "");

            ConstString slash = NetworkBase::getDirectorySeparator();
            if (path.length()>1) {
              if (path[path.length()-1] == slash[0]) {
                path = path.substr(0, path.length()-slash.size());
                }
            }

            ConstString parentPath=getPath(ResourceFinder::getDataHome(), "contexts", "", "");
            if (yarp::os::stat(parentPath.c_str()))
                yarp::os::mkdir(parentPath.c_str());

            if (yarp::os::mkdir(path.c_str()) <0 && errno != EEXIST)
                fprintf(RTARGET, "||| WARNING  Could not create %s directory\n", path.c_str());
            return path;
        }
    }

    ConstString getHomeRobotPath()
    {
        if (useNearMain)
            return configFilePath;
        bool found = false;
        ConstString robot = NetworkBase::getEnvironment("YARP_ROBOT_NAME",
                                                            &found);
        if (!found) robot = "default";
        ConstString path = getPath(ResourceFinder::getDataHome(), "robots", robot, "");

        ConstString slash = NetworkBase::getDirectorySeparator();
        if (path.length()>1) {
            if (path[path.length()-1]==slash[0]) {
            path = path.substr(0, path.length()-slash.size());
            }
        }

        ConstString parentPath=getPath(ResourceFinder::getDataHome(), "robots", "", "");
        if (yarp::os::stat(parentPath.c_str()))
            yarp::os::mkdir(parentPath.c_str());

        if (yarp::os::mkdir(path.c_str()) <0 && errno != EEXIST)
            fprintf(RTARGET, "||| WARNING  Could not create %s directory\n", path.c_str());
        return path;
    }

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.60
    ConstString context2path(Property& config, const ConstString& context ) {
        if (useNearMain)
            return configFilePath;
        else
        {
            if (root != "") //configured with policy
            {
                ConstString cap =
                config.check("capability_directory", Value("app")).asString();
                ConstString path = getPath(root, cap, context, "");
                ConstString slash = NetworkBase::getDirectorySeparator();
                if (path.length()>1) {
                    if (path[path.length()-1]==slash[0]) {
                        path = path.substr(0, path.length()-slash.size());
                    }
                }
                return path;
            }
            return getHomeContextPath(config, context);
        }
    }
#endif // YARP_NO_DEPRECATED
};

#define HELPER(x) (*((ResourceFinderHelper*)(x)))

ResourceFinder::ResourceFinder()
{
    // We need some pieces of YARP to be initialized.
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    implementation = new ResourceFinderHelper();
    yAssert(implementation!=YARP_NULLPTR);
    owned = true;
    nullConfig = false;
    isConfiguredFlag = false;
}

ResourceFinder::ResourceFinder(const ResourceFinder& alt) :
        Searchable(alt)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    implementation = new ResourceFinderHelper();
    yAssert(implementation!=YARP_NULLPTR);
    owned = true;
    nullConfig = false;
    isConfiguredFlag = false;
    *this = alt;
}

ResourceFinder::ResourceFinder(Searchable& data, void *implementation)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    this->implementation = implementation;
    if (!data.isNull()) {
        config.fromString(data.toString());
    }
    nullConfig = data.isNull();
    owned = false;
    isConfiguredFlag = true;
}

ResourceFinder::~ResourceFinder()
{
    if (implementation!=YARP_NULLPTR) {
        if (owned) {
            delete &HELPER(implementation);
        }
        implementation = YARP_NULLPTR;
    }
}


const ResourceFinder& ResourceFinder::operator= (const ResourceFinder& alt)
{
    HELPER(implementation) = HELPER(alt.implementation);
    owned = true;
    nullConfig = alt.nullConfig;
    isConfiguredFlag = alt.isConfiguredFlag;
    config = alt.config;
    return *this;
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
bool ResourceFinder::configure(const char *policyName, int argc, char *argv[],
                               bool skipFirstArgument)
{
    isConfiguredFlag = true;
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
return HELPER(implementation).configure(config, policyName, argc, argv,
                                            skipFirstArgument);
YARP_WARNING_POP
}
#endif // YARP_NO_DEPRECATED

bool ResourceFinder::configure(int argc, char *argv[], bool skipFirstArgument)
{
    isConfiguredFlag = true;
    return HELPER(implementation).configure(config, argc, argv, skipFirstArgument);
}


bool ResourceFinder::addContext(const char *appName)
{
    if (appName[0]=='\0') return true;
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| adding context [%s]\n", appName);
    }
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearContext()
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| clearing context\n");
    }
    return HELPER(implementation).clearAppNames();
}

bool ResourceFinder::setDefault(const char *key, const yarp::os::ConstString& val)
{
    Value val2;
    val2.fromString(val.c_str());
    return HELPER(implementation).setDefault(config, key, val2);
}

bool ResourceFinder::setDefault(const char *key, const yarp::os::Value& val)
{
    return HELPER(implementation).setDefault(config, key, val);
}

yarp::os::ConstString ResourceFinder::findFile(const ConstString& name)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding file [%s]\n", name.c_str());
    }
    return HELPER(implementation).findFile(config, name, YARP_NULLPTR);
}

yarp::os::ConstString ResourceFinder::findFile(const ConstString& name,
                                               const ResourceFinderOptions& options)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding file [%s]\n", name.c_str());
    }
    return HELPER(implementation).findFile(config, name, &options);
}

yarp::os::ConstString ResourceFinder::findFileByName(const ConstString& name)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding file %s\n", name.c_str());
    }
    return HELPER(implementation).findFileByName(config, name, YARP_NULLPTR);
}

yarp::os::ConstString ResourceFinder::findFileByName(const ConstString& name,
                                                     const ResourceFinderOptions& options)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding file %s\n", name.c_str());
    }
    return HELPER(implementation).findFileByName(config, name, &options);
}


yarp::os::ConstString ResourceFinder::findPath(const ConstString& name)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding path [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPath(config, name, YARP_NULLPTR);
}

yarp::os::ConstString ResourceFinder::findPath(const ConstString& name,
                                               const ResourceFinderOptions& options)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding path [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPath(config, name, &options);
}

yarp::os::Bottle ResourceFinder::findPaths(const ConstString& name)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding paths [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPaths(config, name, YARP_NULLPTR);
}

yarp::os::Bottle ResourceFinder::findPaths(const ConstString& name,
                                           const ResourceFinderOptions& options)
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding paths [%s]\n", name.c_str());
    }
    return HELPER(implementation).findPaths(config, name, &options);
}

yarp::os::ConstString ResourceFinder::findPath()
{
    if (HELPER(implementation).isVerbose()) {
        fprintf(RTARGET, "||| finding path\n");
    }
    return HELPER(implementation).findPath(config);
}


bool ResourceFinder::setVerbose(bool verbose)
{
    return HELPER(implementation).setVerbose(verbose);
}

bool ResourceFinder::setQuiet(bool quiet)
{
    return HELPER(implementation).setQuiet(quiet);
}



bool ResourceFinder::check(const ConstString& key) const
{
    return config.check(key);
}


Value& ResourceFinder::find(const ConstString& key) const
{
    return config.find(key);
}


Bottle& ResourceFinder::findGroup(const ConstString& key) const
{
    return config.findGroup(key);
}


bool ResourceFinder::isNull() const
{
    return nullConfig||config.isNull();
}


ConstString ResourceFinder::toString() const
{
    return config.toString();
}

ConstString ResourceFinder::getContext()
{
    return HELPER(implementation).getContext();
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.60
ConstString ResourceFinder::getContextPath()
{
    return HELPER(implementation).context2path(config,
                                               HELPER(implementation).getContext());
}
#endif // YARP_NO_DEPRECATED

ConstString ResourceFinder::getHomeContextPath()
{
    return HELPER(implementation).getHomeContextPath(config,
                                               HELPER(implementation).getContext());
}

ConstString ResourceFinder::getHomeRobotPath()
{
    return HELPER(implementation).getHomeRobotPath();
}

Bottle ResourceFinder::getContexts()
{
    return HELPER(implementation).getContexts();
}


ResourceFinder ResourceFinder::findNestedResourceFinder(const char *key)
{
    return ResourceFinder(findGroup(key), implementation);
}


ResourceFinder& ResourceFinder::getResourceFinderSingleton()
{
    return NameClient::getNameClient().getResourceFinder();
}

ConstString ResourceFinder::getDataHomeWithPossibleCreation(bool mayCreate)
{
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    ConstString yarp_version = NetworkBase::getEnvironment("YARP_DATA_HOME",
                                                           &found);
    if (yarp_version != "") return yarp_version;
    ConstString xdg_version = NetworkBase::getEnvironment("XDG_DATA_HOME",
                                                          &found);
    if (found) return createIfAbsent(mayCreate, xdg_version + slash + "yarp");
#if defined(_WIN32)
    ConstString app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return createIfAbsent(mayCreate, app_version + slash + "yarp");
    }
#endif
    ConstString home_version = NetworkBase::getEnvironment("HOME");
#if defined(__APPLE__)
    if (home_version != "") {
        return createIfAbsent(mayCreate,
                              home_version
                              + slash + "Library"
                              + slash + "Application Support"
                              + slash + "yarp");
    }
#endif
    if (home_version != "") {
        return createIfAbsent(mayCreate,
                              home_version
                              + slash + ".local"
                              + slash + "share"
                              + slash + "yarp");
    }
    return "";
}


ConstString ResourceFinder::getConfigHomeWithPossibleCreation(bool mayCreate)
{
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    ConstString yarp_version = NetworkBase::getEnvironment("YARP_CONFIG_HOME",
                                                           &found);
    if (found) return yarp_version;
    ConstString xdg_version = NetworkBase::getEnvironment("XDG_CONFIG_HOME",
                                                          &found);
    if (found) return createIfAbsent(mayCreate, xdg_version + slash + "yarp");
#if defined(_WIN32)
    ConstString app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return createIfAbsent(mayCreate,
                              app_version + slash + "yarp" + slash + "config");
    }
#endif

#if defined(__APPLE__)
    ConstString home_mac_version = getDataHomeNoCreate();
    if (home_mac_version != "") {
        return createIfAbsent(mayCreate,
                              home_mac_version
                              + slash + "config");
    }
#endif
    ConstString home_version = NetworkBase::getEnvironment("HOME");
    if (home_version != "") {
        return createIfAbsent(mayCreate,
                              home_version
                              + slash + ".config"
                              + slash + "yarp");
    }
    return "";
}

ConstString ResourceFinder::createIfAbsent(bool mayCreate,
                                           const ConstString& path)
{
    if (!mayCreate) return path;
    NameConfig::createPath(path, 0);
    return path;
}

Bottle ResourceFinder::getDataDirs()
{
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_DATA_DIRS",
                                                                 &found));
    if (found) return yarp_version;
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_DATA_DIRS",
                                                                &found));
    if (found) {
        appendResourceType(xdg_version, "yarp");
        return xdg_version;
    }
#if defined(_WIN32)
    ConstString app_version = NetworkBase::getEnvironment("YARP_DIR");
    if (app_version != "") {
        appendResourceType(app_version, "share");
        appendResourceType(app_version, "yarp");
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


Bottle ResourceFinder::getConfigDirs()
{
    ConstString slash = NetworkBase::getDirectorySeparator();
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_CONFIG_DIRS",
                                                                 &found));
    if (found) return yarp_version;
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_CONFIG_DIRS",
                                                                &found));
    if (found) {
        appendResourceType(xdg_version, "yarp");
        return xdg_version;
    }
#if defined(_WIN32)
    ConstString app_version = NetworkBase::getEnvironment("ALLUSERSPROFILE");
    if (app_version != "") {
        appendResourceType(app_version, "yarp");
        Bottle result;
        result.addString(app_version);
        return result;
    }
#endif

    Bottle result;
#if defined(__APPLE__)
    result.addString("/Library/Preferences/yarp");
#endif
    result.addString("/etc/yarp");
    return result;
}



bool ResourceFinder::readConfig(Property& config,
                                const ConstString& key,
                                const ResourceFinderOptions& options)
{
    Bottle bot = HELPER(implementation).findPaths(config, key, &options, false);

    for (int i=bot.size()-1; i>=0; i--) {
        ConstString fname = bot.get(i).asString();
        config.fromConfigFile(fname, false);
    }

    return bot.size()>=1;
}
