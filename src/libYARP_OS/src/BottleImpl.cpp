// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/BottleImpl.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/StringOutputStream.h>
#include <yarp/StringInputStream.h>

#include <yarp/os/Vocab.h>
#include <yarp/os/NetFloat64.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

using namespace yarp;
using namespace yarp::os;


//#define YMSG(x) ACE_OS::printf x;
//#define YTRACE(x) YMSG(("at %s\n",x))

#define YMSG(x)
#define YTRACE(x) 

// YARP1 compatible codes
//const int StoreInt::code = 1;
//const int StoreString::code = 5;
//const int StoreDouble::code = 2;
//const int StoreList::code = 16;
//const int StoreVocab::code = 32;
//const int StoreBlob::code = 33;
//#define USE_YARP1_PREFIX

// new YARP2 codes
const int StoreInt::code = BOTTLE_TAG_INT;
const int StoreVocab::code = BOTTLE_TAG_VOCAB;
const int StoreDouble::code = BOTTLE_TAG_DOUBLE;
const int StoreString::code = BOTTLE_TAG_STRING;
const int StoreBlob::code = BOTTLE_TAG_BLOB;
const int StoreList::code = BOTTLE_TAG_LIST;

#define UNIT_MASK (BOTTLE_TAG_INT|BOTTLE_TAG_VOCAB|BOTTLE_TAG_DOUBLE|BOTTLE_TAG_STRING|BOTTLE_TAG_BLOB)
#define GROUP_MASK (BOTTLE_TAG_LIST)


yarp::StoreNull BottleImpl::storeNull;


BottleImpl::BottleImpl() {
    dirty = true;
    nested = false;
    speciality = 0;
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
        bool numberLike = true;
        bool preamble = true;
        bool hexActive = false;
        int periodCount = 0;
        for (int i=0; i<(int)str.length(); i++) {
            char ch2 = str[i];
            if (ch=='.') {
                periodCount++;
                if (periodCount>1) {
                    numberLike = false;
                }
            }
            if (preamble) {
                if (ch2=='x'||ch2=='X') {
                    hexActive = true;
                    continue;
                }
            }
            if (preamble) {
                if (ch2=='0'||ch2=='+'||ch2=='-') {
                    continue;
                }
            }
            preamble = false;
            if (!((ch2>='0'&&ch2<='9')||ch2=='.'||ch2=='e'||ch2=='E'||
                  (hexActive&&((ch2>='a'&&ch2<='f')||
                                 (ch2>='A'&&ch2<='F'))))) {
                numberLike = false;
                break;
            }
        }

        if (numberLike && (ch>='0'&&ch<='9'||ch=='+'||ch=='-'||ch=='.')) {
            if (str.strstr(".")==String::npos) {
                s = new StoreInt(0);
            } else {
                s = new StoreDouble(0);
            }
        } else if (ch=='(') {
            s = new StoreList();
        } else if (ch=='[') {
            s = new StoreVocab();
        } else if (ch=='{') {
            s = new StoreBlob();
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
    int nestedAlt = 0;
    String nline = line + " ";

    for (unsigned int i=0; i<nline.length(); i++) {
        char ch = nline[i];
        if (back) {
            arg += ch;
            back = false;
        } else {
            if (!begun) {
                if (ch!=' '&&ch!='\t'&&ch!='\n'&&ch!='\r') {
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
                    if (ch=='(') {
                        nested++;
                    }
                    if (ch==')') {
                        nested--;
                    }
                    if (ch=='{') {
                        nestedAlt++;
                    }
                    if (ch=='}') {
                        nestedAlt--;
                    }
                }
                if (ch=='\\') {
                    back = true;
                    arg += ch;
                } else {
                    if ((!quoted)&&(ch==' '||ch=='\t'||ch=='\n'||ch=='\r')
                        &&(nestedAlt==0)
                        &&(nested==0)) {
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

bool BottleImpl::isComplete(const char *txt) {
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    int nestedAlt = 0;
    String nline = txt;
    nline += " ";

    for (unsigned int i=0; i<nline.length(); i++) {
        char ch = nline[i];
        if (back) {
            back = false;
        } else {
            if (!begun) {
                if (ch!=' '&&ch!='\t'&&ch!='\n'&&ch!='\r') {
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
                    if (ch=='(') {
                        nested++;
                    }
                    if (ch==')') {
                        nested--;
                    }
                    if (ch=='{') {
                        nestedAlt++;
                    }
                    if (ch=='}') {
                        nestedAlt--;
                    }
                }
                if (ch=='\\') {
                    back = true;
                    //arg += ch;
                } else {
                    if ((!quoted)&&(ch==' '||ch=='\t'||ch=='\n'||ch=='\r')
                        &&(nestedAlt==0)
                        &&(nested==0)) {
                        //smartAdd(arg);
                        begun = false;
                    } else {
                        //arg += ch;
                    }
                }
            }
        }
    }
    return nested==0 && nestedAlt==0 && quoted==false;
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

int BottleImpl::size() const {
    return content.size();
}


bool BottleImpl::fromBytes(ConnectionReader& reader) {
    int id = speciality;
    YMSG(("READING, nest flag is %d\n", nested));
    if (id==0) {
        id = reader.expectInt();
        YMSG(("READ subcode %d\n", id));
    } else {
        YMSG(("READ skipped subcode %d\n", speciality));
    }
    Storable *storable = NULL;
    int subCode = 0;
    switch (id) {
    case StoreInt::code:
        storable = new StoreInt();
        break;
    case StoreVocab::code:
        storable = new StoreVocab();
        break;
    case StoreDouble::code:
        storable = new StoreDouble();
        break;
    case StoreString::code:
        storable = new StoreString();
        break;
    case StoreBlob::code:
        storable = new StoreBlob();
        break;
    case StoreList::code:
        storable = new StoreList();
        YARP_ASSERT(storable!=NULL);
        storable->asList()->setNested(true);
        break;
    default:
        if ((id&GROUP_MASK)!=0) {
            // typed list
            subCode = (id&UNIT_MASK);
            storable = new StoreList();
            YARP_ASSERT(storable!=NULL);
            storable->asList()->specialize(subCode);
            storable->asList()->setNested(true);
        }
        break;
    }
    if (storable==NULL) {
        YARP_ERROR(Logger::get(), "BottleImpl reader failed");
        throw IOException((String("BottleImpl reader failed - unrecognized format? ") + NetType::toString(id)).c_str());
    }
    storable->read(reader);
    add(storable);
    return true;
}


void BottleImpl::fromBinary(const char *text, int len) {
    String wrapper;
    wrapper.set(text,len,0);
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis,NULL,route,len,false);
    read(reader);
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
        if (!nested) {

            clear();
            specialize(0);
            
            int code = reader.expectInt();
            YMSG(("READ got top level code %d\n", code));
            code = code & UNIT_MASK;
            if (code!=0) {
                specialize(code);
            }
        }
        int len = reader.expectInt();
        YMSG(("READ bottle length %d\n", len));
        for (int i=0; i<len; i++) {
            fromBytes(reader);
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " -- bottle reader stopped");
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
    YMSG(("am I nested? %d\n", nested));
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
#ifdef USE_YARP1_PREFIX
            if (!nested) {
                String name = "YARP2";
                writer.appendInt(name.length()+1);
                writer.appendString(name.c_str(),'\0');
            }
#endif
            synch();
            /*
            if (!nested) {
                // No byte count any more, to facilitate nesting
                //YMSG(("bottle byte count %d\n",byteCount()));
                //writer.appendInt(byteCount()+sizeof(NetInt32));

                writer.appendInt(StoreList::code + speciality);
            }
            */
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
                    if (isComplete(str.c_str())) {
                        done = true;
                    } else {
                        str += "\n";
                        str += reader.expectText().c_str();
                    }
                }
            }
            fromString(str);
            result = true;
        } else {
#if USE_YARP1_PREFIX
            if (!nested) {
                int len = reader.expectInt();
                //String name = reader.expectString(len);
                String buf((size_t)len);
                reader.expectBlock((const char *)buf.c_str(),len);
                String name = buf.c_str();
            }
#endif
            if (!nested) {
                // no byte length any more to facilitate nesting
                //reader.expectInt(); // the bottle byte ct; ignored

                clear();
                specialize(0);

                int code = reader.expectInt();
                YMSG(("READ got top level code %d\n", code));
                code = code & UNIT_MASK;
                if (code!=0) {
                    specialize(code);
                }
            }

            //ManagedBytes b(bct);
            //reader.expectBlock(b.get(),b.length());
            //result = fromBytes(b.bytes());

            result = true;
            clear();
            dirty = true; // for clarity

            int len = 0;
            int i = 0;
            try {
                len = reader.expectInt();
                YMSG(("READ got length %d\n", len));
                for (i=0; i<len; i++) {
                    fromBytes(reader);
                }
            } catch (IOException e) {
                YARP_DEBUG(Logger::get(), e.toString() + 
                           " -- bottle reader stopped at " +
                           NetType::toString(i) + " of " +
                           NetType::toString(len));
                result = false;;
            }


        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), String("Bottle read exception: ")+e.toString());
        // leave result false
    }
    return result;
}


void BottleImpl::synch() {
    if (dirty) {
        if (!nested) {
            subCode();
            YMSG(("bottle code %d\n",StoreList::code + subCode()));
        }
        data.clear();
        //StringOutputStream sos;
        BufferedConnectionWriter writer;
        if (!nested) {
            writer.appendInt(StoreList::code + speciality);
            YMSG(("wrote bottle code %d\n",StoreList::code + speciality));
        }
        YMSG(("bottle length %d\n",size()));
        writer.appendInt(size());
        for (unsigned int i=0; i<content.size(); i++) {
            Storable *s = content[i];
            if (speciality==0) {
                YMSG(("subcode %d\n",s->getCode()));
                writer.appendInt(s->getCode());
            } else {
                YMSG(("skipped subcode %d\n",s->getCode()));
                YARP_ASSERT(speciality==s->getCode());
            }
            if (s->isList()) {
                s->asList()->setNested(true);
            }
            s->write(writer);
        }
        //buf.write(sos);
        String str = writer.toString();
        //int extra = 0;
        //if (!nested) {
        //  extra = sizeof(NetInt32);
        //}
        data.resize(str.length(),' ');
        memcpy(&data[0],str.c_str(),str.length());
        /*
        if (!nested) {
            Bytes b(&data[0],sizeof(NetInt32));
            NetType::netInt(StoreList::code + speciality,b);
        }
        */
        dirty = false;
    }
}


void BottleImpl::specialize(int subCode) {
    speciality = subCode;
}


int BottleImpl::getSpecialization() {
    return speciality;
}

void BottleImpl::setNested(bool nested) {
    this->nested = nested;
}

////////////////////////////////////////////////////////////////////////////
// StoreInt

String StoreInt::toStringFlex() const {
    char buf[256];
    ACE_OS::sprintf(buf,"%d",x);
    return String(buf);
}

void StoreInt::fromString(const String& src) {
    //x = ACE_OS::atoi(src.c_str());
    x = ACE_OS::strtol(src.c_str(), (char **)NULL, 0);
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
// StoreVocab

String StoreVocab::toStringFlex() const {
    return String(Vocab::decode(x).c_str());
}

void StoreVocab::fromString(const String& src) {
    x = Vocab::encode(src.c_str());
}

String StoreVocab::toStringNested() const {
    return String("[") + toStringFlex() + "]";
}

void StoreVocab::fromStringNested(const String& src) {
    if (src.length()>0) {
        if (src[0]=='[') {
            // ignore first [ and last ]
            String buf = src.substr(1,src.length()-2);
            fromString(buf.c_str());
        }
    }
}

bool StoreVocab::read(ConnectionReader& reader) {
    x = reader.expectInt();
    return true;
}

bool StoreVocab::write(ConnectionWriter& writer) {
    writer.appendInt(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreDouble

String StoreDouble::toStringFlex() const {
    char buf[256];
    ACE_OS::sprintf(buf,"%f",x);
    String str(buf);
    if (str.strstr(".")==String::npos) {
        str += ".0";
    }
    int ct = 0;
    for (int i=str.length()-1; i>=0; i--) {
        if (str[i]!='0') {
            if (str[i]=='.') {
                ct--;
                i++;
            }
            if (ct>=1) {
                str = str.substr(0,i+1);
            }
            break;
        }
        ct++;
    }
    return str;
}

void StoreDouble::fromString(const String& src) {
    x = ACE_OS::strtod(src.c_str(),NULL);
}

bool StoreDouble::read(ConnectionReader& reader) {
    NetFloat64 flt = 0;
    reader.expectBlock((const char*)&flt,sizeof(flt));
    x = flt;
    return true;
}

bool StoreDouble::write(ConnectionWriter& writer) {
    //writer.appendBlockCopy(Bytes((char*)&x,sizeof(x)));
    NetFloat64 flt = x;
    writer.appendBlock((char*)&flt,sizeof(flt));
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

    bool needQuote = false;
    for (unsigned int i=0; i<x.length(); i++) {
        char ch = x[i];
        if ((ch<'a'||ch>'z')&&
            (ch<'A'||ch>'Z')&&
            ch!='_') {
            if ((ch>='0'&&ch<='9')||ch=='.'||ch=='-') {
                if (i==0) {
                    needQuote = true;
                    break;
                }
            } else {
                needQuote = true;
                break;
            }
        }
    }
    if (x.length()==0) {
        needQuote = true;
    }

    if (!needQuote) {
        return x;
    }

    result += "\"";
    for (unsigned int j=0; j<x.length(); j++) {
        char ch = x[j];
        if (ch=='\n') {
            result += '\\';
            result += 'n';
        } else if (ch=='\r') {
            result += '\\';
            result += 'r';
        } else {
            if (ch=='\\'||ch=='\"') {
                result += '\\';
            }
            result += ch;
        }
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
                    if (back) {
                        if (ch=='n') {
                            x += '\n';
                        } else if (ch=='r') {
                            x += '\r';
                        } else {
                            x += ch;
                        }
                    } else {
                        x += ch;
                    }
                    back = false;
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


////////////////////////////////////////////////////////////////////////////
// StoreBlob


String StoreBlob::toStringFlex() const {
    String result = "";
    for (unsigned int i=0; i<x.length(); i++) {
        if (i>0) {
            result += " ";
        }
        unsigned char *src = (unsigned char *)(&x[i]);
        fflush(stdout);
        result += NetType::toString(*src);
    }
    return result;
}

String StoreBlob::toStringNested() const {
    return String("{") + toStringFlex() + "}";
}

void StoreBlob::fromString(const String& src) {
    Bottle bot(src.c_str());
    String buf((size_t)(bot.size()));
    for (int i=0; i<bot.size(); i++) {
        buf[i] = (char)((unsigned char)(bot.get(i).asInt()));
    }
    x.set(buf.c_str(),bot.size(),1);
}

void StoreBlob::fromStringNested(const String& src) {
    if (src.length()>0) {
        if (src[0]=='{') {
            // ignore first { and last }
            String buf = src.substr(1,src.length()-2);
            fromString(buf.c_str());
        }
    }
}


bool StoreBlob::read(ConnectionReader& reader) {
    int len = reader.expectInt();
    String buf((size_t)len);
    reader.expectBlock((const char *)buf.c_str(),len);
    x.set(buf.c_str(),(size_t)len,1);
    return true;
}

bool StoreBlob::write(ConnectionWriter& writer) {
    writer.appendInt(x.length());
    writer.appendBlock(x.c_str(),x.length());
    return true;
}



////////////////////////////////////////////////////////////////////////////
// StoreList




String StoreList::toStringFlex() const {
    return String(content.toString().c_str());
}

String StoreList::toStringNested() const {
    return String("(") + content.toString().c_str() + ")";
}

void StoreList::fromString(const String& src) {
    content.fromString(src.c_str());
}

void StoreList::fromStringNested(const String& src) {
    if (src.length()>0) {
        if (src[0]=='(') {
            // ignore first ( and last )
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

template <class T>
int subCoder(T& content) {
    int c = -1;
    bool ok = false;
    for (int i=0; i<content.size(); i++) {
        int sc = content.get(i).getCode();
        if (c==-1) {
            c = sc;
            ok = true;
        }
        if (sc!=c) {
            ok = false;
        }
    }
    // just optimize primitive types
    if ((c&GROUP_MASK)!=0) {
        ok = false;
    }
    c = ok?c:0;
    content.specialize(c);
    return c;
}

int StoreList::subCode() const {
    Bottle *op = (Bottle*)(&content);
    return subCoder(*op);
}

int BottleImpl::subCode() {
    return subCoder(*this);
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
        return content[index]->isList();
    }
    return false;
}



Storable& BottleImpl::get(int index) const {
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


void BottleImpl::copyRange(const BottleImpl& alt, int first, int len) {
    const BottleImpl *src = &alt;
    BottleImpl tmp;
    if (&alt == this) {
        tmp.fromString(toString());
        src = &tmp;
    }

    clear();
    if (len==-1) { len = src->size(); }
    int last = first + len - 1;
    int top = src->size()-1;
    if (first<0) { first = 0; }
    if (last<0) { last = 0; }
    if (first>top) { first = top; }
    if (last>top) { last = top; }

    for (int i=first; i<=last; i++) {
        add(src->get(i).cloneStorable());
    }
}



Value& Storable::find(const char *txt) {
    return BottleImpl::getNull();
}

Bottle& Storable::findGroup(const char *txt) {
    return Bottle::getNullBottle();
}

bool Storable::check(const char *key) {
    Bottle& val = findGroup(key);
    return !val.isNull();
}



bool Storable::operator == (const Value& alt) const {
    return String(toString().c_str()) == alt.toString().c_str();
}

