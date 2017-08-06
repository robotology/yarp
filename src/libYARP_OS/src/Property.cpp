/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Property.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/SplitString.h>

#include <yarp/os/impl/PlatformDirent.h>

#include <algorithm>
#include <map>
#include <cctype>
#include <cstdio>
#include <cstring>

using namespace yarp::os::impl;
using namespace yarp::os;

class PropertyItem {
public:
    Bottle bot;
    Property *backing;
    bool singleton;

    PropertyItem() {
        singleton = false;
        backing = YARP_NULLPTR;
    }

    ~PropertyItem() {
        clear();
    }

    void clear() {
        if (backing) {
            delete backing;
            backing = YARP_NULLPTR;
        }
    }

    void flush() {
        if (backing) {
            Bottle flatten(backing->toString());
            bot.append(flatten);
            clear();
        }
    }

    ConstString toString() {
        flush();
        return bot.toString();
    }
};

class PropertyHelper {
public:
    std::map<ConstString, PropertyItem> data;
    Property& owner;

    PropertyHelper(Property& owner, int hash_size) :
        owner(owner) {}

    PropertyItem *getPropNoCreate(const ConstString& key) const {
        auto it = data.find(key);
        if (it==data.end()) {
            return YARP_NULLPTR;
        }
        return const_cast<PropertyItem*>(&(it->second));
    }

    PropertyItem *getProp(const ConstString& key, bool create = true) {
        std::map<ConstString, PropertyItem>::iterator entry = data.find(key);
        if (entry == data.end()) {
            if (!create) {
                return YARP_NULLPTR;
            }
            data[key] = PropertyItem();
            entry = data.find(key);
        }
        yAssert(entry != data.end());
        return &(entry->second);
    }

    void put(const ConstString& key, const ConstString& val) {
        PropertyItem *p = getProp(key, true);
        p->singleton = true;
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.addString(val);
    }

    void put(const ConstString& key, const Value& bit) {
        PropertyItem *p = getProp(key, true);
        p->singleton = true;
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    void put(const ConstString& key, Value *bit) {
        PropertyItem *p = getProp(key, true);
        p->singleton = true;
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    Property& addGroup(const ConstString& key) {
        PropertyItem *p = getProp(key, true);
        p->singleton = true;
        p->clear();
        p->bot.clear();
        p->bot.addString(key);
        p->backing = new Property();
        yAssert(p->backing);
        return *(p->backing);
    }

    bool check(const ConstString& key, Value *&output) const {
        PropertyItem *p = getPropNoCreate(key);

        return p!=YARP_NULLPTR;
    }

    void unput(const ConstString& key) {
        data.erase(key);
    }

    bool check(const ConstString& key) const {
        PropertyItem *p = getPropNoCreate(key);
        if (owner.getMonitor()!=YARP_NULLPTR) {
            SearchReport report;
            report.key = key;
            report.isFound = (p!=YARP_NULLPTR);
            owner.reportToMonitor(report);
        }
        return p!=YARP_NULLPTR;
    }

    Value& get(const ConstString& key) const {
        ConstString out;
        PropertyItem *p = getPropNoCreate(key);
        if (p!=YARP_NULLPTR) {
            p->flush();
            if (owner.getMonitor()!=YARP_NULLPTR) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                report.value = p->bot.get(1).toString();
                owner.reportToMonitor(report);
            }
            return p->bot.get(1);
        }
        if (owner.getMonitor()!=YARP_NULLPTR) {
            SearchReport report;
            report.key = key;
            owner.reportToMonitor(report);
        }
        return Value::getNullValue();
    }

    Bottle& putBottleCompat(const char *key, const Bottle& val) {
        if (val.get(1).asString()=="=") {
            Bottle b;
            b.add(val.get(0));
            b.append(val.tail().tail());
            return putBottle(key, b);
        }
        return putBottle(key, val);
    }

    Bottle& putBottle(const char *key, const Bottle& val) {
        PropertyItem *p = getProp(key, true);
        p->singleton = false;
        p->clear();
        // inefficient! copy not implemented yet...
        p->bot.fromString(val.toString().c_str());
        return p->bot;
    }


    Bottle& putBottle(const char *key) {
        PropertyItem *p = getProp(key, true);
        p->singleton = false;
        p->clear();
        p->bot.clear();
        return p->bot;
    }


    Bottle *getBottle(const ConstString& key) const {
        PropertyItem *p = getPropNoCreate(key);
        if (p!=YARP_NULLPTR) {
            p->flush();
            return &(p->bot);
        }
        return YARP_NULLPTR;
    }

    void clear() {
        data.clear();
    }

    void fromString(const ConstString& txt, bool wipe=true) {
        Bottle bot;
        bot.fromString(txt);
        fromBottle(bot, wipe);
    }

    void fromCommand(int argc, char *argv[], bool wipe=true) {
        ConstString tag = "";
        Bottle accum;
        Bottle total;
        bool qualified = false;
        for (int i=0; i<argc; i++) {
            ConstString work = argv[i];
            bool isTag = false;
            if (work.length()>=2) {
                if (work[0]=='-'&&work[1]=='-') {
                    work = work.substr(2, work.length()-2);
                    isTag = true;
                    if (work.find("::")!=ConstString::npos) {
                        qualified = true;
                    }
                }
            }
            if (isTag) {
                if (tag!="") {
                    total.addList().copy(accum);
                }
                tag = work;
                accum.clear();
            } else {
                if (work.find('\\')!=ConstString::npos) {
                    // Specifically when reading from the command
                    // line, we will allow windows-style paths.
                    // Hence we have to break the "\" character
                    ConstString buf = "";
                    for (unsigned int i=0; i<work.length(); i++) {
                        buf += work[i];
                        if (work[i]=='\\') {
                            buf += work[i];
                        }
                    }
                    work = buf;
                }
            }
            accum.add(Value::makeValue(work.c_str()));
        }
        if (tag!="") {
            total.addList().copy(accum);
        }
        if (!qualified) {
            fromBottle(total, wipe);
            return;
        }
        if (wipe) {
            clear();
        }
        Bottle *cursor = YARP_NULLPTR;
        for (int i=0; i<total.size(); i++) {
            cursor = YARP_NULLPTR;
            Bottle *term = total.get(i).asList();
            if (!term) continue;
            ConstString key = term->get(0).asString();
            ConstString base = key;
            while (key.length()>0) {
                int at = key.find("::");
                base = key;
                if (at>=0) {
                    base = key.substr(0, at);
                    key = key.substr(at+2);
                } else {
                    key = "";
                }
                Bottle& result = (cursor!=YARP_NULLPTR)? (cursor->findGroup(base.c_str())) : owner.findGroup(base.c_str());
                if (result.isNull()) {
                    if (!cursor) {
                        cursor = &putBottle((base).c_str());
                    } else {
                        cursor = &cursor->addList();
                    }
                    cursor->addString(base);
                } else {
                    cursor = &result;
                }
            }
            if (cursor) {
                cursor->copy(*term);
                cursor->get(0) = Value(base);
            }
        }
    }

    bool readDir(const ConstString& dirname, yarp::os::impl::DIR *&dir, ConstString& result, const ConstString& section=ConstString()) {
        bool ok = true;
        YARP_DEBUG(Logger::get(),
                   ConstString("reading directory ") + dirname);

        yarp::os::impl::dirent **namelist;
        yarp::os::impl::closedir(dir);
        dir = YARP_NULLPTR;
        int n = yarp::os::impl::scandir(dirname.c_str(), &namelist, YARP_NULLPTR, yarp::os::impl::alphasort);
        if (n<0) {
            return false;
        }
        for (int i=0; i<n; i++) {
            ConstString name = namelist[i]->d_name;
            free(namelist[i]);
            int len = (int)name.length();
            if (len<4) continue;
            if (name.substr(len-4)!=".ini") continue;
            ConstString fname = ConstString(dirname) + "/" + name;
            std::replace(fname.begin(), fname.end(), '\\', '/');
            if (section.empty()) {
                ok = ok && readFile(fname, result, false);
                result += "\n[]\n";  // reset any nested sections
            } else {
                result += "[include " + section + " \"" + fname + "\" \"" + fname + "\"]\n";
            }
        }
        free(namelist);
        return ok;
    }

    bool readFile(const ConstString& fname, ConstString& result, bool allowDir) {
        if (allowDir) {
            yarp::os::impl::DIR *dir = yarp::os::impl::opendir(fname.c_str());
            if (dir) return readDir(fname, dir, result);
        }
        YARP_DEBUG(Logger::get(),
                   ConstString("reading file ") + fname);
        FILE *fin = fopen(fname.c_str(), "r");
        if (!fin) {
            return false;
        }
        char buf[25600];
        while(fgets(buf, sizeof(buf)-1, fin) != YARP_NULLPTR) {
            result += buf;
        }
        fclose(fin);
        fin = YARP_NULLPTR;
        return true;
    }

    bool fromConfigFile(const ConstString& fname, Searchable& env, bool wipe=true) {
        ConstString searchPath =
            env.check("CONFIG_PATH",
                      Value(""),
                      "path to search for config files").toString().c_str();

        YARP_DEBUG(Logger::get(),
                   ConstString("looking for ") + fname.c_str() + ", search path: " +
                   searchPath);

        ConstString pathPrefix("");
        ConstString txt;

        bool ok = true;
        if (!readFile(fname, txt, true)) {
            ok = false;
            SplitString ss(searchPath.c_str(), ';');
            for (int i=0; i<ss.size(); i++) {
                ConstString trial = ss.get(i);
                trial += '/';
                trial += fname;

                YARP_DEBUG(Logger::get(),
                           ConstString("looking for ") + fname + " as " +
                           trial.c_str());

                txt = "";
                if (readFile(trial.c_str(), txt, true)) {
                    ok = true;
                    pathPrefix = ss.get(i);
                    pathPrefix += '/';
                    break;
                }
            }
        }

        ConstString path("");
        size_t index = fname.rfind('/');
        if (index==ConstString::npos) {
            index = fname.rfind('\\');
        }
        if (index!=ConstString::npos) {
            path = fname.substr(0, index);
        }

        if (!ok) {
            YARP_ERROR(Logger::get(), ConstString("cannot read from ") +
                       fname);
            return false;
        }

        Property envExtended;
        envExtended.fromString(env.toString());
        if (path!="") {
            if (searchPath.length()>0) {
                searchPath += ";";
            }
            searchPath += pathPrefix;
            searchPath += path;
            envExtended.put("CONFIG_PATH", searchPath.c_str());
        }

        fromConfig(txt.c_str(), envExtended, wipe);
        return true;
    }

    bool fromConfigDir(const ConstString& dirname, const ConstString& section, bool wipe=true) {
        Property p;
        if (section.empty()) {
            return fromConfigFile(dirname, p, wipe);
        }

        YARP_DEBUG(Logger::get(), ConstString("looking for ") + dirname.c_str());

        yarp::os::impl::DIR *dir = yarp::os::impl::opendir(dirname.c_str());
        if (!dir) {
            YARP_ERROR(Logger::get(), ConstString("cannot read from ") + dirname);
            return false;
        }

        ConstString txt;
        if (!readDir(dirname, dir, txt, section)) {
            YARP_ERROR(Logger::get(), ConstString("cannot read from ") + dirname);
            return false;
        }

        fromConfig(txt.c_str(), p, wipe);
        return true;
    }

    void fromConfig(const char *txt, Searchable& env, bool wipe=true) {
        StringInputStream sis;
        sis.add(txt);
        sis.add("\n");
        if (wipe) {
            clear();
        }
        ConstString tag = "";
        Bottle accum;
        bool done = false;
        do {
            bool isTag = false;
            bool including = false;
            ConstString buf;
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

                if (buf.find("//")!=ConstString::npos||buf.find('#')!=ConstString::npos) {
                    bool quoted = false;
                    bool prespace = true;
                    int comment = 0;
                    for (unsigned int i=0; i<buf.length(); i++) {
                        char ch = buf[i];
                        if (ch=='\"') { quoted = !quoted; }
                        if (!quoted) {
                            if (ch=='/') {
                                comment++;
                                if (comment==2) {
                                    buf = buf.substr(0, i-1);
                                    break;
                                }
                            } else {
                                comment = 0;
                                if (ch=='#'&&prespace) {
                                    if (i==0) {
                                        buf = "";
                                    } else {
                                        buf = buf.substr(0, i-1);
                                    }
                                    break;
                                }
                            }
                            prespace = (ch==' '||ch=='\t');
                        } else {
                            comment = 0;
                            prespace = false;
                        }
                    }
                }

                // expand any environment references
                buf = expand(buf.c_str(), env, owner).c_str();

                if (buf.length()>0 && buf[0]=='[') {
                    size_t stop = buf.find(']');
                    if (stop!=ConstString::npos) {
                        buf = buf.substr(1, stop-1);
                        size_t space = buf.find(' ');
                        if (space!=ConstString::npos) {
                            Bottle bot(buf.c_str());
                            // BEGIN Handle include option
                            if (bot.size()>1) {
                                if (bot.get(0).toString() == "include") {
                                    including = true;
                                    // close an open group if an [include something] tag is found
                                    if (bot.size()>1) {
                                        if (tag!="") {
                                            if (accum.size()>=1) {
                                                putBottleCompat(tag.c_str(),
                                                                accum);
                                            }
                                            tag = "";
                                        }
                                    }
                                    if (bot.size()>2) {
                                        ConstString subName, fname;
                                        if (bot.size()==3) {
                                            // [include section "filename"]
                                            subName = bot.get(1).toString();
                                            fname = bot.get(2).toString();


                                        } else if (bot.size()==4) {
                                            // [include type section "filename"]
                                            ConstString key;
                                            key = bot.get(1).toString();
                                            subName = bot.get(2).toString();
                                            fname = bot.get(3).toString();
                                            Bottle *target =
                                                getBottle(key.c_str());
                                            if (target==YARP_NULLPTR) {
                                                Bottle init;
                                                init.addString(key.c_str());
                                                init.addString(subName.c_str());
                                                putBottleCompat(key.c_str(),
                                                                init);
                                            } else {
                                                target->addString(subName.c_str());
                                            }
                                        } else {
                                            YARP_ERROR(Logger::get(),
                                                       ConstString("bad include"));
                                            return;
                                        }


                                        Property p;
                                        if (getBottle(subName)!=YARP_NULLPTR) {
                                            p.fromString(getBottle(subName)->tail().toString());
                                            //printf(">>> prior p %s\n",
                                            //     p.toString().c_str());
                                        }
                                        p.fromConfigFile(fname.c_str(),
                                                         env, false);
                                        accum.fromString(p.toString());
                                        tag = subName.c_str();
                                        //printf(">>> tag %s accum %s\n",
                                        //     tag.c_str(),
                                        //     accum.toString().c_str());
                                        if (tag!="") {
                                            if (accum.size()>=1) {
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
                                        ConstString fname =
                                            bot.get(1).toString();
                                        //printf("Including %s\n", fname.c_str());
                                        fromConfigFile(fname.c_str(),
                                                       env, false);
                                    }
                                }
                            }
                            // END handle include option
                            // BEGIN handle group
                            if (bot.size()==2 && !including) {
                                buf = bot.get(1).toString().c_str();
                                ConstString key = bot.get(0).toString().c_str();
                                Bottle *target = getBottle(key.c_str());
                                if (target==YARP_NULLPTR) {
                                    Bottle init;
                                    init.addString(key.c_str());
                                    init.addString(buf.c_str());
                                    putBottleCompat(key.c_str(), init);
                                } else {
                                    target->addString(buf.c_str());
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
                bot.fromString(buf.c_str());
                if (bot.size()>=1) {
                    if (tag=="") {
                        putBottleCompat(bot.get(0).toString().c_str(), bot);
                    } else {
                        if (bot.get(1).asString()=="=") {
                            Bottle& b = accum.addList();
                            for (int i=0; i<bot.size(); i++) {
                                if (i!=1) {
                                    b.add(bot.get(i));
                                }
                            }
                        } else {
                            accum.addList().copy(bot);
                        }
                    }
                }
            }
            if (isTag||done) {
                if (tag!="") {
                    if (accum.size()>=1) {
                        putBottleCompat(tag.c_str(), accum);
                    }
                    tag = "";
                }
                tag = buf;
                accum.clear();
                accum.addString(tag.c_str());
                if (tag!="") {
                    if (getBottle(tag.c_str())!=YARP_NULLPTR) {
                        // merge data
                        accum.append(getBottle(tag.c_str())->tail());
                        //printf("MERGE %s, got %s\n", tag.c_str(),
                        //     accum.toString().c_str());
                    }
                }
            }
        } while (!done);
    }

    void fromBottle(Bottle& bot, bool wipe=true) {
        if (wipe) {
            clear();
        }
        for (int i=0; i<bot.size(); i++) {
            Value& bb = bot.get(i);
            if (bb.isList()) {
                Bottle *sub = bb.asList();
                putBottle(bb.asList()->get(0).toString().c_str(), *sub);
            }
        }
    }

    ConstString toString() {
        Bottle bot;
        for (std::map<ConstString, PropertyItem>::iterator it = data.begin(); it != data.end(); ++it) {
            PropertyItem& rec = it->second;
            Bottle& sub = bot.addList();
            rec.flush();
            sub.copy(rec.bot);
        }
        return bot.toString();
    }

    // expand any environment variables found
    ConstString expand(const char *txt, Searchable& env, Searchable& env2) {
        //printf("expanding %s\n", txt);
        ConstString input = txt;
        if (input.find('$')==ConstString::npos) {
            // no variables present for sure
            return txt;
        }
        // check if variables present
        ConstString output = "";
        ConstString var = "";
        bool inVar = false;
        bool varHasParen = false;
        bool quoted = false;
        for (int i=0; i<=(int)input.length(); i++) {
            char ch = 0;
            if (i<(int)input.length()) {
                ch = input[i];
            }
            if (quoted) {
                if (!inVar) {
                    output += '\\';
                    if (ch!=0) {
                        output += ch;
                    }
                } else {
                    if (ch!=0) {
                        var += ch;
                    }
                }
                quoted = false;
                continue;
            } else {
                if (ch=='\\') {
                    quoted = true;
                    continue;
                }
            }

            if (inVar) {
                if (isalnum(ch)||(ch=='_')) {
                    var += ch;
                    continue;
                } else {
                    if (ch=='('||ch=='{') {
                        if (var.length()==0) {
                            // ok, just ignore
                            varHasParen = true;
                            continue;
                        }
                    }
                    inVar = false;
                    //printf("VARIABLE %s\n", var.c_str());
                    ConstString add = NetworkBase::getEnvironment(var.c_str()).c_str();
                    if (add=="") {
                        add = env.find(var.c_str()).toString().c_str();
                    }
                    if (add=="") {
                        add = env2.find(var.c_str()).toString().c_str();
                    }
                    if (add=="") {
                        if (var=="__YARP__") {
                            add = "1";
                        }
                    }
                    if (add.find('\\')!=ConstString::npos) {
                        // Specifically when reading from the command
                        // line, we will allow windows-style paths.
                        // Hence we have to break the "\" character
                        ConstString buf = "";
                        for (unsigned int i=0; i<add.length(); i++) {
                            buf += add[i];
                            if (add[i]=='\\') {
                                buf += add[i];
                            }
                        }
                        add = buf;
                    }
                    output += add;
                    var = "";
                    if (varHasParen && (ch=='}'||ch==')')) {
                        continue;
                        // don't need current char
                    }
                }
            }

            if (!inVar) {
                if (ch=='$') {
                    inVar = true;
                    varHasParen = false;
                    continue;
                } else {
                    if (ch!=0) {
                        output += ch;
                    }
                }
            }
        }
        return output.c_str();
    }

    void fromArguments(const char *command, bool wipe=true) {
        char** szarg = new char*[128 + 1];  // maximum 128 arguments
        char* szcmd = new char[strlen(command)+1];
        strcpy(szcmd, command);
        int nargs = 0;
        parseArguments(szcmd, &nargs, szarg, 128);
        szarg[nargs]=YARP_NULLPTR;
        fromCommand(nargs, szarg, wipe);
        // clear allocated memory for arguments
        if (szcmd) {
            delete [] szcmd;
            szcmd = YARP_NULLPTR;
        }
        if (szarg) {
            delete [] szarg;
            szarg = YARP_NULLPTR;
        }
    }

    void parseArguments(char *azParam, int *argc, char **argv, int max_arg) {
        char *pNext = azParam;
        size_t i;
        int j;
        int quoted = 0;
        size_t len = strlen(azParam);

        // Protect spaces inside quotes, but lose the quotes
        for(i = 0; i < len; i++) {
            if ((!quoted) && ('"' == azParam [i])) {
                quoted = 1;
                azParam [i] = ' ';
            } else if ((quoted) && ('"' == azParam [i])) {
                quoted = 0;
                azParam [i] = ' ';
            } else if ((quoted) && (' ' == azParam [i])) {
                azParam [i] = '\1';
            }
        }

        // init
        memset(argv, 0x00, sizeof(char*) * max_arg);
        *argc = 1;
        argv[0] = azParam ;

        while ((YARP_NULLPTR != pNext) && (*argc < max_arg)) {
            splitArguments(pNext, &(argv[*argc]));
            pNext = argv[*argc];

            if (YARP_NULLPTR != argv[*argc]) {
                *argc += 1;
            }
        }

        for(j = 0; j < *argc; j++) {
            len = strlen(argv[j]);
            for(i = 0; i < len; i++) {
                if ('\1' == argv[j][i]) {
                    argv[j][i] = ' ';
                }
            }
        }
    }

    void splitArguments(char *line, char **args) {
        char *pTmp = strchr(line, ' ');
        if (pTmp) {
           *pTmp = '\0';
           pTmp++;
           while ((*pTmp) && (*pTmp == ' ')) {
               pTmp++;
           }
           if (*pTmp == '\0') {
               pTmp = YARP_NULLPTR;
           }
        }
        *args = pTmp;
    }


};


// implementation is a PropertyHelper
#define HELPER(x) (*((PropertyHelper*)(x)))


Property::Property(int hash_size) {
    this->hash_size = hash_size;
    implementation = YARP_NULLPTR;
}


Property::Property(const char *str) {
    hash_size = 0;
    implementation = new PropertyHelper(*this, 0);
    yAssert(implementation!=YARP_NULLPTR);
    fromString(str);
}


Property::Property(const Property& prop) : Searchable(), Portable() {
    hash_size = 0;
    implementation = new PropertyHelper(*this, 0);
    yAssert(implementation!=YARP_NULLPTR);
    fromString(prop.toString());
}


void Property::summon() {
    if (check()) return;
    implementation = new PropertyHelper(*this, hash_size);
    yAssert(implementation!=YARP_NULLPTR);
}


bool Property::check() const {
    return implementation!=YARP_NULLPTR;
}


Property::~Property() {
    if (implementation!=YARP_NULLPTR) {
        delete &HELPER(implementation);
        implementation = YARP_NULLPTR;
    }
}


const Property& Property::operator = (const Property& prop) {
    summon();
    fromString(prop.toString());
    return *this;
}


void Property::put(const ConstString& key, const ConstString& value) {
    summon();
    HELPER(implementation).put(key, value);
}

void Property::put(const ConstString& key, const Value& value) {
    summon();
    HELPER(implementation).put(key, value);
}


void Property::put(const ConstString& key, Value *value) {
    summon();
    HELPER(implementation).put(key, value);
}

void Property::put(const ConstString& key, int value) {
    summon();
    put(key, Value::makeInt(value));
}

void Property::put(const ConstString& key, double value) {
    summon();
    put(key, Value::makeDouble(value));
}

bool Property::check(const ConstString& key) const {
    if (!check()) return false;
    return HELPER(implementation).check(key);
}


void Property::unput(const ConstString& key) {
    summon();
    HELPER(implementation).unput(key);
}


Value& Property::find(const ConstString& key) const {
    if (!check()) return Value::getNullValue();
    return HELPER(implementation).get(key);
}


void Property::clear() {
    summon();
    HELPER(implementation).clear();
}


void Property::fromString(const ConstString& txt, bool wipe) {
    summon();
    HELPER(implementation).fromString(txt, wipe);
}


ConstString Property::toString() const {
    if (!check()) return "";
    return HELPER(implementation).toString();
}

void Property::fromCommand(int argc, char *argv[], bool skipFirst,
                           bool wipe) {
    summon();
    if (skipFirst) {
        argc--;
        argv++;
    }
    HELPER(implementation).fromCommand(argc, argv, wipe);
}

void Property::fromCommand(int argc, const char *argv[], bool skipFirst, bool wipe) {
    summon();
    fromCommand(argc, (char **)argv, skipFirst, wipe);
}

void Property::fromArguments(const char *arguments, bool wipe) {
    summon();
    HELPER(implementation).fromArguments(arguments, wipe);
}

bool Property::fromConfigDir(const ConstString& dirname, const ConstString& section, bool wipe) {
    summon();
    return HELPER(implementation).fromConfigDir(dirname, section, wipe);
}

bool Property::fromConfigFile(const ConstString& fname, bool wipe) {
    summon();
    Property p;
    return fromConfigFile(fname, p, wipe);
}


bool Property::fromConfigFile(const ConstString& fname, Searchable& env, bool wipe) {
    summon();
    return HELPER(implementation).fromConfigFile(fname, env, wipe);
}

void Property::fromConfig(const char *txt, bool wipe) {
    summon();
    Property p;
    fromConfig(txt, p, wipe);
}

void Property::fromConfig(const char *txt, Searchable& env, bool wipe) {
    summon();
    HELPER(implementation).fromConfig(txt, env, wipe);
}


bool Property::read(ConnectionReader& reader) {
    // for now just delegate to Bottle
    Bottle b;
    bool ok = b.read(reader);
    if (ok) {
        fromString(b.toString());
    }
    return ok;
}


bool Property::write(ConnectionWriter& writer) {
    // for now just delegate to Bottle
    Bottle b(toString());
    return b.write(writer);
}


Bottle& Property::findGroup(const ConstString& key) const {
    if (!check()) return Bottle::getNullBottle();
    Bottle *result = HELPER(implementation).getBottle(key);
    if (getMonitor()!=YARP_NULLPTR) {
        SearchReport report;
        report.key = key;
        report.isGroup = true;
        if (result != YARP_NULLPTR) {
            report.isFound = true;
            report.value = result->toString();
        }
        reportToMonitor(report);
        if (result != YARP_NULLPTR) {
            ConstString context = getMonitorContext();
            context += ".";
            context += key;
            result->setMonitor(getMonitor(),
                               context.c_str()); // pass on any monitoring
        }
    }

    if (result!=((Bottle*)YARP_NULLPTR)) { return *result; }
    return Bottle::getNullBottle();
}


void Property::fromQuery(const char *url, bool wipe) {
    summon();
    if (wipe) {
        clear();
    }
    ConstString str = url;
    str += "&";
    ConstString buf = "";
    ConstString key = "";
    ConstString val = "";
    int code = 0;
    int coding = 0;

    for (unsigned int i=0; i<str.length(); i++) {
        char ch = str[i];
        if (ch=='=') {
            key = buf;
            val = "";
            buf = "";
            //printf("adding key %s\n", key.c_str());
        } else if (ch=='&') {
            //printf("adding val %s\n", val.c_str());
            val = buf;
            buf = "";
            if (key!="" && val!="") {
                put(key.c_str(), val.c_str());
            }
            key = "";
        } else if (ch=='?') {
            buf = "";
        } else {
            if (ch=='+') {
                ch = ' ';
            } else if (ch=='%') {
                coding = 2;
            } else {
                if (coding) {
                    int hex = 0;
                    if (ch>='0'&&ch<='9') { hex = ch-'0'; }
                    if (ch>='A'&&ch<='F') { hex = ch-'A'+10; }
                    if (ch>='a'&&ch<='f') { hex = ch-'a'+10; }
                    code *= 16;
                    code += hex;
                    coding--;
                    if (coding == 0) {
                        ch = code;
                    }
                }
            }
            if (coding==0) {
                buf += ch;
            }
        }
    }
}


Property& yarp::os::Property::addGroup(const ConstString& key) {
    summon();
    return HELPER(implementation).addGroup(key);
}
