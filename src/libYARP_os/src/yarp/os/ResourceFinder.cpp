/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ResourceFinder.h>

#include <yarp/conf/dirs.h>
#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/LogComponent.h>
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

namespace {
#ifndef YARP_NO_DEPRECATED // since YARP 3.4
// The log component cannot be const because we still support setVerbose and
// setQuiet
YARP_OS_NON_CONST_LOG_COMPONENT(RESOURCEFINDER, "yarp.os.ResourceFinder")
#else
YARP_OS_LOG_COMPONENT(RESOURCEFINDER, "yarp.os.ResourceFinder")
#endif

std::vector<std::string> bottle_to_vector(const Bottle& b)
{
    std::vector<std::string> out;
    for (size_t i = 0; i < b.size(); ++i) {
        out.push_back(b.get(i).asString());
    }
    return out;
}

Bottle vector_to_bottle(const std::vector<std::string>& v)
{
    Bottle out;
    for (const auto& s : v) {
        out.addString(s);
    }
    return out;
}

} // namespace

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

static void appendResourceType(std::vector<std::string>& paths,
                               const std::string& resourceType)
{
    for (auto& txt : paths) {
        appendResourceType(txt, resourceType);
    }
}


//---------------------------------------------------------------------------------------------------
class ResourceFinder::Private
{
private:
    yarp::os::Bottle apps;
    std::string configFilePath;
    yarp::os::Property cache;
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

    bool configureProp(Property& config, int argc, char* argv[], bool skip)
    {
        Property p;
        p.fromCommand(argc, argv, skip);

        bool user_specified_from = p.check("from");

        if (p.check("verbose")) {
            yCWarning(RESOURCEFINDER, "The 'verbose' option is deprecated.");
        }

        yCDebug(RESOURCEFINDER, "configuring");

        bool configured_normally = true;

        if (p.check("context")) {
            clearAppNames();
            std::string c = p.check("context", Value("default")).asString();
            addAppName(c.c_str());
            yCDebug(RESOURCEFINDER, "added context %s", c.c_str());
        }

        config.fromCommand(argc, argv, skip, false);
        if (config.check("from")) {
            std::string from = config.check("from", Value("config.ini")).toString();
            yCDebug(RESOURCEFINDER, "default config file specified as %s", from.c_str());
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

        std::string base = doc.toString();
        yCDebug(RESOURCEFINDER, "checking [%s] (%s%s%s)", s.c_str(), base.c_str(), (base.length() == 0) ? "" : " ", doc2.c_str());

        bool ok = exists(s.c_str(), isDir);
        Value status;
        yCAssert(RESOURCEFINDER, status.asList());
        status.asList()->addFloat64(SystemClock::nowSystem());
        status.asList()->addInt32(ok ? 1 : 0);
        cache.put(s, status);
        if (ok) {
            yCDebug(RESOURCEFINDER, "found %s", s.c_str());
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

    std::vector<std::string> findPaths(Property& config, const std::string& name, const ResourceFinderOptions* externalOptions, bool enforcePlural = true)
    {
        std::string fname = config.check(name, Value(name)).asString();
        std::vector<std::string> paths;
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
        std::vector<std::string> output;
        ResourceFinderOptions opts;
        if (externalOptions == nullptr) {
            externalOptions = &opts;
        }
        findFileBase(config, name, isDir, output, *externalOptions);
        return output.empty() ? std::string{} : output.at(0);
    }

    bool canShowErrors(const ResourceFinderOptions& opts) const
    {
        if (opts.messageFilter == ResourceFinderOptions::ShowNone) {
            return false;
        }
        return true;
    }

    void findFileBase(Property& config, const std::string& name, bool isDir, std::vector<std::string>& output, const ResourceFinderOptions& opts)
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
                yCDebug(RESOURCEFINDER, "did not find %s", name.c_str());
            }
        }
    }

    static void addString(std::vector<std::string>& output, const std::string& txt)
    {
        for (const auto& str : output) {
            if (txt == str) {
                return;
            }
        }
        output.push_back(txt);
    }

    void findFileBaseInner(Property& config, const std::string& name, bool isDir, bool allowPathd, std::vector<std::string>& output, const ResourceFinderOptions& opts, const Bottle& predoc, const std::string& reason)
    {
        Bottle doc;
        doc = predoc;
        if (!reason.empty()) {
            doc.addString(reason);
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
            std::string robot = yarp::conf::environment::get_string("YARP_ROBOT_NAME", &found);
            if (!found) {
                robot = "default";
            }

            // Nested search to locate robot directory
            std::vector<std::string> paths;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(ResourceFinderOptions::User | ResourceFinderOptions::Sysadmin | ResourceFinderOptions::Installed);
            opts2.resourceType = "robots";
            opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
            findFileBaseInner(config, robot, true, allowPathd, paths, opts2, doc, "robot");
            appendResourceType(paths, resourceType);
            for (const auto& path : paths) {
                std::string str = check(path,
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
                std::vector<std::string> paths;
                ResourceFinderOptions opts2;
                prependResourceType(app, "contexts");
                opts2.searchLocations = (ResourceFinderOptions::SearchLocations)ResourceFinderOptions::Default;
                opts2.duplicateFilesPolicy = ResourceFinderOptions::All;
                findFileBaseInner(config, app, true, allowPathd, paths, opts2, doc, "context");
                appendResourceType(paths, resourceType);
                for (const auto& path : paths) {
                    std::string str = check(path, "", "", name, isDir, doc, "context");
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
            std::string home = yarp::conf::dirs::yarpconfighome();
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
            std::string home = yarp::conf::dirs::yarpdatahome();
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
            auto dirs = yarp::conf::dirs::yarpconfigdirs();
            appendResourceType(dirs, resourceType);
            for (const auto& dir : dirs) {
                std::string str = check(dir,
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
            auto dirs = yarp::conf::dirs::yarpdatadirs();
            appendResourceType(dirs, resourceType);
            for (const auto& dir : dirs) {
                std::string str = check(dir,
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
            std::vector<std::string> pathd_paths;
            ResourceFinderOptions opts2;
            opts2.searchLocations = (ResourceFinderOptions::SearchLocations)(opts.searchLocations & ResourceFinderOptions::Installed);
            opts2.resourceType = "config";
            findFileBaseInner(config, "path.d", true, false, pathd_paths, opts2, doc, "path.d");

            for (const auto& pathd_path : pathd_paths) {
                // check /.../path.d/*
                // this directory is expected to contain *.ini files like this:
                //   [search BUNDLE_NAME]
                //   path /PATH1 /PATH2
                // for example:
                //   [search icub]
                //   path /usr/share/iCub
                Property pathd;
                pathd.fromConfigFile(pathd_path);
                Bottle sections = pathd.findGroup("search").tail();
                for (size_t i = 0; i < sections.size(); i++) {
                    std::string search_name = sections.get(i).asString();
                    Bottle group = pathd.findGroup(search_name);
                    auto paths_v = bottle_to_vector(group.findGroup("path").tail());
                    appendResourceType(paths_v, resourceType);
                    for (const auto& path : paths_v) {
                        std::string str = check(path, "", "", name, isDir, doc, "yarp.d");
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
        std::string path = getPath(yarp::conf::dirs::yarpdatahome(), "contexts", context, "");

        std::string slash{fs::preferred_separator};
        if (path.length() > 1) {
            if (path[path.length() - 1] == slash[0]) {
                path = path.substr(0, path.length() - slash.size());
            }
        }

        std::string parentPath = getPath(yarp::conf::dirs::yarpdatahome(), "contexts", "", "");
        if (yarp::os::stat(parentPath.c_str()) != 0) {
            yarp::os::mkdir_p(parentPath.c_str());
        }

        if (yarp::os::mkdir_p(path.c_str()) < 0 && errno != EEXIST) {
            yCWarning(RESOURCEFINDER, "Could not create %s directory", path.c_str());
        }
        return path;
    }

    std::string getHomeRobotPath()
    {
        if (useNearMain) {
            return configFilePath;
        }
        bool found = false;
        std::string robot = yarp::conf::environment::get_string("YARP_ROBOT_NAME", &found);
        if (!found) {
            robot = "default";
        }
        std::string path = getPath(yarp::conf::dirs::yarpdatahome(), "robots", robot, "");

        std::string slash{fs::preferred_separator};
        if (path.length() > 1) {
            if (path[path.length() - 1] == slash[0]) {
                path = path.substr(0, path.length() - slash.size());
            }
        }

        std::string parentPath = getPath(yarp::conf::dirs::yarpdatahome(), "robots", "", "");
        if (yarp::os::stat(parentPath.c_str()) != 0) {
            yarp::os::mkdir_p(parentPath.c_str());
        }

        if (yarp::os::mkdir_p(path.c_str()) < 0 && errno != EEXIST) {
            yCWarning(RESOURCEFINDER, "Could not create %s directory", path.c_str());
        }
        return path;
    }
};


//---------------------------------------------------------------------------------------------------
ResourceFinder::ResourceFinder() :
        Searchable(),
        m_owned(true),
        m_nullConfig(false),
        m_isConfiguredFlag(false),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    mPriv = new Private();
}

ResourceFinder::ResourceFinder(const ResourceFinder& alt) :
        Searchable(alt),
        m_owned(true),
        m_nullConfig(false),
        m_isConfiguredFlag(false),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    mPriv = new Private();
    *this = alt;
}

ResourceFinder::ResourceFinder(Searchable& data, Private* altPriv) :
        Searchable(),
        m_owned(false),
        m_nullConfig(data.isNull()),
        m_isConfiguredFlag(true),
        mPriv(nullptr)
{
    NetworkBase::autoInitMinimum(yarp::os::YARP_CLOCK_SYSTEM);
    this->mPriv = altPriv;
    if (!data.isNull()) {
        m_configprop.fromString(data.toString());
    }
}

ResourceFinder::~ResourceFinder()
{
    if (m_owned) {
        delete mPriv;
    }
}

const ResourceFinder& ResourceFinder::operator=(const ResourceFinder& alt)
{
    if (&alt != this) {
        *(mPriv) = *(alt.mPriv);
        m_owned = true;
        m_nullConfig = alt.m_nullConfig;
        m_isConfiguredFlag = alt.m_isConfiguredFlag;
        m_configprop = alt.m_configprop;
    }
    return *this;
}

bool ResourceFinder::configure(int argc, char* argv[], bool skipFirstArgument)
{
    m_isConfiguredFlag = true;
    return mPriv->configureProp(m_configprop, argc, argv, skipFirstArgument);
}


bool ResourceFinder::addContext(const std::string& appName)
{
    if (appName[0] == '\0') {
        return true;
    }
    yCDebug(RESOURCEFINDER, "adding context [%s]", appName.c_str());
    return mPriv->addAppName(appName);
}

bool ResourceFinder::clearContext()
{
    yCDebug(RESOURCEFINDER, "clearing context");
    return mPriv->clearAppNames();
}

bool ResourceFinder::setDefault(const std::string& key, const std::string& val)
{
    Value val2;
    val2.fromString(val.c_str());
    return mPriv->setDefault(m_configprop, key, val2);
}

bool ResourceFinder::setDefault(const std::string& key, std::int32_t val)
{
    return mPriv->setDefault(m_configprop, key, Value(val));
}

bool ResourceFinder::setDefault(const std::string& key, yarp::conf::float64_t val)
{
    return mPriv->setDefault(m_configprop, key, Value(val));
}

bool ResourceFinder::setDefault(const std::string& key, const yarp::os::Value& val)
{
    return mPriv->setDefault(m_configprop, key, val);
}

std::string ResourceFinder::findFile(const std::string& name)
{
    yCDebug(RESOURCEFINDER, "finding file [%s]", name.c_str());
    return mPriv->findFile(m_configprop, name, nullptr);
}

std::string ResourceFinder::findFile(const std::string& name,
                                     const ResourceFinderOptions& options)
{
    yCDebug(RESOURCEFINDER, "finding file [%s]", name.c_str());
    return mPriv->findFile(m_configprop, name, &options);
}

std::string ResourceFinder::findFileByName(const std::string& name)
{
    yCDebug(RESOURCEFINDER, "finding file %s", name.c_str());
    return mPriv->findFileByName(m_configprop, name, nullptr);
}

std::string ResourceFinder::findFileByName(const std::string& name,
                                           const ResourceFinderOptions& options)
{
    yCDebug(RESOURCEFINDER, "finding file %s", name.c_str());
    return mPriv->findFileByName(m_configprop, name, &options);
}


std::string ResourceFinder::findPath(const std::string& name)
{
    yCDebug(RESOURCEFINDER, "finding path [%s]", name.c_str());
    return mPriv->findPath(m_configprop, name, nullptr);
}

std::string ResourceFinder::findPath(const std::string& name,
                                     const ResourceFinderOptions& options)
{
    yCDebug(RESOURCEFINDER, "finding path [%s]", name.c_str());
    return mPriv->findPath(m_configprop, name, &options);
}

yarp::os::Bottle ResourceFinder::findPaths(const std::string& name)
{
    yCDebug(RESOURCEFINDER, "finding paths [%s]", name.c_str());
    return vector_to_bottle(mPriv->findPaths(m_configprop, name, nullptr));
}

yarp::os::Bottle ResourceFinder::findPaths(const std::string& name,
                                           const ResourceFinderOptions& options)
{
    yCDebug(RESOURCEFINDER, "finding paths [%s]", name.c_str());
    return vector_to_bottle(mPriv->findPaths(m_configprop, name, &options));
}

std::string ResourceFinder::findPath()
{
    yCDebug(RESOURCEFINDER, "finding path");
    return mPriv->findPath(m_configprop);
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
bool ResourceFinder::setVerbose(bool verbose)
{
    RESOURCEFINDER().setMinimumPrintLevel(verbose ? yarp::os::Log::DebugType : yarp::os::Log::InfoType);
    return true;
}

bool ResourceFinder::setQuiet(bool quiet)
{
    RESOURCEFINDER().setMinimumPrintLevel(quiet ? yarp::os::Log::WarningType : yarp::os::Log::InfoType);
    return true;
}
#endif

bool ResourceFinder::check(const std::string& key) const
{
    return m_configprop.check(key);
}


Value& ResourceFinder::find(const std::string& key) const
{
    return m_configprop.find(key);
}


Bottle& ResourceFinder::findGroup(const std::string& key) const
{
    return m_configprop.findGroup(key);
}


bool ResourceFinder::isNull() const
{
    return m_nullConfig || m_configprop.isNull();
}


std::string ResourceFinder::toString() const
{
    return m_configprop.toString();
}

std::string ResourceFinder::getContext()
{
    return mPriv->getContext();
}

std::string ResourceFinder::getHomeContextPath()
{
    return mPriv->getHomeContextPath(m_configprop, mPriv->getContext());
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

std::string ResourceFinder::createIfAbsent(bool mayCreate,
                                           const std::string& path)
{
    if (!mayCreate) {
        return path;
    }
    yarp::os::mkdir_p(path.c_str(), 0);
    return path;
}

bool ResourceFinder::readConfig(Property& config,
                                const std::string& key,
                                const ResourceFinderOptions& options)
{
    std::vector<std::string> paths = mPriv->findPaths(config, key, &options, false);

    for (auto it = paths.rbegin(); it != paths.rend(); ++it) {
        const auto& path = *it;
        config.fromConfigFile(path, false);
    }

    return !paths.empty();
}


#ifndef YARP_NO_DEPRECATED // Since YARP 3.5

std::string ResourceFinder::getDataHomeWithPossibleCreation(bool mayCreate)
{
    return createIfAbsent(mayCreate, yarp::conf::dirs::yarpdatahome());
}


std::string ResourceFinder::getConfigHomeWithPossibleCreation(bool mayCreate)
{
    return createIfAbsent(mayCreate, yarp::conf::dirs::yarpconfighome());
}


Bottle ResourceFinder::getDataDirs()
{
    Bottle result;
    for (const auto& dir : yarp::conf::dirs::yarpdatadirs()) {
        result.addString(dir);
    }
    return result;
}


Bottle ResourceFinder::getConfigDirs()
{
    Bottle result;
    for (const auto& dir : yarp::conf::dirs::yarpconfigdirs()) {
        result.addString(dir);
    }
    return result;
}

#endif // YARP_NO_DEPRECATED
