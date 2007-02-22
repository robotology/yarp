// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Property.h>
#include <yarp/os/Bottle.h>
#include <yarp/Logger.h>
#include <yarp/StringInputStream.h>
#include <yarp/NetType.h>
#include <yarp/NameConfig.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/OS.h>

#include <fstream>
using namespace std;

using namespace yarp;
using namespace yarp::os;

class PropertyItem {
public:
    Bottle bot;
    bool singleton;

    PropertyItem() {
        singleton = false;
    }

    ConstString toString() {
        return bot.toString();
    }
};

class PropertyHelper {
public:
    ACE_Hash_Map_Manager<String,PropertyItem,ACE_Null_Mutex> data;
    Property& owner;

    PropertyHelper(Property& owner) : owner(owner) {}

    PropertyItem *getPropNoCreate(const char *key) const {
        String n(key);
        ACE_Hash_Map_Entry<String,PropertyItem> *entry = NULL;
        int result = data.find(n,entry);
        if (result==-1) {
            return NULL;
        }
        YARP_ASSERT(result!=-1);
        YARP_ASSERT(entry!=NULL);
        return &(entry->int_id_);
    }
    
    PropertyItem *getProp(const char *key, bool create = true) {
        String n(key);
        ACE_Hash_Map_Entry<String,PropertyItem> *entry = NULL;
        int result = data.find(n,entry);
        if (result==-1) {
            if (!create) {
                return NULL;
            }
            data.bind(n,PropertyItem());
            result = data.find(n,entry);
        }
        YARP_ASSERT(result!=-1);
        YARP_ASSERT(entry!=NULL);
        return &(entry->int_id_);
    }
    
    void put(const char *key, const char *val) {
        PropertyItem *p = getProp(key,true);
        p->singleton = true;
        p->bot.clear();
        p->bot.addString(key);
        p->bot.addString(val);
    }

    void put(const char *key, const Value& bit) {
        PropertyItem *p = getProp(key,true);
        p->singleton = true;
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    void put(const char *key, Value *bit) {
        PropertyItem *p = getProp(key,true);
        p->singleton = true;
        p->bot.clear();
        p->bot.addString(key);
        p->bot.add(bit);
    }

    bool check(const char *key, Value *&output) const {
        PropertyItem *p = getPropNoCreate(key);
        
        return p!=NULL;
    }

    void unput(const char *key) {
        data.unbind(String(key));
    }

    bool check(const char *key) const {
        PropertyItem *p = getPropNoCreate(key);
        if (owner.getMonitor()!=NULL) {
            SearchReport report;
            report.key = key;
            report.isFound = (p!=NULL);
            owner.reportToMonitor(report);
        }
        return p!=NULL;
    }

    Value& get(const char *key) const {
        String out;
        PropertyItem *p = getPropNoCreate(key);
        if (p!=NULL) {
            if (owner.getMonitor()!=NULL) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                report.value = p->bot.get(1).toString();
                owner.reportToMonitor(report);
            }
            return p->bot.get(1);
        }
        if (owner.getMonitor()!=NULL) {
            SearchReport report;
            report.key = key;
            owner.reportToMonitor(report);
        }
        return Value::getNullValue();
    }

    Bottle& putBottle(const char *key, const Bottle& val) {
        PropertyItem *p = getProp(key,true);
        p->singleton = false;
        // inefficient! copy not implemented yet...
        p->bot.fromString(val.toString().c_str());
        return p->bot;
    }


    Bottle& putBottle(const char *key) {
        PropertyItem *p = getProp(key,true);
        p->singleton = false;
        p->bot.clear();
        return p->bot;
    }


    Bottle *getBottle(const char *key) const {
        PropertyItem *p = getPropNoCreate(key);
        if (p!=NULL) {
            return &(p->bot);
        }
        return NULL;
    }
    
    void clear() {
        data.unbind_all();
    }

    void fromString(const char *txt,bool wipe=true) {
        Bottle bot;
        bot.fromString(txt);
        fromBottle(bot,wipe);
    }

    void fromCommand(int argc, char *argv[],bool wipe=true) {
        String tag = "";
        Bottle accum;
        Bottle total;
        for (int i=0; i<argc; i++) {
            String work = argv[i];
            bool isTag = false;
            if (work.length()>=2) {
                if (work[0]=='-'&&work[1]=='-') {
                    work = work.substr(2,work.length()-2);
                    isTag = true;
                }
            }
            if (isTag) {
                if (tag!="") {
                    total.addList().copy(accum);
                }
                tag = work;
                accum.clear();
            }
            accum.add(Value::makeValue(work.c_str()));
        }
        if (tag!="") {
            total.addList().copy(accum);
        }
        fromBottle(total,wipe);
    }


    bool fromConfigFile(const char *fname,Searchable& env, bool wipe=true) {
        ifstream fin(fname);
        String txt;
        if (fin.fail()) {
            YARP_DEBUG(Logger::get(),String("cannot read from ") +
                       fname);
            return false;
        }
        while (!(fin.eof()||fin.fail())) {
            char buf[1000];
            fin.getline(buf,sizeof(buf));
            if (!fin.eof()) {
                txt += buf;
                txt += "\n";
            }
        }
        fromConfig(txt.c_str(),env,wipe);
        return true;
    }


    void fromConfig(const char *txt,Searchable& env, bool wipe=true) {
        StringInputStream sis;
        sis.add(txt);
        sis.add("\n");
        if (wipe) {
            clear();
        }
        String tag = "";
        Bottle accum;
        bool done = false;
        do {
            bool isTag = false;
            String buf;
            try {
                buf = NetType::readLine(sis);
            } catch (IOException e) {
                done = true;
            }
            if (!done) {
                // this comment filter is not safe for quoting
                if (buf.strstr("//")!=String::npos) {
                    buf = buf.substr(0,buf.strstr("//"));
                }

                // expand any environment references
                buf = expand(buf.c_str(),env).c_str();

                if (buf[0]=='[') {
                    int stop = buf.strstr("]");
                    if (stop>=0) {
                        buf = buf.substr(1,stop-1);
                        int space = buf.strstr(" ");
                        if (space>=0) {
                            Bottle bot(buf.c_str());
                            if (bot.size()==2) {
                                buf = bot.get(1).toString().c_str();
                                String key = bot.get(0).toString().c_str();
                                Bottle *target = getBottle(key.c_str());
                                if (target==NULL) {
                                    Bottle init;
                                    init.addString(key.c_str());
                                    init.addString(buf.c_str());
                                    putBottle(key.c_str(),init);
                                } else {
                                    target->addString(buf.c_str());
                                }
                            }
                        }
                        isTag = true;
                    }
                }
            }
            if (!isTag) {
                Bottle bot;
                bot.fromString(buf.c_str());
                if (bot.size()>=1) {
                    if (tag=="") {
                        putBottle(bot.get(0).toString().c_str(),bot);
                    } else {
                        accum.addList().copy(bot);
                    }
                }
            }
            if (isTag||done) {
                if (tag!="") {
                    if (accum.size()>=1) {
                        putBottle(tag.c_str(),accum);
                    }
                    tag = "";
                }
                tag = buf;
                accum.clear();
                accum.addString(tag.c_str());
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
                putBottle(bb.asList()->get(0).toString().c_str(),*sub);
            }
        }
    }

    ConstString toString() {
        Bottle bot;
        for (ACE_Hash_Map_Manager<String,PropertyItem,ACE_Null_Mutex>::iterator
                 it = data.begin(); it!=data.end(); it++) {
            PropertyItem& rec = (*it).int_id_;
            Bottle& sub = bot.addList();
            sub.copy(rec.bot);
        }
        return bot.toString();
    }

    // expand any environment variables found
    ConstString expand(const char *txt, Searchable& env) {
        //printf("expanding %s\n", txt);
        String input = txt;
        if (input.strstr("$")<0) {
            // no variables present for sure
            return txt;
        }
        // check if variables present
        String output = "";
        String var = "";
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
                if (ACE_OS::ace_isalnum(ch)||(ch=='_')) {
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
                    String add = NameConfig::getEnv(var);
                    if (add=="") {
                        add = env.find(var.c_str()).asString().c_str();
                    }
                    if (add=="") {
                        if (var=="__YARP__") {
                            add = "1";
                        }
                    }
                    output += add;
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
};


// implementation is a PropertyHelper
#define HELPER(x) (*((PropertyHelper*)(x)))


Property::Property() {
    implementation = new PropertyHelper(*this);
    YARP_ASSERT(implementation!=NULL);
}


Property::Property(const char *str) {
    implementation = new PropertyHelper(*this);
    YARP_ASSERT(implementation!=NULL);
    fromString(str);
}


Property::Property(const Property& prop) {
    implementation = new PropertyHelper(*this);
    YARP_ASSERT(implementation!=NULL);
    fromString(prop.toString());
}


Property::~Property() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


const Property& Property::operator = (const Property& prop) {
    fromString(prop.toString());
    return *this;
}


void Property::put(const char *key, const char *val) {
    HELPER(implementation).put(key,val);
}

void Property::put(const char *key, const Value& value) {
    HELPER(implementation).put(key,value);
}


void Property::put(const char *key, Value *value) {
    HELPER(implementation).put(key,value);
}


/*
  bool Property::check(const char *key, Value *&output) {
  bool ok = false;
  if (HELPER(implementation).check(key)) {
  output = &find(key);
  ok = true;
  }
  return ok;
  }
*/

bool Property::check(const char *key) {
    return HELPER(implementation).check(key);
}


void Property::unput(const char *key) {
    HELPER(implementation).unput(key);
}


Value& Property::find(const char *key) {
    return HELPER(implementation).get(key);
}


/*
  Bottle& Property::putBottle(const char *key, const Bottle& val) {
  return HELPER(implementation).putBottle(key,val);
  }

  Bottle& Property::putBottle(const char *key) {
  return HELPER(implementation).putBottle(key);
  }

  Bottle *Property::getBottle(const char *key) const {
  return HELPER(implementation).getBottle(key);
  }
*/

void Property::clear() {
    HELPER(implementation).clear();
}


void Property::fromString(const char *txt,bool wipe) {
    HELPER(implementation).fromString(txt,wipe);
}


ConstString Property::toString() const {
    return HELPER(implementation).toString();
}

void Property::fromCommand(int argc, char *argv[], bool skipFirst,
                           bool wipe) {
    if (skipFirst) {
        argc--;
        argv++;
    }
    HELPER(implementation).fromCommand(argc,argv,wipe);
}

bool Property::fromConfigFile(const char *fname,Searchable& env,bool wipe) {
    return HELPER(implementation).fromConfigFile(fname,env,wipe);
}


void Property::fromConfig(const char *txt,Searchable& env,bool wipe) {
    HELPER(implementation).fromConfig(txt,env,wipe);
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


Bottle& Property::findGroup(const char *key) {
    Bottle *result = HELPER(implementation).getBottle(key);
    if (getMonitor()!=NULL) {
        SearchReport report;
        report.key = key;
        report.isGroup = true;
        if (result!=0/*NULL*/) {
            report.isFound = true;
            report.value = result->toString();
        }
        reportToMonitor(report);
        if (result!=0/*NULL*/) {
            String context = getContext().c_str();
            context += ".";
            context += key;
            result->setMonitor(getMonitor(),
                               context.c_str()); // pass on any monitoring
        }
    }

    if (result!=((Bottle*)0)) { return *result; }
    return Bottle::getNullBottle();
}


void Property::fromQuery(const char *url, bool wipe) {
    if (wipe) {
        clear();
    }
    String str = url;
    str += "&";
    String buf = "";
    String key = "";
    String val = "";
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
                put(key.c_str(),val.c_str());
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
