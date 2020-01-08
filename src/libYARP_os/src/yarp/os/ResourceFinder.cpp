/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ResourceFinder.h>

#include <yarp/conf/filesystem.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformSysStat.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace yarp::os;
using namespace yarp::os::impl;
namespace fs = yarp::conf::filesystem;

#define RTARGET stderr
#define RESOURCE_FINDER_CACHE_TIME 10


static std::string getPwd()
{
    std::string result;
    int len = 5;
    char* buf = nullptr;
    while (true) {
        delete[] buf;
        buf = new char[len];
        if (buf == nullptr) {
            break;
        }
        char* dir = yarp::os::getcwd(buf, len);
        if (dir != nullptr) {
            result = dir;
            break;
        }
        if (errno != ERANGE) {
            break;
        }
        len *= 2;
    }
    delete[] buf;
    buf = nullptr;
    return result;
}


static Bottle parsePaths(const std::string& txt)
{
    if (txt.empty()) {
        return Bottle();
    }
    constexpr fs::value_type slash = fs::preferred_separator;
    constexpr fs::value_type sep   = fs::path_separator;
    Bottle result;
    const char* at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (char ch : txt) {
        if (ch == sep) {
            result.addString(std::string(at, len - slash_tweak));
            at += len + 1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch == slash && len > 0) ? 1 : 0;
        len++;
    }
    if (len > 0) {
        result.addString(std::string(at, len - slash_tweak));
    }
    return result;
}


static void appendResourceType(std::string& path,
                               const std::string& resourceType)
{
    if (resourceType.empty()) {
        return;
    }
    std::string slash{fs::preferred_separator};
    if (path.length() > 0) {
        if (path[path.length() - 1] != slash[0]) {
            path +=slash;
        }
    }
    path += resourceType;
}

static void prependResourceType(std::string& path,
                                const std::string& resourceType)
{
    if (resourceType.empty()) {
        return;
    }
    std::string slash{fs::preferred_separator};
    path = resourceType + slash + path;
}

static void appendResourceType(Bottle& paths,
                               const std::string& resourceType)
{
    if (resourceType.empty()) {
        return;
    }
    for (size_t i = 0; i < paths.size(); i++) {
        std::string txt = paths.get(i).asString();
        appendResourceType(txt, resourceType);
        paths.get(i) = Value(txt);
    }
}


class ResourceFinder::Private
{
private:
    yarp::os::Bottle apps;
    std::string configFilePath;
    yarp::os::Property cache;
    bool verbose{false};
    bool quiet{false};
    bool mainActive{false};
    bool useNearMain{false};

public:
    bool addAppName(const std::string& appName)
    {
        apps.addString(appName);
        return true;
    }

    bool clearAppNames()
    {
        apps.clear();
        return true;
    }

    bool isVerbose() const
    {
        return verbose;
    }

    static std::string extractPath(const std::string& fname)
    {
        std::string s{fname};
        auto n = s.rfind('/');
#if defined(_WIN32)
        if (n == std::string::npos) {
            n = s.rfind('\\');
        }
#endif
        if (n != std::string::npos) {
            return s.substr(0, n);
        }
        return {};
    }

    bool configure(Property& config, int argc, char* argv[], bool skip)
    {
        Property p;
        p.fromCommand(argc, argv, skip);

        bool user_specified_from = p.check("from");

        if (p.check("verbose")) {
            setVerbose(p.check("verbose", Value(1)).asBool());
        }

        if (isVerbose()) {
            fprintf(RTARGET, "||| configuring\n");
        }

        bool configured_normally = true;

        if (p.check("context")) {
            clearAppNames();
            std::string c = p.check("context", Value("default")).asString();
            addAppName(c.c_str());
            if (verbose) {
                fprintf(RTARGET, "||| added context %s\n", c.c_str());
            }
        }

        config.fromCommand(argc, argv, skip, false);
        if (config.check("from")) {
            std::string from = config.check("from",
                                            Value("config.ini"))
                                   .toString();
            if (verbose) {
                fprintf(RTARGET, "||| default config file specified as %s\n", from.c_str());
            }
            mainActive = true;
            std::string corrected = findFile(config, from, nullptr);
            mainActive = false;
            if (!corrected.empty()) {
                from = corrected;
            }
            std::string fromPath = extractPath(from.c_str());
            configFilePath = fromPath;
            if (!config.fromConfigFile(from, false) && user_specified_from) {
                configured_normally = false;
            }
            config.fromCommand(argc, argv, skip, false);
        }
        return configured_normally;
    }

    bool setDefault(Property& config, const std::string& key, const yarp::os::Value& val)
    {
        if (!config.check(key)) {
            config.put(key, val);
        }
        return true;
    }

    bool isAbsolute(const std::string& str)
    {
        if (str.length() > 0 && (str[0] == '/' || str[0] == '\\')) {
            return true;
        }
        if (str.length() > 1) {
            if (str[1] == ':') {
                return true;
            }
        }
        return false;
    }

    bool isRooted(const std::string& str)
    {
        if (isAbsolute(str)) {
            return true;
        }
        if (str.length() >= 2) {
            if (str[0] == '.' && (str[1] == '/' || str[1] == '\\')) {
                return true;
            }
        } else if (str == ".") {
            return true;
        }
        return false;
    }

    std::string getPath(const std::string& base1,
                        const std::string& base2,
                        const std::string& base3,
                        const std::string& name)
    {
        if (isAbsolute(name)) {
            return name;
        }

        std::string s;
        std::string slash{fs::preferred_separator};

        if (!base1.empty()) {
            s = base1;
            s = s + slash;
        }

        if (isRooted(base2)) {
            s = base2;
        } else {
            s = s + base2;
        }
        if (!base2.empty()) {
            s = s + slash;
        }

        if (isRooted(base3)) {
            s = base3;
        } else {
            s = s + base3;
        }
        if (!base3.empty()) {
            s = s + slash;
        }

        s = s + name;

        return s;
    }

    std::string check(const std::string& base1,
                      const std::string& base2,
                      const std::string& base3,
                      const std::string& name,
                      bool isDir,
                      const Bottle& doc,
                      const std::string& doc2)
    {
        std::string s = getPath(base1, base2, base3, name);

        // check cache first
        Bottle* prev = cache.find(s).asList();
        if (prev != nullptr) {
            double t = prev->get(0).asFloat64();
            int flag = prev->get(1).asInt32();
            if (SystemClock::nowSystem() - t < RESOURCE_FINDER_CACHE_TIME) {
                if (flag != 0) {
                    return s;
                }
                return {};
            }
        }

        if (verbose) {
            std::string base = doc.toString();
            fprintf(RTARGET, "||| checking [%s] (%s%s%s)\n", s.c_str(), base.c_str(), (base.length() == 0) ? "" : " ", doc2.c_str());
        }
        bool ok = exists(s.c_str(), isDir);
        Value status;
        yAssert(status.asList());
        status.asList()->addFloat64(SystemClock::nowSystem());
        status.asList()->addInt32(ok ? 1 : 0);
        cache.put(s, status);
        if (ok) {
            if (verbose) {
                fprintf(RTARGET, "||| found %s\n", s.c_str());
            }
            return s;
        }
        return {};
    }

    std::string findPath(Property& config, const std::string& name, const ResourceFinderOptions* externalOptions)
    {
        std::string fname = config.check(name, Value(name)).asString();
        std::string result = findFileBase(config, fname, true, externalOptions);
        return result;
    }

    yarp::os::Bottle findPaths(Property& config, const std::string& name, const ResourceFinderOptions* externalOptions, bool enforcePlural = true)
    {
        std::string fname = config.check(name, Value(name)).asString();
        Bottle paths;
        if (externalOptions != nullptr) {
            if (externalOptions->duplicateFilesPolicy == ResourceFinderOptions::All) {
                findFileBase(config, fname, true, paths, *externalOptions);
                return paths;
            }
        }
        ResourceFinderOptions opts;
        if (externalOptions != nullptr) {
            opts = *externalOptions;
        }
        if (enforcePlural) {
            opts.duplicateFilesPolicy = ResourceFinderOptions::All;
        }
        findFileBase(config, fname, true, paths, opts);
        return paths;
    }

    std::string findPath(Property& config)
    {
        std::string result = findFileBase(config, "", true, nullptr);
        if (result.empty()) {
            result = getPwd();
        }
        return result;
    }

    std::string findFile(Property& config, const std::string& name, const ResourceFinderOptions* externalOptions)
    {
        std::string fname = config.check(name, Value(name)).asString();
        std::string result = findFileBase(config, fname, false, externalOptions);
        return result;
    }

    std::string findFileByName(Property& config, const std::string& fname, const ResourceFinderOptions* externalOptions)
    {
        std::string result = findFileBase(config, fname, false, externalOptions);
        return result;
    }

    std::string findFileBase(Property& config, const std::string& name, bool isDir, const ResourceFinderOptions* externalOptions)
    {
        Bottle output;
        ResourceFinderOptions opts;
        if (externalOptions == nullptr) {
            externalOptions = &opts;
        }
        findFileBase(config, name, isDir, output, *externalOptions);
        return output.get(0).asString();
    }

    bool canShowErrors(const ResourceFinderOptions& opts) const
    {
        if ((opts.messageFilter & ResourceFinderOptions::ShowErrors) != 0) {
            return true;
        }
        if (opts.messageFilter == ResourceFinderOptions::ShowNone) {
            return false;
        }
        return !quiet;
    }

    void findFileBase(Property& config, const std::string& name, bool isDir, Bottle& output, const ResourceFinderOptions& opts)
    {
        Bottle doc;
        size_t prelen = output.size();
        findFileBaseInner(config, name, isDir, true, output, opts, doc, {});
        if (output.size() != prelen) {
            return;
        }
        bool justTop = (opts.duplicateFilesPolicy == ResourceFinderOptions::First);
        if (justTop) {
            if (canShowErrors(opts)) {
                fprintf(RTARGET, "||| did not find %s\n", name.c_str());
            }
        }
    }

    void addString(Bottle& output, const std::string& txt)
    {
        for (size_t i = 0; i < output.size(); i++) {
            if (txt == output.get(i).asString()) {
                return;
            }
        }
        output.addString(txt);
    }

    void findFileBaseInner(Property& config, const std::string& name, bool isDir, bool allowPathd, Bottle& output, const ResourceFinderOptions& opts, const Bottle& predoc, const std::string& reason)
    {
        Bottle doc;
        if (verbose) {
            doc = predoc;
            if (!reason.empty()) {
                doc.addString(reason);
            }
        }
        ResourceFinderOptions::SearchLocations locs = opts.searchLocations;
        ResourceFinderOptions::SearchFlavor flavor = opts.searchFlavor;
        std::string resourceType = opts.resourceType;

        bool justTop = (opts.duplicateFilesPolicy == ResourceFinderOptions::First);

        // check current directory
        if ((locs & ResourceFinderOptions::Directory) != 0) {
            if (name.empty() && isDir) {
                addString(output, getPwd());
                if (justTop) {
                    return;
                }
            }
            std::string str = check(getPwd(), resourceType, "", name, isDir, doc, "pwd");
            if (!str.empty()) {
                if (mainActive) {
                    useNearMain = true;
                }
                addString(output, str);
                if (justTop) {
                    return;
                }
            }
        }

        if (((locs & ResourceFinderOptions::NearMainConfig) != 0) && useNearMain) {
            if (!configFilePath.empty()) {
                std::string str = check(configFilePath, resourceType, "", name, isDir, doc, "defaultConfigFile path");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        if ((locs & ResourceFinderOptions::Robot) != 0) {
            std::string slash{fs::preferred_separator};
            bool found = false;
            std::string robot = NetworkBase::getEnvironment("YARP_ROBOT_NAME", &found);
            if (!found) {
                robot = "default";
            }

            // Nested search to locate robot directory
            Bottle paths;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(ResourceFinderOptions::User | ResourceFinderOptions::Sysadmin | ResourceFinderOptions::Installed);
            opts2.resourceType = "robots";
            opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
            findFileBaseInner(config, robot, true, allowPathd, paths, opts2, doc, "robot");
            appendResourceType(paths, resourceType);
            for (size_t j = 0; j < paths.size(); j++) {
                std::string str = check(paths.get(j).asString(),
                                        "",
                                        "",
                                        name,
                                        isDir,
                                        doc,
                                        "robot");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        if (((locs & ResourceFinderOptions::Context) != 0) && !useNearMain) {
            for (size_t i = 0; i < apps.size(); i++) {
                std::string app = apps.get(i).asString();

                // New context still apparently applies only to "applications"
                // which means we need to restrict our attention to "app"
                // directories.

                // Nested search to locate context directory
                Bottle paths;
                ResourceFinderOptions opts2;
                prependResourceType(app, "contexts");
                opts2.searchLocations = (ResourceFinderOptions::SearchLocations)ResourceFinderOptions::Default;
                opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
                findFileBaseInner(config, app, true, allowPathd, paths, opts2, doc, "context");
                appendResourceType(paths, resourceType);
                for (size_t j = 0; j < paths.size(); j++) {
                    std::string str = check(paths.get(j).asString(), "", "", name, isDir, doc, "context");
                    if (!str.empty()) {
                        addString(output, str);
                        if (justTop) {
                            return;
                        }
                    }
                }
            }
        }

        // check YARP_CONFIG_HOME
        if (((locs & ResourceFinderOptions::User) != 0) && ((flavor & ResourceFinderOptions::ConfigLike) != 0)) {
            std::string home = ResourceFinder::getConfigHomeNoCreate();
            if (!home.empty()) {
                appendResourceType(home, resourceType);
                std::string str = check(home, "", "", name, isDir, doc, "YARP_CONFIG_HOME");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        // check YARP_DATA_HOME
        if (((locs & ResourceFinderOptions::User) != 0) && ((flavor & ResourceFinderOptions::DataLike) != 0)) {
            std::string home = ResourceFinder::getDataHomeNoCreate();
            if (!home.empty()) {
                appendResourceType(home, resourceType);
                std::string str = check(home, "", "", name, isDir, doc, "YARP_DATA_HOME");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        // check YARP_CONFIG_DIRS
        if ((locs & ResourceFinderOptions::Sysadmin) != 0) {
            Bottle dirs = ResourceFinder::getConfigDirs();
            appendResourceType(dirs, resourceType);
            for (size_t i = 0; i < dirs.size(); i++) {
                std::string str = check(dirs.get(i).asString(),
                                        "",
                                        "",
                                        name,
                                        isDir,
                                        doc,
                                        "YARP_CONFIG_DIRS");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        // check YARP_DATA_DIRS
        if ((locs & ResourceFinderOptions::Installed) != 0) {
            Bottle dirs = ResourceFinder::getDataDirs();
            appendResourceType(dirs, resourceType);
            for (size_t i = 0; i < dirs.size(); i++) {
                std::string str = check(dirs.get(i).asString(),
                                        "",
                                        "",
                                        name,
                                        isDir,
                                        doc,
                                        "YARP_DATA_DIRS");
                if (!str.empty()) {
                    addString(output, str);
                    if (justTop) {
                        return;
                    }
                }
            }
        }

        if (allowPathd && ((locs & ResourceFinderOptions::Installed) != 0)) {
            // Nested search to locate path.d directories
            Bottle pathds;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ResourceFinderOptions::Installed);
            opts2.resourceType = "config";
            findFileBaseInner(config, "path.d", true, false, pathds, opts2, doc, "path.d");

            for (size_t i = 0; i < pathds.size(); i++) {
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
                for (size_t i = 0; i < sections.size(); i++) {
                    std::string search_name = sections.get(i).asString();
                    Bottle group = pathd.findGroup(search_name);
                    Bottle paths = group.findGroup("path").tail();
                    appendResourceType(paths, resourceType);
                    for (size_t j = 0; j < paths.size(); j++) {
                        std::string str = check(paths.get(j).asString(), "", "", name, isDir, doc, "yarp.d");
                        if (!str.empty()) {
                            addString(output, str);
                            if (justTop) {
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    bool setVerbose(bool verbose)
    {
        this->verbose = verbose;
        return this->verbose;
    }

    bool setQuiet(bool quiet)
    {
        this->quiet = quiet;
        return this->quiet;
    }

    bool exists(const std::string& fname, bool isDir)
    {
        int result = yarp::os::stat(fname.c_str());
        if (result != 0) {
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
        if (dir!=nullptr) {
            ACE_OS::closedir(dir);
            dir = nullptr;
            return true;
        }
        return false;
        */
        return true;
    }


    std::string getContext()
    {
        return apps.get(0).asString();
    }

    Bottle getContexts()
    {
        return apps;
    }

    std::string getHomeContextPath(Property& config, const std::string& context)
    {
        YARP_UNUSED(config);
        if (useNearMain) {
            return configFilePath;
        }
        std::string path = getPath(ResourceFinder::getDataHome(), "contexts", context, "");

        std::string slash{fs::preferred_separator};
        if (path.length() > 1) {
            if (path[path.length() - 1] == slash[0]) {
                path = path.substr(0, path.length() - slash.size());
            }
        }

        std::string parentPath = getPath(ResourceFinder::getDataHome(), "contexts", "", "");
        if (yarp::os::stat(parentPath.c_str()) != 0) {
            yarp::os::mkdir(parentPath.c_str());
        }

        if (yarp::os::mkdir(path.c_str()) < 0 && errno != EEXIST) {
            fprintf(RTARGET, "||| WARNING  Could not create %s directory\n", path.c_str());
        }
        return path;
    }

    std::string getHomeRobotPath()
    {
        if (useNearMain) {
            return configFilePath;
        }
        bool found = false;
        std::string robot = NetworkBase::getEnvironment("YARP_ROBOT_NAME", &found);
        if (!found) {
            robot = "default";
        }
        std::string path = getPath(ResourceFinder::getDataHome(), "robots", robot, "");

        std::string slash{fs::preferred_separator};
        if (path.length() > 1) {
            if (path[path.length() - 1] == slash[0]) {
                path = path.substr(0, path.length() - slash.size());
            }
        }

        std::string parentPath = getPath(ResourceFinder::getDataHome(), "robots", "", "");
        if (yarp::os::stat(parentPath.c_str()) != 0) {
            yarp::os::mkdir(parentPath.c_str());
        }

        if (yarp::os::mkdir(path.c_str()) < 0 && errno != EEXIST) {
            fprintf(RTARGET, "||| WARNING  Could not create %s directory\n", path.c_str());
        }
        return path;
    }
};


ResourceFinder::ResourceFinder() :
        Searchable(),
        owned(true),
        nullConfig(false),
        isConfiguredFlag(false),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    mPriv = new Private();
}

ResourceFinder::ResourceFinder(const ResourceFinder& alt) :
        Searchable(alt),
        owned(true),
        nullConfig(false),
        isConfiguredFlag(false),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    mPriv = new Private();
    *this = alt;
}

ResourceFinder::ResourceFinder(Searchable& data, Private* altPriv) :
        Searchable(),
        owned(false),
        nullConfig(data.isNull()),
        isConfiguredFlag(true),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    this->mPriv = altPriv;
    if (!data.isNull()) {
        config.fromString(data.toString());
    }
}

ResourceFinder::~ResourceFinder()
{
    if (owned) {
        delete mPriv;
    }
}

const ResourceFinder& ResourceFinder::operator=(const ResourceFinder& alt)
{
    if (&alt != this) {
        *(mPriv) = *(alt.mPriv);
        owned = true;
        nullConfig = alt.nullConfig;
        isConfiguredFlag = alt.isConfiguredFlag;
        config = alt.config;
    }
    return *this;
}

bool ResourceFinder::configure(int argc, char* argv[], bool skipFirstArgument)
{
    isConfiguredFlag = true;
    return mPriv->configure(config, argc, argv, skipFirstArgument);
}


bool ResourceFinder::addContext(const std::string& appName)
{
    if (appName[0] == '\0') {
        return true;
    }
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| adding context [%s]\n", appName.c_str());
    }
    return mPriv->addAppName(appName);
}

bool ResourceFinder::clearContext()
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| clearing context\n");
    }
    return mPriv->clearAppNames();
}

bool ResourceFinder::setDefault(const std::string& key, const std::string& val)
{
    Value val2;
    val2.fromString(val.c_str());
    return mPriv->setDefault(config, key, val2);
}

bool ResourceFinder::setDefault(const std::string& key, std::int32_t val)
{
    return mPriv->setDefault(config, key, Value(val));
}

bool ResourceFinder::setDefault(const std::string& key, yarp::conf::float64_t val)
{
    return mPriv->setDefault(config, key, Value(val));
}

bool ResourceFinder::setDefault(const std::string& key, const yarp::os::Value& val)
{
    return mPriv->setDefault(config, key, val);
}

std::string ResourceFinder::findFile(const std::string& name)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding file [%s]\n", name.c_str());
    }
    return mPriv->findFile(config, name, nullptr);
}

std::string ResourceFinder::findFile(const std::string& name,
                                     const ResourceFinderOptions& options)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding file [%s]\n", name.c_str());
    }
    return mPriv->findFile(config, name, &options);
}

std::string ResourceFinder::findFileByName(const std::string& name)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding file %s\n", name.c_str());
    }
    return mPriv->findFileByName(config, name, nullptr);
}

std::string ResourceFinder::findFileByName(const std::string& name,
                                           const ResourceFinderOptions& options)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding file %s\n", name.c_str());
    }
    return mPriv->findFileByName(config, name, &options);
}


std::string ResourceFinder::findPath(const std::string& name)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding path [%s]\n", name.c_str());
    }
    return mPriv->findPath(config, name, nullptr);
}

std::string ResourceFinder::findPath(const std::string& name,
                                     const ResourceFinderOptions& options)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding path [%s]\n", name.c_str());
    }
    return mPriv->findPath(config, name, &options);
}

yarp::os::Bottle ResourceFinder::findPaths(const std::string& name)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding paths [%s]\n", name.c_str());
    }
    return mPriv->findPaths(config, name, nullptr);
}

yarp::os::Bottle ResourceFinder::findPaths(const std::string& name,
                                           const ResourceFinderOptions& options)
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding paths [%s]\n", name.c_str());
    }
    return mPriv->findPaths(config, name, &options);
}

std::string ResourceFinder::findPath()
{
    if (mPriv->isVerbose()) {
        fprintf(RTARGET, "||| finding path\n");
    }
    return mPriv->findPath(config);
}


bool ResourceFinder::setVerbose(bool verbose)
{
    return mPriv->setVerbose(verbose);
}

bool ResourceFinder::setQuiet(bool quiet)
{
    return mPriv->setQuiet(quiet);
}


bool ResourceFinder::check(const std::string& key) const
{
    return config.check(key);
}


Value& ResourceFinder::find(const std::string& key) const
{
    return config.find(key);
}


Bottle& ResourceFinder::findGroup(const std::string& key) const
{
    return config.findGroup(key);
}


bool ResourceFinder::isNull() const
{
    return nullConfig || config.isNull();
}


std::string ResourceFinder::toString() const
{
    return config.toString();
}

std::string ResourceFinder::getContext()
{
    return mPriv->getContext();
}

std::string ResourceFinder::getHomeContextPath()
{
    return mPriv->getHomeContextPath(config, mPriv->getContext());
}

std::string ResourceFinder::getHomeRobotPath()
{
    return mPriv->getHomeRobotPath();
}

Bottle ResourceFinder::getContexts()
{
    return mPriv->getContexts();
}


ResourceFinder ResourceFinder::findNestedResourceFinder(const std::string& key)
{
    return ResourceFinder(findGroup(key), mPriv);
}


ResourceFinder& ResourceFinder::getResourceFinderSingleton()
{
    static ResourceFinder instance;
    return instance;
}

std::string ResourceFinder::getDataHomeWithPossibleCreation(bool mayCreate)
{
    std::string slash{fs::preferred_separator};
    bool found = false;
    std::string yarp_version = NetworkBase::getEnvironment("YARP_DATA_HOME",
                                                           &found);
    if (!yarp_version.empty()) {
        return yarp_version;
    }
    std::string xdg_version = NetworkBase::getEnvironment("XDG_DATA_HOME",
                                                          &found);
    if (found) {
        return createIfAbsent(mayCreate, xdg_version + slash + "yarp");
    }
#if defined(_WIN32)
    std::string app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return createIfAbsent(mayCreate, app_version + slash + "yarp");
    }
#endif
    std::string home_version = NetworkBase::getEnvironment("HOME");
#if defined(__APPLE__)
    if (home_version != "") {
        return createIfAbsent(mayCreate,
                              home_version
                                  + slash + "Library"
                                  + slash + "Application Support"
                                  + slash + "yarp");
    }
#endif
    if (!home_version.empty()) {
        return createIfAbsent(mayCreate,
                              home_version
                                  + slash + ".local"
                                  + slash + "share"
                                  + slash + "yarp");
    }
    return {};
}


std::string ResourceFinder::getConfigHomeWithPossibleCreation(bool mayCreate)
{
    std::string slash{fs::preferred_separator};
    bool found = false;
    std::string yarp_version = NetworkBase::getEnvironment("YARP_CONFIG_HOME",
                                                           &found);
    if (found) {
        return yarp_version;
    }
    std::string xdg_version = NetworkBase::getEnvironment("XDG_CONFIG_HOME",
                                                          &found);
    if (found) {
        return createIfAbsent(mayCreate, xdg_version + slash + "yarp");
    }
#if defined(_WIN32)
    std::string app_version = NetworkBase::getEnvironment("APPDATA");
    if (app_version != "") {
        return createIfAbsent(mayCreate,
                              app_version + slash + "yarp" + slash + "config");
    }
#endif

#if defined(__APPLE__)
    std::string home_mac_version = getDataHomeNoCreate();
    if (home_mac_version != "") {
        return createIfAbsent(mayCreate,
                              home_mac_version
                                  + slash + "config");
    }
#endif
    std::string home_version = NetworkBase::getEnvironment("HOME");
    if (!home_version.empty()) {
        return createIfAbsent(mayCreate,
                              home_version
                                  + slash + ".config"
                                  + slash + "yarp");
    }
    return {};
}

std::string ResourceFinder::createIfAbsent(bool mayCreate,
                                           const std::string& path)
{
    if (!mayCreate) {
        return path;
    }
    yarp::os::mkdir_p(path.c_str(), 0);
    return path;
}

Bottle ResourceFinder::getDataDirs()
{
    std::string slash{fs::preferred_separator};
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_DATA_DIRS",
                                                                 &found));
    if (found) {
        return yarp_version;
    }
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_DATA_DIRS",
                                                                &found));
    if (found) {
        appendResourceType(xdg_version, "yarp");
        return xdg_version;
    }
#if defined(_WIN32)
    std::string app_version = NetworkBase::getEnvironment("YARP_DIR");
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
    bool found = false;
    Bottle yarp_version = parsePaths(NetworkBase::getEnvironment("YARP_CONFIG_DIRS",
                                                                 &found));
    if (found) {
        return yarp_version;
    }
    Bottle xdg_version = parsePaths(NetworkBase::getEnvironment("XDG_CONFIG_DIRS",
                                                                &found));
    if (found) {
        appendResourceType(xdg_version, "yarp");
        return xdg_version;
    }
#if defined(_WIN32)
    std::string app_version = NetworkBase::getEnvironment("ALLUSERSPROFILE");
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
                                const std::string& key,
                                const ResourceFinderOptions& options)
{
    Bottle bot = mPriv->findPaths(config, key, &options, false);

    for (int i = bot.size() - 1; i >= 0; i--) {
        std::string fname = bot.get(i).asString();
        config.fromConfigFile(fname, false);
    }

    return bot.size() >= 1;
}
