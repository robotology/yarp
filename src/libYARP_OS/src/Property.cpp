// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Property.h>
#include <yarp/os/Bottle.h>
#include <yarp/Logger.h>
#include <yarp/StringInputStream.h>
#include <yarp/NetType.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

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

    void put(const char *key, BottleBit& bit) {
        PropertyItem *p = getProp(key,true);
        p->singleton = true;
        p->bot.clear();
        p->bot.addString(key);
        p->bot.addBit(bit);
    }

    bool check(const char *key, BottleBit *&output) const {
        PropertyItem *p = getPropNoCreate(key);
        
        return p!=NULL;
    }

    bool check(const char *key) const {
        PropertyItem *p = getPropNoCreate(key);
        return p!=NULL;
    }

    BottleBit& get(const char *key) const {
        String out;
        PropertyItem *p = getPropNoCreate(key);
        if (p!=NULL) {
            return p->bot.get(1);
        }
        return Bottle::getNullBit();
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

    void fromString(const char *txt) {
        Bottle bot;
        bot.fromString(txt);
        fromBottle(bot);
    }

    void fromCommand(int argc, char *argv[]) {
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
            accum.addBit(work.c_str());
        }
        if (tag!="") {
            total.addList().copy(accum);
        }
        fromBottle(total);
    }


    void fromConfigFile(const char *fname) {
        ifstream fin(fname);
        String txt;
        if (fin.fail()) {
            YARP_ERROR(Logger::get(),String("cannot read from ") +
                       fname);
        }
        while (!(fin.eof()||fin.fail())) {
            char buf[1000];
            fin.getline(buf,sizeof(buf));
            if (!fin.eof()) {
                txt += buf;
                txt += "\n";
            }
        }
        fromConfig(txt.c_str());
    }

    void fromConfig(const char *txt) {
        StringInputStream sis;
        sis.add(txt);
        clear();
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
                int comment = buf.strstr("//");
                if (comment>=0) {
                    buf = buf.substr(0,comment);
                }

                if (buf[0]=='[') {
                    int stop = buf.strstr("]");
                    if (stop>=0) {
                        buf = buf.substr(1,stop-1);
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


    void fromBottle(Bottle& bot) {
        for (int i=0; i<bot.size(); i++) {
            BottleBit& bb = bot.get(i);
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
};


// implementation is a PropertyHelper
#define HELPER(x) (*((PropertyHelper*)(x)))


Property::Property() {
    implementation = new PropertyHelper;
    YARP_ASSERT(implementation!=NULL);
}


Property::~Property() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


void Property::put(const char *key, const char *val) {
    HELPER(implementation).put(key,val);
}

void Property::put(const char *key, BottleBit& bit) {
    HELPER(implementation).put(key,bit);
}


/*
bool Property::check(const char *key, BottleBit *&output) {
    bool ok = false;
    if (HELPER(implementation).check(key)) {
        output = &find(key);
        ok = true;
    }
    return ok;
}
*/

bool Property::check(const char *key) const {
    return HELPER(implementation).check(key);
}


BottleBit& Property::get(const char *key) const {
    return HELPER(implementation).get(key);
}

Bottle *Property::getList(const char *key) const {
    return HELPER(implementation).getBottle(key);
}

ConstString Property::getString(const char *key) const {
    return HELPER(implementation).get(key).toString();
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


void Property::fromString(const char *txt) {
    HELPER(implementation).fromString(txt);
}


ConstString Property::toString() const {
    return HELPER(implementation).toString();
}

void Property::fromCommand(int argc, char *argv[], bool skipFirst) {
    if (skipFirst) {
        argc--;
        argv++;
    }
    HELPER(implementation).fromCommand(argc,argv);
}

void Property::fromConfigFile(const char *fname) {
    HELPER(implementation).fromConfigFile(fname);
}


void Property::fromConfig(const char *txt) {
    HELPER(implementation).fromConfig(txt);
}


