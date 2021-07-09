/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Property.h>

#include <yarp/conf/environment.h>
#include <yarp/conf/string.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/NetType.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PlatformDirent.h>
#include <yarp/os/ResourceFinder.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(PROPERTY, "yarp.os.Property" )
}

class PropertyItem
{
public:
    Bottle bot;
    std::unique_ptr<Property> backing;

    PropertyItem() = default;

    PropertyItem(const PropertyItem& rhs) :
        bot(rhs.bot),
        backing(nullptr)
    {
        if (rhs.backing) {
            backing = std::make_unique<Property>(*(rhs.backing));
        }
    }

    PropertyItem& operator=(const PropertyItem& rhs)
    {
        if (&rhs != this) {
            bot = rhs.bot;
            if (rhs.backing) {
                backing = std::make_unique<Property>(*(rhs.backing));
            }
        }
        return *this;
    }

    PropertyItem(PropertyItem&& rhs) noexcept = default;
    PropertyItem& operator=(PropertyItem&& rhs) noexcept = default;

    ~PropertyItem() = default;

    void clear()
    {
        backing.reset();
    }

    /*
     * The const version of the processBuffered() method performs a const_cast,
     * and calls the non-const version. This allows to call it in const methods.
     * Conceptually this is not completely wrong because it does not modify
     * the external state of the class, but just some internal representation.
     */
    void flush() const
    {
        const_cast<PropertyItem*>(this)->flush();
    }

    void flush()
    {
        if (backing) {
            Bottle flatten(backing->toString());
            bot.append(flatten);
            clear();
        }
    }

    std::string toString() const
    {
        flush();
        return bot.toString();
    }
};

class Property::Private
{
public:
    std::map<std::string, PropertyItem> data;
    Property* owner;

    explicit Private(Property* owner) :
            owner(owner)
    {
    }

    PropertyItem* getPropNoCreate(const std::string& key) const
    {
        auto it = data.find(key);
        if (it == data.end()) {
            return nullptr;
        }
        return const_cast<PropertyItem*>(&(it->second));
    }

    PropertyItem* getProp(const std::string& key, bool create = true)
    {
        auto entry = data.find(key);
        if (entry == data.end()) {
            if (!create) {
                return nullptr;
            }
            data[key] = PropertyItem();
            entry = data.find(key);
        }
        yCAssert(PROPERTY, entry != data.end());
        return &(entry->second);
    }

    void put(const std::string& key, const std::string& val)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.addString(val);
    }

    void put(const std::string& key, const Value& bit)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    void put(const std::string& key, Value* bit)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    Property& addGroup(const std::string& key)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->backing = std::make_unique<Property>();
        return *(p->backing);
    }

    void unput(const std::string& key)
    {
        data.erase(key);
    }

    bool check(const std::string& key) const
    {
        PropertyItem* p = getPropNoCreate(key);
        if (owner->getMonitor() != nullptr) {
            SearchReport report;
            report.key = key;
            report.isFound = (p != nullptr);
            owner->reportToMonitor(report);
        }
        return p != nullptr;
    }

    Value& get(const std::string& key) const
    {
        PropertyItem* p = getPropNoCreate(key);
        if (p != nullptr) {
            p->flush();
            if (owner->getMonitor() != nullptr) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                report.value = p->bot.get(1).toString();
                owner->reportToMonitor(report);
            }
            return p->bot.get(1);
        }
        if (owner->getMonitor() != nullptr) {
            SearchReport report;
            report.key = key;
            owner->reportToMonitor(report);
        }
        return Value::getNullValue();
    }

    Bottle& putBottleCompat(const char* key, const Bottle& val)
    {
        if (val.get(1).asString() == "=") {
            Bottle b;
            b.add(val.get(0));
            b.append(val.tail().tail());
            return putBottle(key, b);
        }
        return putBottle(key, val);
    }

    Bottle& putBottle(const char* key, const Bottle& val)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot = val;
        return p->bot;
    }


    Bottle& putBottle(const char* key)
    {
        PropertyItem* p = getProp(key, true);
        p->clear();
        p->bot.clear();
        return p->bot;
    }


    Bottle* getBottle(const std::string& key) const
    {
        PropertyItem* p = getPropNoCreate(key);
        if (p != nullptr) {
            p->flush();
            return &(p->bot);
        }
        return nullptr;
    }

    void clear()
    {
        data.clear();
    }

    void fromString(const std::string& txt, bool wipe = true)
    {
        Bottle bot;
        bot.fromString(txt);
        fromBottle(bot, wipe);
    }

    void fromCommand(int argc, char* argv[], bool wipe = true)
    {
        std::string tag;
        Bottle accum;
        Bottle total;
        bool qualified = false;
        for (int i = 0; i < argc; i++) {
            std::string work = argv[i];
            bool isTag = false;
            if (work.length() >= 2) {
                if (work[0] == '-' && work[1] == '-') {
                    work = work.substr(2, work.length() - 2);
                    isTag = true;
                    if (work.find("::") != std::string::npos) {
                        qualified = true;
                    }
                }
            }
            if (isTag) {
                if (!tag.empty()) {
                    total.addList().copy(accum);
                }
                tag = work;
                accum.clear();
            } else {
                if (work.find('\\') != std::string::npos) {
                    // Specifically when reading from the command
                    // line, we will allow windows-style paths.
                    // Hence we have to break the "\" character
                    std::string buf;
                    for (char i : work) {
                        buf += i;
                        if (i == '\\') {
                            buf += i;
                        }
                    }
                    work = buf;
                }
            }
            accum.add(Value::makeValue(work));
        }
        if (!tag.empty()) {
            total.addList().copy(accum);
        }
        if (!qualified) {
            fromBottle(total, wipe);
            return;
        }
        if (wipe) {
            clear();
        }
        Bottle* cursor = nullptr;
        for (size_t i = 0; i < total.size(); i++) {
            cursor = nullptr;
            Bottle* term = total.get(i).asList();
            if (term == nullptr) {
                continue;
            }
            std::string key = term->get(0).asString();
            std::string base = key;
            while (key.length() > 0) {
                base = key;
                size_t at = key.find("::");
                if (at != std::string::npos) {
                    base = key.substr(0, at);
                    key = key.substr(at + 2);
                } else {
                    key = "";
                }
                Bottle& result = (cursor != nullptr) ? (cursor->findGroup(base)) : owner->findGroup(base);
                if (result.isNull()) {
                    if (cursor == nullptr) {
                        cursor = &putBottle((base).c_str());
                    } else {
                        cursor = &cursor->addList();
                    }
                    cursor->addString(base);
                } else {
                    cursor = &result;
                }
            }
            if (cursor != nullptr) {
                cursor->copy(*term);
                cursor->get(0) = Value(base);
            }
        }
    }

    bool readDir(const std::string& dirname, yarp::os::impl::DIR*& dir, std::string& result, const std::string& section = std::string())
    {
        bool ok = true;
        yCDebug(PROPERTY, "reading directory %s", dirname.c_str());

        yarp::os::impl::dirent** namelist;
        yarp::os::impl::closedir(dir);
        dir = nullptr;
        int n = yarp::os::impl::scandir(dirname.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
        if (n < 0) {
            return false;
        }
        for (int i = 0; i < n; i++) {
            std::string name = namelist[i]->d_name;
            free(namelist[i]);
            auto len = static_cast<int>(name.length());
            if (len < 4) {
                continue;
            }
            if (name.substr(len - 4) != ".ini") {
                continue;
            }
            std::string fname = std::string(dirname) + "/" + name;
            std::replace(fname.begin(), fname.end(), '\\', '/');
            if (section.empty()) {
                ok = ok && readFile(fname, result, false);
                result += "\n[]\n"; // reset any nested sections
            } else {
                result.append("[include ").append(section).append(" \"").append(fname).append("\" \"").append(fname).append("\"]\n");
            }
        }
        free(namelist);
        return ok;
    }

    bool readFile(const std::string& fname, std::string& result, bool allowDir)
    {
        if (allowDir) {
            yarp::os::impl::DIR* dir = yarp::os::impl::opendir(fname.c_str());
            if (dir != nullptr) {
                return readDir(fname, dir, result);
            }
        }
        yCDebug(PROPERTY, "reading file %s", fname.c_str());
        FILE* fin = fopen(fname.c_str(), "r");
        if (fin == nullptr) {
            return false;
        }
        char buf[25600];
        while (fgets(buf, sizeof(buf) - 1, fin) != nullptr) {
            result += buf;
        }
        fclose(fin);
        fin = nullptr;
        return true;
    }

    bool fromConfigFile(const std::string& fname, Searchable& env, bool wipe = true)
    {
        std::string searchPath = env.check("CONFIG_PATH",
                                           Value(""),
                                           "path to search for config files")
                                     .toString();

        yCDebug(PROPERTY, "looking for %s, search path: %s", fname.c_str(), searchPath.c_str());

        std::string pathPrefix;
        std::string txt;

        bool ok = true;
        if (!readFile(fname, txt, true)) {
            ok = false;
            for (const auto& s : yarp::conf::string::split(searchPath, ';')) {
                std::string trial = s;
                trial += '/';
                trial += fname;

                yCDebug(PROPERTY, "looking for %s as %s", fname.c_str(), trial.c_str());

                txt = "";
                if (readFile(trial, txt, true)) {
                    ok = true;
                    pathPrefix = s;
                    pathPrefix += '/';
                    break;
                }
            }
        }

        std::string path;
        size_t index = fname.rfind('/');
        if (index == std::string::npos) {
            index = fname.rfind('\\');
        }
        if (index != std::string::npos) {
            path = fname.substr(0, index);
        }

        if (!ok) {
            yCError(PROPERTY, "cannot read from %s", fname.c_str());
            return false;
        }

        Property envExtended;
        envExtended.fromString(env.toString());
        if (!path.empty()) {
            if (searchPath.length() > 0) {
                searchPath += ";";
            }
            searchPath += pathPrefix;
            searchPath += path;
            envExtended.put("CONFIG_PATH", searchPath);
        }

        fromConfig(txt.c_str(), envExtended, wipe);
        return true;
    }

    bool fromConfigDir(const std::string& dirname, const std::string& section, bool wipe = true)
    {
        Property p;
        if (section.empty()) {
            return fromConfigFile(dirname, p, wipe);
        }

        yCDebug(PROPERTY, "looking for %s", dirname.c_str());

        yarp::os::impl::DIR* dir = yarp::os::impl::opendir(dirname.c_str());
        if (dir == nullptr) {
            yCError(PROPERTY, "cannot read from %s", dirname.c_str());
            return false;
        }

        std::string txt;
        if (!readDir(dirname, dir, txt, section)) {
            yCError(PROPERTY, "cannot read from %s", dirname.c_str());
            return false;
        }

        fromConfig(txt.c_str(), p, wipe);
        return true;
    }

    void fromConfig(const char* txt, Searchable& env, bool wipe = true)
    {
        StringInputStream sis;
        sis.add(txt);
        sis.add("\n");
        if (wipe) {
            clear();
        }
        std::string tag;
        Bottle accum;
        bool done = false;
        do {
            bool isTag = false;
            bool including = false;
            std::string buf;
            bool good = true;
            buf = sis.readLine('\n', &good);
            while (good && !BottleImpl::isComplete(buf.c_str())) {
                buf += sis.readLine('\n', &good);
            }
            if (!good) {
                done = true;
            }
            if (!done) {
                including = false;

                if (buf.find("//") != std::string::npos || buf.find('#') != std::string::npos) {
                    bool quoted = false;
                    bool prespace = true;
                    int comment = 0;
                    for (unsigned int i = 0; i < buf.length(); i++) {
                        char ch = buf[i];
                        if (ch == '\"') {
                            quoted = !quoted;
                        }
                        if (!quoted) {
                            if (ch == '/') {
                                comment++;
                                if (comment == 2) {
                                    buf = buf.substr(0, i - 1);
                                    break;
                                }
                            } else {
                                comment = 0;
                                if (ch == '#' && prespace) {
                                    if (i == 0) {
                                        buf = "";
                                    } else {
                                        buf = buf.substr(0, i - 1);
                                    }
                                    break;
                                }
                            }
                            prespace = (ch == ' ' || ch == '\t');
                        } else {
                            comment = 0;
                            prespace = false;
                        }
                    }
                }

                // expand any environment references
                buf = expand(buf.c_str(), env, *owner);

                if (buf.length() > 0 && buf[0] == '[') {
                    size_t stop = buf.find(']');
                    if (stop != std::string::npos) {
                        buf = buf.substr(1, stop - 1);
                        size_t space = buf.find(' ');
                        if (space != std::string::npos) {
                            Bottle bot(buf);
                            // BEGIN Handle include option
                            if (bot.size() > 1) {
                                if (bot.get(0).toString() == "import")
                                {
                                    including = true;
                                    // close an open group if an [include something] tag is found
                                    if (!tag.empty()) {
                                        if (accum.size() >= 1) {
                                            putBottleCompat(tag.c_str(), accum);
                                        }
                                        tag = "";
                                    }
                                    std::string contextName = bot.get(1).toString();
                                    std::string fileName = bot.get(2).toString();
                                    yarp::os::ResourceFinder rf;
                                    rf.setDefaultContext(contextName);
                                    rf.setDefaultConfigFile(fileName);
                                    bool b = rf.configure(0,nullptr);
                                    if (b)
                                    {
                                        tag = "";
                                        std::string fname = rf.findFile(fileName);
                                        yCTrace(PROPERTY, "Importing: %s\n", fname.c_str());
                                        fromConfigFile(fname, env, false);
                                    }
                                    else
                                    {
                                        yCWarning(PROPERTY, "Unable to import file: %s from context %s\n", fileName.c_str(), contextName.c_str());
                                    }
                                } else
                                if (bot.get(0).toString() == "include") {
                                    including = true;
                                    // close an open group if an [include something] tag is found
                                    if (!tag.empty()) {
                                        if (accum.size() >= 1) {
                                            putBottleCompat(tag.c_str(), accum);
                                        }
                                        tag = "";
                                    }
                                    if (bot.size() > 2) {
                                        std::string subName;
                                        std::string fname;
                                        if (bot.size() == 3) {
                                            // [include section "filename"]
                                            subName = bot.get(1).toString();
                                            fname = bot.get(2).toString();


                                        } else if (bot.size() == 4) {
                                            // [include type section "filename"]
                                            std::string key;
                                            key = bot.get(1).toString();
                                            subName = bot.get(2).toString();
                                            fname = bot.get(3).toString();
                                            Bottle* target = getBottle(key);
                                            if (target == nullptr) {
                                                Bottle init;
                                                init.addString(key.c_str());
                                                init.addString(subName.c_str());
                                                putBottleCompat(key.c_str(),
                                                                init);
                                            } else {
                                                target->addString(subName.c_str());
                                            }
                                        } else {
                                            yCError(PROPERTY, "bad include");
                                            return;
                                        }


                                        Property p;
                                        if (getBottle(subName) != nullptr) {
                                            p.fromString(getBottle(subName)->tail().toString());
                                            yCTrace(PROPERTY,
                                                    ">>> prior p %s\n",
                                                    p.toString().c_str());
                                        }
                                        p.fromConfigFile(fname, env, false);
                                        accum.fromString(p.toString());
                                        tag = subName;
                                        yCTrace(PROPERTY, ">>> tag %s accum %s\n",
                                                tag.c_str(),
                                                accum.toString().c_str());
                                        if (!tag.empty()) {
                                            if (accum.size() >= 1) {
                                                Bottle b;
                                                b.addString(tag.c_str());
                                                //Bottle& subList = b.addList();
                                                //subList.copy(accum);
                                                b.append(accum);
                                                putBottleCompat(tag.c_str(),
                                                                b);
                                            }
                                            tag = "";
                                        }
                                    } else {
                                        tag = "";
                                        std::string fname = bot.get(1).toString();
                                        yCTrace(PROPERTY, "Including %s\n", fname.c_str());
                                        fromConfigFile(fname, env, false);
                                    }
                                }
                            }
                            // END handle include option
                            // BEGIN handle group
                            if (bot.size() == 2 && !including) {
                                buf = bot.get(1).toString();
                                std::string key = bot.get(0).toString();
                                Bottle* target = getBottle(key);
                                if (target == nullptr) {
                                    Bottle init;
                                    init.addString(key);
                                    init.addString(buf);
                                    putBottleCompat(key.c_str(), init);
                                } else {
                                    target->addString(buf);
                                }
                            }
                            // END handle group
                        }
                        if (!including) {
                            isTag = true;
                        }
                    }
                }
            }
            if (!isTag && !including) {
                Bottle bot;
                bot.fromString(buf);
                if (bot.size() >= 1) {
                    if (tag.empty()) {
                        putBottleCompat(bot.get(0).toString().c_str(), bot);
                    } else {
                        if (bot.get(1).asString() == "=") {
                            Bottle& b = accum.addList();
                            for (size_t i = 0; i < bot.size(); i++) {
                                if (i != 1) {
                                    b.add(bot.get(i));
                                }
                            }
                        } else {
                            accum.addList().copy(bot);
                        }
                    }
                }
            }
            if (isTag || done) {
                if (!tag.empty()) {
                    if (accum.size() >= 1) {
                        putBottleCompat(tag.c_str(), accum);
                    }
                    tag = "";
                }
                tag = buf;
                accum.clear();
                accum.addString(tag);
                if (!tag.empty()) {
                    if (getBottle(tag) != nullptr) {
                        // merge data
                        accum.append(getBottle(tag)->tail());
                        yCTrace(PROPERTY,
                                "MERGE %s, got %s\n",
                                tag.c_str(),
                                accum.toString().c_str());
                    }
                }
            }
        } while (!done);
    }

    void fromBottle(Bottle& bot, bool wipe = true)
    {
        if (wipe) {
            clear();
        }
        for (size_t i = 0; i < bot.size(); i++) {
            Value& bb = bot.get(i);
            if (bb.isList()) {
                Bottle* sub = bb.asList();
                putBottle(bb.asList()->get(0).toString().c_str(), *sub);
            }
        }
    }

    std::string toString() const
    {
        Bottle bot;
        for (const auto& it : data) {
            const PropertyItem& rec = it.second;
            Bottle& sub = bot.addList();
            rec.flush();
            sub.copy(rec.bot);
        }
        return bot.toString();
    }

    // expand any environment variables found
    std::string expand(const char* txt, Searchable& env, Searchable& env2)
    {
        yCTrace(PROPERTY, "expanding %s\n", txt);
        std::string input = txt;
        if (input.find('$') == std::string::npos) {
            // no variables present for sure
            return txt;
        }
        // check if variables present
        std::string output;
        std::string var;
        bool inVar = false;
        bool varHasParen = false;
        bool quoted = false;
        for (size_t i = 0; i <= input.length(); i++) {
            char ch = 0;
            if (i < input.length()) {
                ch = input[i];
            }
            if (quoted) {
                if (!inVar) {
                    output += '\\';
                    if (ch != 0) {
                        output += ch;
                    }
                } else {
                    if (ch != 0) {
                        var += ch;
                    }
                }
                quoted = false;
                continue;
            }
            if (ch == '\\') {
                quoted = true;
                continue;
            }

            if (inVar) {
                if ((isalnum(ch) != 0) || (ch == '_')) {
                    var += ch;
                    continue;
                }
                if (ch == '(' || ch == '{') {
                    if (var.length() == 0) {
                        // ok, just ignore
                        varHasParen = true;
                        continue;
                    }
                }
                inVar = false;
                yCTrace(PROPERTY, "VARIABLE %s\n", var.c_str());
                std::string add = yarp::conf::environment::get_string(var);
                if (add.empty()) {
                    add = env.find(var).toString();
                }
                if (add.empty()) {
                    add = env2.find(var).toString();
                }
                if (add.empty()) {
                    if (var == "__YARP__") {
                        add = "1";
                    }
                }
                if (add.find('\\') != std::string::npos) {
                    // Specifically when reading from the command
                    // line, we will allow windows-style paths.
                    // Hence we have to break the "\" character
                    std::string buf;
                    for (char c : add) {
                        buf += c;
                        if (c == '\\') {
                            buf += c;
                        }
                    }
                    add = buf;
                }
                output += add;
                var = "";
                if (varHasParen && (ch == '}' || ch == ')')) {
                    continue;
                    // don't need current char
                }
            }

            if (!inVar) {
                if (ch == '$') {
                    inVar = true;
                    varHasParen = false;
                    continue;
                }
                if (ch != 0) {
                    output += ch;
                }
            }
        }
        return output;
    }

    void fromArguments(const char* command, bool wipe = true)
    {
        char** szarg = new char*[128 + 1]; // maximum 128 arguments
        char* szcmd = new char[strlen(command) + 1];
        strcpy(szcmd, command);
        int nargs = 0;
        parseArguments(szcmd, &nargs, szarg, 128);
        szarg[nargs] = nullptr;
        fromCommand(nargs, szarg, wipe);
        // clear allocated memory for arguments
        delete[] szcmd;
        szcmd = nullptr;
        delete[] szarg;
        szarg = nullptr;
    }

    void parseArguments(char* azParam, int* argc, char** argv, int max_arg)
    {
        char* pNext = azParam;
        size_t i;
        int j;
        int quoted = 0;
        size_t len = strlen(azParam);

        // Protect spaces inside quotes, but lose the quotes
        for (i = 0; i < len; i++) {
            if ((quoted == 0) && ('"' == azParam[i])) {
                quoted = 1;
                azParam[i] = ' ';
            } else if (((quoted) != 0) && ('"' == azParam[i])) {
                quoted = 0;
                azParam[i] = ' ';
            } else if (((quoted) != 0) && (' ' == azParam[i])) {
                azParam[i] = '\1';
            }
        }

        // init
        memset(argv, 0x00, sizeof(char*) * max_arg);
        *argc = 1;
        argv[0] = azParam;

        while ((nullptr != pNext) && (*argc < max_arg)) {
            splitArguments(pNext, &(argv[*argc]));
            pNext = argv[*argc];

            if (nullptr != argv[*argc]) {
                *argc += 1;
            }
        }

        for (j = 0; j < *argc; j++) {
            len = strlen(argv[j]);
            for (i = 0; i < len; i++) {
                if ('\1' == argv[j][i]) {
                    argv[j][i] = ' ';
                }
            }
        }
    }

    void splitArguments(char* line, char** args)
    {
        char* pTmp = strchr(line, ' ');
        if (pTmp != nullptr) {
            *pTmp = '\0';
            pTmp++;
            while (*pTmp == ' ') {
                pTmp++;
            }
            if (*pTmp == '\0') {
                pTmp = nullptr;
            }
        }
        *args = pTmp;
    }
};

Property::Property() :
        Searchable(),
        Portable(),
        mPriv(new Private(this))
{
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
Property::Property(int hash_size) :
        Searchable(),
        Portable(),
        mPriv(new Private(this))
{
    YARP_UNUSED(hash_size);
}
#endif

Property::Property(const char* str) :
        Searchable(),
        Portable(),
        mPriv(new Private(this))
{
    fromString(str);
}

Property::Property(const Property& prop) :
        Searchable(static_cast<const Searchable&>(prop)),
        Portable(static_cast<const Portable&>(prop)),
        mPriv(new Private(this))
{
    fromString(prop.toString());
}

Property::Property(Property&& prop) noexcept :
        Searchable(std::move(static_cast<Searchable&>(prop))),
        Portable(std::move(static_cast<Portable&>(prop))),
        mPriv(prop.mPriv)
{
    mPriv->owner = this;

    prop.mPriv = nullptr;
}

Property::Property(std::initializer_list<std::pair<std::string, yarp::os::Value>> values) :
        Searchable(),
        Portable(),
        mPriv(new Private(this))
{
    for (const auto& val : values) {
        put(val.first, val.second);
    }
}

Property::~Property()
{
    delete mPriv;
}

Property& Property::operator=(const Property& rhs)
{
    if (&rhs != this) {
        Searchable::operator=(static_cast<const Searchable&>(rhs));
        Portable::operator=(static_cast<const Portable&>(rhs));
        mPriv->data = rhs.mPriv->data;
        mPriv->owner = this;
    }
    return *this;
}

Property& Property::operator=(Property&& rhs) noexcept
{
    Searchable::operator=(std::move(static_cast<Searchable&>(rhs)));
    Portable::operator=(std::move(static_cast<Portable&>(rhs)));
    std::swap(mPriv, rhs.mPriv);
    mPriv->owner = this;
    rhs.mPriv->owner = &rhs;
    return *this;
}

void Property::put(const std::string& key, const std::string& value)
{
    mPriv->put(key, value);
}

void Property::put(const std::string& key, const Value& value)
{
    mPriv->put(key, value);
}


void Property::put(const std::string& key, Value* value)
{
    mPriv->put(key, value);
}

void Property::put(const std::string& key, int value)
{
    put(key, Value::makeInt32(value));
}

void Property::put(const std::string& key, double value)
{
    put(key, Value::makeFloat64(value));
}

bool Property::check(const std::string& key) const
{
    return mPriv->check(key);
}

void Property::unput(const std::string& key)
{
    mPriv->unput(key);
}

Value& Property::find(const std::string& key) const
{
    return mPriv->get(key);
}


void Property::clear()
{
    mPriv->clear();
}


void Property::fromString(const std::string& txt, bool wipe)
{
    mPriv->fromString(txt, wipe);
}


std::string Property::toString() const
{
    return mPriv->toString();
}

void Property::fromCommand(int argc, char* argv[], bool skipFirst, bool wipe)
{
    if (skipFirst) {
        argc--;
        argv++;
    }
    mPriv->fromCommand(argc, argv, wipe);
}

void Property::fromCommand(int argc, const char* argv[], bool skipFirst, bool wipe)
{
    fromCommand(argc, const_cast<char**>(argv), skipFirst, wipe);
}

void Property::fromArguments(const char* arguments, bool wipe)
{
    mPriv->fromArguments(arguments, wipe);
}

bool Property::fromConfigDir(const std::string& dirname, const std::string& section, bool wipe)
{
    return mPriv->fromConfigDir(dirname, section, wipe);
}

bool Property::fromConfigFile(const std::string& fname, bool wipe)
{
    Property p;
    return fromConfigFile(fname, p, wipe);
}


bool Property::fromConfigFile(const std::string& fname, Searchable& env, bool wipe)
{
    return mPriv->fromConfigFile(fname, env, wipe);
}

void Property::fromConfig(const char* txt, bool wipe)
{
    Property p;
    fromConfig(txt, p, wipe);
}

void Property::fromConfig(const char* txt, Searchable& env, bool wipe)
{
    mPriv->fromConfig(txt, env, wipe);
}


bool Property::read(ConnectionReader& reader)
{
    // for now just delegate to Bottle
    Bottle b;
    bool ok = b.read(reader);
    if (ok) {
        fromString(b.toString());
    }
    return ok;
}


bool Property::write(ConnectionWriter& writer) const
{
    // for now just delegate to Bottle
    Bottle b(toString());
    return b.write(writer);
}


Bottle& Property::findGroup(const std::string& key) const
{
    Bottle* result = mPriv->getBottle(key);
    if (getMonitor() != nullptr) {
        SearchReport report;
        report.key = key;
        report.isGroup = true;
        if (result != nullptr) {
            report.isFound = true;
            report.value = result->toString();
        }
        reportToMonitor(report);
        if (result != nullptr) {
            std::string context = getMonitorContext();
            context += ".";
            context += key;
            result->setMonitor(getMonitor(),
                               context.c_str()); // pass on any monitoring
        }
    }

    if (result != nullptr) {
        return *result;
    }
    return Bottle::getNullBottle();
}


void Property::fromQuery(const char* url, bool wipe)
{
    if (wipe) {
        clear();
    }
    std::string str = url;
    str += "&";
    std::string buf;
    std::string key;
    std::string val;
    int code = 0;
    int coding = 0;

    for (char ch : str) {
        if (ch == '=') {
            key = buf;
            val = "";
            buf = "";
            yCTrace(PROPERTY, "adding key %s\n", key.c_str());
        } else if (ch == '&') {
            yCTrace(PROPERTY, "adding val %s\n", val.c_str());
            val = buf;
            buf = "";
            if (!key.empty() && !val.empty()) {
                put(key, val);
            }
            key = "";
        } else if (ch == '?') {
            buf = "";
        } else {
            if (ch == '+') {
                ch = ' ';
            } else if (ch == '%') {
                coding = 2;
            } else {
                if (coding != 0) {
                    int hex = 0;
                    if (ch >= '0' && ch <= '9') {
                        hex = ch - '0';
                    }
                    if (ch >= 'A' && ch <= 'F') {
                        hex = ch - 'A' + 10;
                    }
                    if (ch >= 'a' && ch <= 'f') {
                        hex = ch - 'a' + 10;
                    }
                    code *= 16;
                    code += hex;
                    coding--;
                    if (coding == 0) {
                        ch = code;
                    }
                }
            }
            if (coding == 0) {
                buf += ch;
            }
        }
    }
}


Property& yarp::os::Property::addGroup(const std::string& key)
{
    return mPriv->addGroup(key);
}
