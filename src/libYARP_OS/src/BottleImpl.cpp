// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/BottleImpl.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/StringOutputStream.h>
#include <yarp/StringInputStream.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

using namespace yarp;

const int StoreInt::code = 1;
const int StoreString::code = 5;
const int StoreDouble::code = 2;
const int StoreList::code = 16;

yarp::StoreNull BottleImpl::storeNull;


BottleImpl::BottleImpl() {
    dirty = true;
}


BottleImpl::~BottleImpl() {
    clear();
}


void BottleImpl::add(Storable *s) {
    content.push_back(s);
    dirty = true;
}


void BottleImpl::clear() {
    for (unsigned int i=0; i<content.size(); i++) {
        delete content[i];
    }
    content.clear();
    dirty = true;
}

void BottleImpl::smartAdd(const String& str) {
    if (str.length()>0) {
        char ch = str[0];
        Storable *s = NULL;
        if (ch>='0'&&ch<='9'||ch=='+'||ch=='-'||ch=='.') {
            if (str.strstr(".")<0) {
                s = new StoreInt(0);
            } else {
                s = new StoreDouble(0);
            }
        } else if (ch=='[') {
            s = new StoreList();
        } else {
            s = new StoreString("");
        }
        if (s!=NULL) {
            s->fromStringNested(str);
            //ACE_OS::printf("*** smartAdd [%s] [%s]\n", str.c_str(), s->toString().c_str());
            add(s);
        }
    }
}

void BottleImpl::fromString(const String& line) {
    clear();
    dirty = true;
    String arg = "";
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    String nline = line + " ";
    for (unsigned int i=0; i<nline.length(); i++) {
        char ch = nline[i];
        if (back) {
            arg += ch;
            back = false;
        } else {
            if (!begun) {
                if (ch!=' '&&ch!='\t') {
                    begun = true;
                }
            }
            if (begun) {
                if (ch=='\"') {
                    if (!quoted) {
                        quoted = true;
                    } else {
                        quoted = false;
                    }
                }
                if (!quoted) {
                    if (ch=='[') {
                        nested++;
                    }
                    if (ch==']') {
                        nested--;
                    }
                }
                if (ch=='\\') {
                    back = true;
                    arg += ch;
                } else {
                    if ((!quoted)&&(ch==' '||ch=='\t')&&(nested==0)) {
                        smartAdd(arg);
                        arg = "";
                        begun = false;
                    } else {
                        arg += ch;
                    }
                }
            }
        }
    }
}

String BottleImpl::toString() {
    String result = "";
    for (unsigned int i=0; i<content.size(); i++) {
        if (i>0) { result += " "; }
        Storable& s = *content[i];
        result += s.toStringNested();
    }
    return result;
}

int BottleImpl::size() {
    return content.size();
}

bool BottleImpl::fromBytes(const Bytes& data) {
    String wrapper;
    wrapper.set(data.get(),data.length(),0);
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis,NULL,route,data.length(),false);

    clear();
    dirty = true; // for clarity

    try {
        while (reader.getSize()>0) {
            int id = reader.expectInt();
            Storable *storable = NULL;
            switch (id) {
            case StoreInt::code:
                storable = new StoreInt();
                break;
            case StoreDouble::code:
                storable = new StoreDouble();
                break;
            case StoreString::code:
                storable = new StoreString();
                break;
            case StoreList::code:
                storable = new StoreList();
                break;
            }
            if (storable==NULL) {
                YARP_ERROR(Logger::get(), "BottleImpl reader failed");
                throw IOException("BottleImpl reader failed - unrecognized format?");
            }
            storable->read(reader);
            add(storable);
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " bottle reader stopped");
        return false;
    }
    return true;
}

void BottleImpl::toBytes(const Bytes& data) {
    synch();
    YARP_ASSERT(data.length()==byteCount());
    ACE_OS::memcpy(data.get(),getBytes(),byteCount());
}


const char *BottleImpl::getBytes() {
    synch();
    return &data[0];
}

int BottleImpl::byteCount() {
    synch();
    return data.size();
}

bool BottleImpl::write(ConnectionWriter& writer) {
    try {
        // could simplify this if knew lengths of blocks up front
        if (writer.isTextMode()) {
            //writer.appendLine(toString());
            writer.appendString(toString().c_str(),'\n');
        } else {
            String name = "YARP2";
            writer.appendInt(name.length()+1);
            writer.appendString(name.c_str(),'\0');
            synch();
            writer.appendInt(byteCount());
            //writer.appendBlockCopy(Bytes((char*)getBytes(),byteCount()));
            writer.appendBlock((char*)getBytes(),byteCount());
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), String("Bottle write exception: ")+e.toString());
        return false;
    }
    return true;
}


bool BottleImpl::read(ConnectionReader& reader) {
    bool result = false;
    try {
        if (reader.isTextMode()) {
            String str = reader.expectText().c_str();
            bool done = (str.length()<=0);
            while (!done) {
                if (str[str.length()-1]=='\\') {
                    str = str.substr(0,str.length()-1);
                    str += reader.expectText().c_str();
                } else {
                    done = true;
                }
            }
            fromString(str);
            result = true;
        } else {
            int len = reader.expectInt();
            //String name = reader.expectString(len);
            String buf((size_t)len);
            reader.expectBlock((const char *)buf.c_str(),len);
            String name = buf.c_str();
            int bct = reader.expectInt();
            ManagedBytes b(bct);
            reader.expectBlock(b.get(),b.length());
            result = fromBytes(b.bytes());
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), String("Bottle read exception: ")+e.toString());
        // leave result false
    }
    return result;
}


void BottleImpl::synch() {
    if (dirty) {
        data.clear();
        //StringOutputStream sos;
        BufferedConnectionWriter writer;
        for (unsigned int i=0; i<content.size(); i++) {
            Storable *s = content[i];
            writer.appendInt(s->getCode());
            s->write(writer);
        }
        //buf.write(sos);
        String str = writer.toString();
        data.resize(str.length(),' ');
        memcpy(&data[0],str.c_str(),str.length());
        dirty = false;
    }
}



////////////////////////////////////////////////////////////////////////////
// StoreInt

String StoreInt::toStringFlex() const {
    char buf[256];
    ACE_OS::sprintf(buf,"%d",x);
    return String(buf);
}

void StoreInt::fromString(const String& src) {
    x = ACE_OS::atoi(src.c_str());
}

bool StoreInt::read(ConnectionReader& reader) {
    x = reader.expectInt();
    return true;
}

bool StoreInt::write(ConnectionWriter& writer) {
    writer.appendInt(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreDouble

String StoreDouble::toStringFlex() const {
    char buf[256];
    ACE_OS::sprintf(buf,"%g",x);
    String str(buf);
    if (str.strstr(".")<0) {
        str += ".0";
    }
    return str;
}

void StoreDouble::fromString(const String& src) {
    x = ACE_OS::strtod(src.c_str(),NULL);
}

bool StoreDouble::read(ConnectionReader& reader) {
    reader.expectBlock((const char*)&x,sizeof(x));
    return true;
}

bool StoreDouble::write(ConnectionWriter& writer) {
    //writer.appendBlockCopy(Bytes((char*)&x,sizeof(x)));
    writer.appendBlock((char*)&x,sizeof(x));
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreString


String StoreString::toStringFlex() const {
    return x;
}

String StoreString::toStringNested() const {
    // quoting code: very inefficient, but portable
    String result;
    result += "\"";
    for (unsigned int i=0; i<x.length(); i++) {
        char ch = x[i];
        if (ch=='\\'||ch=='\"') {
            result += '\\';
        }
        result += ch;
    }
    result += "\"";
    return result;
}

void StoreString::fromString(const String& src) {
    x = src;
}

void StoreString::fromStringNested(const String& src) {
    // unquoting code: very inefficient, but portable
    String result = "";
    x = "";
    int len = src.length();
    if (len>0) {
        bool skip = false;
        bool back = false;
        if (src[0]=='\"') {
            skip = true;
        }
        for (int i=0; i<len; i++) {
            if (skip&&(i==0||i==len-1)) {
                // omit
            } else {
                char ch = src[i];
                if (ch=='\\') {
                    if (!back) {
                        back = true;
                    } else {
                        x += '\\';
                        back = false;
                    }
                } else {
                    back = false;
                    x += ch;
                }
            }
        }
    }
}


bool StoreString::read(ConnectionReader& reader) {
    int len = reader.expectInt();
    String buf((size_t)len);
    reader.expectBlock((const char *)buf.c_str(),len);
    x = buf.c_str();
    //x = reader.expectString(len);
    return true;
}

bool StoreString::write(ConnectionWriter& writer) {
    writer.appendInt(x.length()+1);
    writer.appendString(x.c_str(),'\0');
    return true;
}





String StoreList::toStringFlex() const {
    return String(content.toString().c_str());
}

String StoreList::toStringNested() const {
    return String("[") + content.toString().c_str() + "]";
}

void StoreList::fromString(const String& src) {
    content.fromString(src.c_str());
}

void StoreList::fromStringNested(const String& src) {
    if (src.length()>0) {
        if (src[0]=='[') {
            // ignore first [ and last ]
            String buf = src.substr(1,src.length()-2);
            content.fromString(buf.c_str());
        }
    }
}

bool StoreList::read(ConnectionReader& reader) {
    // not using the most efficient representation
    content.read(reader);
    return true;
}

bool StoreList::write(ConnectionWriter& writer) {
    // not using the most efficient representation
    content.write(writer);
    return true;
}





bool BottleImpl::isInt(int index) {
    if (index>=0 && index<size()) {
        return content[index]->getCode() == StoreInt::code;
    }
    return false;
}


bool BottleImpl::isString(int index) {
    if (index>=0 && index<size()) {
        return content[index]->getCode() == StoreString::code;
    }
    return false;
}

bool BottleImpl::isDouble(int index) {
    if (index>=0 && index<size()) {
        return content[index]->getCode() == StoreDouble::code;
    }
    return false;
}


bool BottleImpl::isList(int index) {
    if (index>=0 && index<size()) {
        return content[index]->getCode() == StoreList::code;
    }
    return false;
}



yarp::os::BottleBit& BottleImpl::get(int index) {
    if (index>=0 && index<size()) {
        return *(content[index]);
    }
    return storeNull;
}

int BottleImpl::getInt(int index) {
    if (!isInt(index)) { return 0; }
    return content[index]->asInt();
}

yarp::os::ConstString BottleImpl::getString(int index) {
    if (!isString(index)) { return ""; }
    return content[index]->asString();
}

double BottleImpl::getDouble(int index) {
    if (!isDouble(index)) { return 0; }
    return content[index]->asDouble();
}

yarp::os::Bottle *BottleImpl::getList(int index) {
    if (!isList(index)) { return NULL; }
    return &(((StoreList*)content[index])->internal());
}


yarp::os::Bottle& BottleImpl::addList() {
    StoreList *lst = new StoreList();
    add(lst);
    return lst->internal();
}


