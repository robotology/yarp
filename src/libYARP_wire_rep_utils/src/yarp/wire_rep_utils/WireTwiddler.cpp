/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "WireTwiddler.h"

#include <vector>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Log.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/Os.h>
#include <yarp/sig/Image.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::wire_rep_utils;

#define dbg_flag 0
#define dbg_printf if (dbg_flag) printf

WireTwiddlerWriter::~WireTwiddlerWriter() = default;


int WireTwiddler::configure(Bottle& desc, int offset, bool& ignored,
                            const std::string& vtag) {
    int start = offset;
    // example: list 4 int32 * float64 * vector int32 * vector int32 3 *
    bool is_vector = false;
    bool is_list = false;
    bool ignore = false;
    bool saving = false;
    bool loading = false;
    bool computing = false;
    std::string kind = desc.get(offset).asString();
    std::string var;
    offset++;
    if (kind=="---") {
        offset = desc.size();
        return offset;
    }
    if (kind=="skip"||kind=="compute") {
        if (kind=="compute") computing = true;
        ignore = true;
        var = kind = desc.get(offset).asString();
        offset++;
    }
    if (kind.length()>0 && (kind[0]=='>'||kind[0]=='<')) {
        saving = (kind[0]=='>');
        loading = (kind[0]=='<');
        ignore = saving;
        var = kind.substr(1,kind.length());
        kind = desc.get(offset).asString();
        offset++;
    }

    is_vector = (kind=="vector");
    is_list = (kind=="list");
    if (kind=="item_vector") {
        is_vector = true;
        is_list = true;
    }
    if (is_vector) {
        kind = desc.get(offset).asString();
        offset++;
    }
    int len = 1;
    bool data = false;
    if (computing) data = true;
    if (is_vector||is_list) {
        Value v = desc.get(offset);
        offset++;
        if (v.isInt32()) {
            len = v.asInt32();
        } else {
            if (v.asString()!="*") {
                fprintf(stderr,"Does not look like length: %s\n",
                        v.toString().c_str());
            }
            len = -1;
            if (is_list) {
                fprintf(stderr,"List should have fixed length\n");
                len = 0;
            }
            data = true;
        }
    }
    if (!is_list) {
        if (!data) {
            Value v = desc.get(offset);
            offset++;
            if (v.asString()!="*") {
                fprintf(stderr,"Does not look like data: %s\n",
                        v.toString().c_str());
            }
            data = true;
        }
    }

    int tag = 0;
    int unit_length = 0;
    int wire_unit_length = -1;
    bool item = false;
    if (kind=="item") {
        kind = vtag;
        item = true;
    }
    if (kind=="int8"||kind=="uint8"||kind=="bool") {
        tag = BOTTLE_TAG_INT8;
        unit_length = 1;
    } else if (kind=="int16"||kind=="uint16") {
        tag = BOTTLE_TAG_INT16;
        unit_length = 2;
    } else if (kind=="int32"||kind=="uint32") {
        tag = BOTTLE_TAG_INT32;
        unit_length = 4;
    } else if (kind=="int64"||kind=="uint64") {
        tag = BOTTLE_TAG_INT64;
        unit_length = 8;
    } else if (kind=="float32") {
        tag = BOTTLE_TAG_FLOAT32;
        unit_length = 4;
    } else if (kind=="float64") {
        tag = BOTTLE_TAG_FLOAT64;
        unit_length = 8;
    } else if (kind=="vocab") {
        tag = BOTTLE_TAG_VOCAB32;
        unit_length = 4;
    } else if (kind=="string") {
        tag = BOTTLE_TAG_STRING;
        unit_length = -1;
        //len = -1;
    } else if (kind=="blob") {
        tag = BOTTLE_TAG_BLOB;
        unit_length = -1;
        //len = -1;
    } else if (kind=="list"||kind=="vector"||computing) {
        //pass
    } else {
        fprintf(stderr,"%s does not know about %s\n", __FILE__, kind.c_str());
        std::exit(1);
    }

    dbg_printf("Type %s (%s) len %d unit %d %s\n",
               kind.c_str(),
               is_list?"LIST":(is_vector?"VECTOR":"PRIMITIVE"), len,
               unit_length,
               ignore?"SKIP":"");

    if (!ignore) {
        if (is_vector) {
            buffer.push_back(BOTTLE_TAG_LIST+tag);
            if (len!=-1) {
                buffer.push_back(len);
            }
        } else if (is_list) {
            buffer.push_back(BOTTLE_TAG_LIST);
            buffer.push_back(len);
        } else {
            if (!item) buffer.push_back(tag);
        }
    }

    if (data) {
        WireTwiddlerGap gap;
        if (!ignore) {
            gap.buffer_start = buffer_start;
            gap.buffer_length = (int)buffer.size()-buffer_start;
            buffer_start = (int)buffer.size();
        } else {
            gap.buffer_start = 0;
            gap.buffer_length = 0;
        }
        gap.unit_length = unit_length;
        gap.wire_unit_length = (wire_unit_length!=-1)?wire_unit_length:unit_length;
        gap.length = len;
        gap.ignore_external = ignore;
        gap.save_external = saving;
        gap.load_external = loading;
        gap.computing = computing;
        gap.var_name = var;
        Bottle tmp;
        tmp.copy(desc,start,offset-start-1);
        gap.origin = tmp.toString();
        gap.flavor = tag;
        gaps.push_back(gap);
    }
    ignored = ignore;

    if (is_list) {
        int i=0;
        while (i<len) {
            bool ign = false;
            offset = configure(desc,offset,ign,kind);
            if (!ign) i++;
        }
    }
    return offset;
}

bool WireTwiddler::configure(const char *txt, const char *prompt) {
    this->prompt = prompt;
    clear();
    std::string str(txt);
    char *cstr = (char *)str.c_str();
    for (size_t i=0; i<str.length(); i++) {
        if (cstr[i]==',') {
            cstr[i] = ' ';
        }
    }

    if (str.find("list") == std::string::npos &&
        str.find("vector") == std::string::npos) {
        str += " list 0";
    }

    dbg_printf("Configure as %s\n", str.c_str());

    Bottle desc(str);

    buffer_start = 0;
    buffer.clear();
    size_t at = 0;
    size_t next = 0;
    do {
        bool ign = false;
        at = next;
        dbg_printf("Configuring, length %zu, at %zu\n", desc.size(), at);
        next = configure(desc,at,ign,"");
    } while (next>at&&next<desc.size());
    if (buffer_start!=(int)buffer.size()) {
        WireTwiddlerGap gap;
        gap.buffer_start = buffer_start;
        gap.buffer_length = (int)buffer.size()-buffer_start;
        buffer_start = (int)buffer.size();
        gaps.push_back(gap);
    }
    dbg_printf("buffer has %zu items\n", buffer.size());
    dbg_printf("gaps has %zu items\n", gaps.size());
    for (auto& gap : gaps) {
        if (gap.buffer_length!=0) {
            gap.byte_start = (char *) (&buffer[gap.buffer_start]);
            gap.byte_length = gap.buffer_length*4;
        } else {
            gap.byte_start = nullptr;
            gap.byte_length = 0;
        }
    }
    if (dbg_flag) show();
    return at == desc.size();
}

std::string nameThatCode(int code) {
    switch (code) {
    case BOTTLE_TAG_INT8:
        return "int8";
        break;
    case BOTTLE_TAG_INT16:
        return "int16";
        break;
    case BOTTLE_TAG_INT32:
        return "int32";
        break;
    case BOTTLE_TAG_INT64:
        return "int64";
        break;
    case BOTTLE_TAG_FLOAT32:
        return "float32";
        break;
    case BOTTLE_TAG_FLOAT64:
        return "float64";
        break;
    case BOTTLE_TAG_VOCAB32:
        return "vocab";
        break;
    case BOTTLE_TAG_STRING:
        return "string";
        break;
    case BOTTLE_TAG_LIST:
        return "list";
        break;
    case BOTTLE_TAG_BLOB:
        return "blob";
        break;
    }
    return "unsupported";
}

std::string WireTwiddler::fromTemplate(const yarp::os::Bottle& msg) {
    string result;

    // assume we want to remove any meta-information

    int len = msg.size();

    int code = -1;
    for (int i=0; i<len; i++) {
        Value&v = msg.get(i);
        int icode = v.getCode();
        if (i==0) code = icode;
        if (icode!=code) code = -1;
    }
    string codeName = nameThatCode(code);
    if (code == -1) {
        result += "list ";
    } else {
        result += "vector ";
        result += codeName;
        result += " ";
    }
    result += yarp::conf::numeric::to_string(len);
    result += " ";
    for (int i=0; i<len; i++) {
        Value&v = msg.get(i);
        if (!v.isList()) {
            if (code == -1) {
                result += nameThatCode(v.getCode());
                result += " ";
            }
            result += "* ";
        } else {
            result += fromTemplate(*v.asList());
        }
    }
    return result;
}


void WireTwiddler::show() {
    for (size_t i=0; i<gaps.size(); i++) {
        WireTwiddlerGap& gap = gaps[i];
        printf("Block #%zu (%s)\n", i, gap.getOrigin().c_str());
        if (gap.buffer_length!=0) {
            printf("  Buffer from %d to %d\n", gap.buffer_start,
                   gap.buffer_start+gap.buffer_length-1);
        }
        if (gap.ignore_external) {
            printf("  External data will be ignored\n");
        } else {
            if (gap.unit_length!=0) {
                printf("  Expect %d x %d\n", gap.length, gap.unit_length);
            }
        }
    }
}

std::string WireTwiddler::toString() const {
    std::string result;
    for (const auto& gap : gaps) {
        std::string item;
        item += gap.origin;
        //if (gap.buffer_length!=0) {
        //  char buf[1024];
            //sprintf(buf," (%d to %d)", gap.buffer_start,
            //      gap.buffer_start+gap.buffer_length-1);
            //item += buf;
        //}
        if (gap.unit_length!=0) {
            char buf[1024];
            if (gap.length==-1 && gap.unit_length==-1) {
                sprintf(buf," [4-byte length] [<length> instances of 4-byte-length bytes followed by specified number of bytes]");
            } else if (gap.length==-1) {
                sprintf(buf," [4-byte length] [<length>*%d bytes]", gap.unit_length);
            } else if (gap.length==1) {
                sprintf(buf," [%d bytes]", gap.unit_length);
            } else {
                sprintf(buf," [%d*%d bytes]", gap.length, gap.unit_length);
            }
            item += buf;
        }
        if (result!="") result += " ";
        result += item;
    }
    return result;
}


bool WireTwiddler::read(Bottle& bot, const Bytes& data) {
    StringInputStream sis;
    sis.add(data);
    WireTwiddlerReader twiddled_input(sis,*this);
    return ConnectionReader::readFromStream(bot,twiddled_input);
}


bool WireTwiddler::write(yarp::os::Bottle& bot,
                         yarp::os::ManagedBytes& data) {
    StringOutputStream sos;
    if (!writer) {
        writer = ConnectionWriter::createBufferedConnectionWriter();
    }
    if (!writer) {
        return false;
    }
    SizedWriter *buf = writer->getBuffer();
    if (!buf) {
        return false;
    }
    buf->clear();
    bot.write(*writer);
    WireTwiddlerWriter twiddled_output(*buf,*this);
    twiddled_output.write(sos);
    std::string result = sos.toString();
    data = ManagedBytes(Bytes((char*)result.c_str(),result.length()),false);
    data.copy();
    return true;
}


void WireTwiddlerReader::compute(const WireTwiddlerGap& gap) {
    if (gap.var_name == "image_params") {
        int w = prop.find("width").asInt32();
        int h = prop.find("height").asInt32();
        int step = prop.find("step").asInt32();
        bool bigendian = prop.find("is_bigendian").asInt32()==1;
        if (bigendian) {
            fprintf(stderr,"Sorry, cannot handle bigendian images yet.\n");
            std::exit(1);
        }
        std::string encoding = prop.find("encoding").asString();
        int bpp = 1;
        int translated_encoding = 0;
        switch (Vocab32::encode(encoding)) {
        case yarp::os::createVocab32('b','g','r','8'):
            bpp = 3;
            translated_encoding = VOCAB_PIXEL_BGR;
            break;
        case yarp::os::createVocab32('r','g','b','8'):
            bpp = 3;
            translated_encoding = VOCAB_PIXEL_RGB;
            break;
        case yarp::os::createVocab32('3','2','F','C'):
            yAssert(encoding=="32FC1");
            bpp = 4;
            translated_encoding = VOCAB_PIXEL_MONO_FLOAT;
            break;
        case yarp::os::createVocab32('1','6','U','C'):
            yAssert(encoding=="16UC1");
            bpp = 2;
            translated_encoding = VOCAB_PIXEL_MONO16;
            break;
        case yarp::os::createVocab32('m','o','n','o'):
            yAssert(encoding=="mono8"||encoding=="mono16");
            if (encoding == "mono8") {
                bpp = 1;
                translated_encoding = VOCAB_PIXEL_MONO;
            } else {
                bpp = 2;
                translated_encoding = VOCAB_PIXEL_MONO16;
            }
            break;
        case yarp::os::createVocab32('b', 'a', 'y', 'e'):
            bpp = 1;
            translated_encoding = VOCAB_PIXEL_MONO;
            if (encoding == "bayer_grbg8")
                translated_encoding = VOCAB_PIXEL_ENCODING_BAYER_GRBG8;
            else
            {
                fprintf(stderr, "Warning automatic debayering not yet supported, keeping raw format.\n");
            }

            break;
        default:
            fprintf(stderr, "Sorry, cannot handle [%s] images yet.\n",
                encoding.c_str());
            std::exit(1);
            break;
        }
        int quantum = 1;
        if (step!=w*bpp) {
            bool ok = false;
            while (quantum<=256) {
                quantum *= 2;
                if (((w*bpp+quantum)/quantum)*quantum == step) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                quantum = step;
            }
        }
        int img_size = step*h;
        prop.put("depth",3);
        prop.put("img_size",img_size);
        prop.put("quantum",quantum);
        prop.put("translated_encoding",translated_encoding);
    }
}

yarp::conf::ssize_t WireTwiddlerReader::read(Bytes& b) {
    dbg_printf("Want %zu bytes\n", b.length());
    if (index==-1) {
        dbg_printf("WireTwidderReader::read getting started\n");
    }
    int ct = twiddler.getGapCount();
    if (ct<=0) {
        fprintf(stderr,"WireTwidderReader, nothing available\n");
        return -1;
    }
    if (index==-1) {
        index = 0;
        sent = 0;
        consumed = 0;
    }
    if (index>=ct) {
        fprintf(stderr,"WireTwidderReader, nothing left\n");
        return -1;
    }
    bool more = false;
    do {
        const WireTwiddlerGap& gap = twiddler.getGap(index);
        if (gap.computing) {
            compute(gap);
        }
        dbg_printf("*** index %d sent %d consumed %d / len %d unit %d/%d\n", index, sent, consumed, gap.length, gap.unit_length, gap.wire_unit_length);
        char *byte_start = gap.getStart();
        int byte_length = gap.getLength();
        if (byte_start != nullptr) {
            byte_start += consumed;
            byte_length -= consumed;
        }
        if (byte_start != nullptr && byte_length > 0) {
            int len = b.length();
            if (len>byte_length) {
                len = byte_length;
            }
            memcpy(b.get(),byte_start,len);
            sent += len;
            consumed += len;
            auto* nn = reinterpret_cast<NetInt32 *> (b.get());
            dbg_printf("WireTwidderReader sending %d boilerplate bytes:\n",len);
            dbg_printf("   [[[%d]]]\n", (int)(*nn));
            return len;
        }
        if ((gap.length==-1||gap.unit_length==-1) && override_length==-1) {
            dbg_printf("LOOKING TO EXTERNAL\n");
            Bytes bytes(reinterpret_cast<char*>(&lengthBuffer), sizeof(NetInt32));
            int r = is.readFull(bytes);
            if (r!=sizeof(NetInt32)) return -1;
            dbg_printf("Read length %d\n", lengthBuffer);
            pending_length = sizeof(lengthBuffer);
            if (gap.length==-1) {
                dbg_printf("Expect 4 + %d x %d\n", lengthBuffer, gap.unit_length);
                if (gap.unit_length<0) {
                    pending_strings = lengthBuffer;
                    pending_string_length = 0;
                    pending_string_data = 0;
                    override_length = 0;
                } else {
                    override_length = lengthBuffer*gap.unit_length;
                }
            } else {
                override_length = lengthBuffer;
                dbg_printf("Expect 1 x %d\n", lengthBuffer);
            }
        }
        if (gap.shouldIgnoreExternal()) {
            pending_length = 0;
        }
        if (pending_length) {
            int len = b.length();
            if (len>pending_length) {
                len = pending_length;
            }
            Bytes b2(b.get(),len);
            memcpy(b.get(),
                   (char*)(&lengthBuffer)+sizeof(lengthBuffer)-pending_length,
                   len);
            pending_length -= len;
            dbg_printf("WireTwidderReader sending %d length bytes:\n",len);
            dbg_printf("   (((%d)))\n", lengthBuffer);
            return len;
        }
        while (pending_strings) {
            dbg_printf("### %d pending strings\n", pending_strings);
            if (pending_string_length==0&&pending_string_data==0) {
                dbg_printf("Checking string length\n");
                Bytes bytes(reinterpret_cast<char*>(&lengthBuffer), sizeof(NetInt32));
                int r = is.readFull(bytes);
                if (r!=sizeof(NetInt32)) return -1;
                dbg_printf("Read length %d\n", lengthBuffer);
                pending_string_length = sizeof(lengthBuffer);
                pending_string_data = lengthBuffer;
            }
            if (pending_string_length) {
                int len = b.length();
                if (len>pending_string_length) {
                    len = pending_string_length;
                }
                Bytes b2(b.get(),len);
                memcpy(b.get(),
                       (char*)(&lengthBuffer)+sizeof(lengthBuffer)-pending_string_length,
                       len);
                pending_string_length -= len;
                dbg_printf("WireTwidderReader sending %d string length bytes:\n",len);
                dbg_printf("   (((%d)))\n", lengthBuffer);
                if (pending_string_length==0&&pending_string_data==0) { pending_strings--; }
                return len;
            }
            if (pending_string_data) {
                int len = b.length();
                if (len>pending_string_data) {
                    len = pending_string_data;
                }
                Bytes b2(b.get(),len);
                int r = is.readFull(b2);
                if (r<0) {
                    fprintf(stderr,"No string payload bytes available\n");
                    return r;
                }
                pending_string_data -= r;
                dbg_printf("WireTwidderReader sending %d string payload bytes\n",r);
                if (pending_string_data==0) { pending_strings--; }
                return r;
            }
        }
        int extern_length = gap.length * gap.unit_length;
        if (gap.unit_length<0||gap.length<0) extern_length = override_length;
        dbg_printf("extern_length %d\n", extern_length);

        if (extern_length>sent-consumed) {
            int len = b.length();
            if (len>extern_length) {
                len = extern_length;
            }
            Bytes b2(b.get(),len);
            int r = 0;
            if (!gap.shouldIgnoreExternal()) {
                r = readMapped(is,b2,gap);
                auto* nn = reinterpret_cast<NetInt32 *> (b.get());
                dbg_printf("WireTwidderReader sending %d payload bytes:\n",r);
                dbg_printf("   [[[%d]]]\n", (int)(*nn));
                if (r>0) {
                    sent += r;
                }
                if (r<0) {
                    fprintf(stderr,"No payload bytes available\n");
                    return r;
                }
                return r;
            } else {
                int len2 = extern_length;
                if (gap.wire_unit_length>=0 && gap.wire_unit_length!=len2) {
                    len2 = gap.wire_unit_length;
                }
                dump.allocateOnNeed(len2,len2);
                Bytes b3(dump.get(),len2);
                r = is.readFull(b3);
                auto* nn = reinterpret_cast<NetInt32 *> (dump.get());
                if (gap.save_external) {
                    if (override_length>=0) {
                        prop.put(gap.var_name,
                                 std::string((char *)(dump.get()),
                                             len2));
                        dbg_printf("Saved %s: %s\n",
                               gap.var_name.c_str(),
                               prop.find(gap.var_name).asString().c_str());
                    } else if (len2>=4) {
                        prop.put(gap.var_name,(int)(*nn));
                        dbg_printf("Saved %s: is %d\n",
                               gap.var_name.c_str(),
                               prop.find(gap.var_name).asInt32());
                    }
                }
                dbg_printf("WireTwidderReader sending %d payload bytes:\n",r);
                dbg_printf("   [[[%d]]]\n", (int)(*nn));
                dbg_printf("   (ignoring %d payload bytes)\n",r);
                if (r>0) {
                    sent += r;
                }
            }
        }
        more = false;
        if (index<ct-1) {
            index++;
            consumed = 0;
            sent = 0;
            override_length = -1;
            pending_length = 0;
            pending_strings = 0;
            more = true;
        }
    } while (more);
    fprintf(stderr,"WireTwidderReader shrugging\n");
    return -1;
}


bool WireTwiddlerWriter::update() {
    scratchOffset = 0;
    errorState = false;
    activeGap = nullptr;
    codeExpected = 0;
    codeReceived = 0;
    srcs.clear();

    lengthBytes = Bytes((char*)(&lengthBuffer),sizeof(yarp::os::NetInt32));
    offset = 0;
    blockPtr = nullptr;
    blockLen = 0;
    block = parent->headerLength();
    lastBlock = parent->length()-block-1;
    activeEmit = nullptr;
    activeEmitLength = 0;
    activeEmitOffset = -1;

    dbg_printf("Parent headers %zu blocks %zu\n", parent->headerLength(), parent->length());

    for (int i=0; i<twiddler->getGapCount(); i++) {
        if (errorState) return false;
        std::string item;
        const WireTwiddlerGap& gap = twiddler->getGap(i);
        if (gap.buffer_length!=0) {
            dbg_printf("Skip %d bytes\n", gap.byte_length);
            skip(gap.byte_start,gap.byte_length);
            if (gap.unit_length!=0) {
                dbg_printf("Length %d unit_length %d\n", gap.length,
                           gap.unit_length);
                if (gap.length==-1 && gap.unit_length==-1) {
                    dbg_printf("Pass [4-byte length] [<length> instances of 4-byte-length bytes followed by specified number of bytes]\n");
                    readLengthAndPass(-1);
                } else if (gap.length==1&&gap.unit_length==1) {
                    dbg_printf("Pass [%d bytes]\n", gap.unit_length);
                    pass(gap.unit_length);
                } else if (gap.length==1&&gap.unit_length==-1) {
                    dbg_printf("Pass [4-byte length] [<length> bytes]\n");
                    readLengthAndPass(1);
                } else if (gap.length==-1) {
                    dbg_printf("Pass [4-byte length] [<length>*%d bytes]\n", gap.unit_length);
                    readLengthAndPass(gap.unit_length,&gap);
                } else {
                    dbg_printf("Pass [%d*%d bytes]\n", gap.length, gap.unit_length);
                    if (gap.unit_length!=gap.wire_unit_length) {
                        dbg_printf("Need to tweak length (not correct for neg numbers yet)...\n");
                        for (int i=0; i<gap.length; i++) {
                            if (errorState) return false;
                            transform(gap);
                        }
                    } else {
                        pass(gap.length*gap.unit_length);
                    }
                }
            }
        }
    }
    emit(nullptr, 0);
    dbg_printf("%zu write blocks\n", srcs.size());
    if (dbg_flag) {
        for (size_t i=0; i<srcs.size(); i++) {
            dbg_printf("  write block %zu: len %d offset %d ptr %p\n", i, srcs[i].len, srcs[i].offset, srcs[i].src);
        }
    }
    return !errorState;
}


bool WireTwiddlerWriter::skip(const char *start, int len) {
    activeCheck = start;
    return advance(len,false,false,true);
}

bool WireTwiddlerWriter::pass(int len) {
    return advance(len,true);
}

bool WireTwiddlerWriter::pad(size_t len) {
    if (zeros.length()<len) {
        zeros.allocate(len);
        for (size_t i=0; i<len; i++) {
            zeros.get()[i] = '\0';
        }
    }
    return emit(zeros.get(),len);
}

bool WireTwiddlerWriter::readLengthAndPass(int unitLength,
                                           const WireTwiddlerGap *gap) {
    int len = readLength();
    if (len==0) return false;
    if (unitLength!=-1) {
        if (gap == nullptr || gap->wire_unit_length == unitLength) {
            advance(unitLength*len,true);
        } else {
            for (int i=0; i<len; i++) {
                if (!transform(*gap)) return false;
            }
        }
    } else {
        for (int i=0; i<len; i++) {
            bool ok = readLengthAndPass(1,gap);
            if (!ok) return false;
        }
    }
    return true;
}

void WireTwiddlerWriter::showBrokenExpectation(const yarp::os::NetInt32& expected,
                                              const yarp::os::NetInt32& received,
                                              int evidence) {
    if (!errorState) {
        yError("Structure of message is unexpected (expected %s)", twiddler->getPrompt().c_str());
        if (evidence>=4) {
            if (expected!=received) {
                yError("Expected '%s', got '%s'\n",
                       Bottle::describeBottleCode(expected).c_str(),
                       Bottle::describeBottleCode(received).c_str());
            }
        }
    }
    errorState = true;
}


int WireTwiddlerWriter::readLength() {
    advance(4,true,true);
    if (accumOffset==4) {
        if (codeExpected!=codeReceived) {
            if (lengthBuffer!=0) {
                showBrokenExpectation(codeExpected,codeReceived,4);
            }
            codeExpected = codeReceived = 0;
        }
        return lengthBuffer;
    }
    return 0;
}

bool WireTwiddlerWriter::advance(int length, bool shouldEmit,
                                 bool shouldAccum, bool shouldCheck) {
    accumOffset = 0;
    if (length==0) return true;
    if (length<0) return false;
    while (length>0) {
        if (blockPtr == nullptr) {
            blockPtr = parent->data(block);
            blockLen = parent->length(block);
            dbg_printf("  block %d is at %ld, length %d\n",block,
                       (long int)blockPtr, blockLen);
            offset = 0;
        }
        int rem = blockLen-offset;
        if (rem==0) {
            block++;
            blockPtr = nullptr;
            if (block>lastBlock) {
                return false;
            }
            dbg_printf("  moved on to block %d\n",block);
            continue;
        }
        if (rem>length) rem = length;
        if (shouldCheck) {
            dbg_printf("Type check against %ld\n", (long int)activeCheck);
            int result = memcmp(activeCheck,blockPtr+offset,rem);
            activeCheck += rem;
            if (result!=0) {
                NetInt32 t1 = 0;
                NetInt32 t2 = 0;
                if (rem>=4) {
                    t1 = *(reinterpret_cast<const NetInt32*>(blockPtr+offset));
                    t2 = *(reinterpret_cast<const NetInt32*>(activeCheck-rem));
                    if (t1!=t2 && (t1&BOTTLE_TAG_LIST) && (t2&BOTTLE_TAG_LIST)) {
                        // delay checking of codes until list length is read
                        // since list may be empty
                        codeExpected = t2;
                        codeReceived = t1;
                        result = 0;
                    }
                }
                if (result!=0) {
                    dbg_printf("Type check failed! >>>\n");
                    showBrokenExpectation(t2,t1,rem);
                    return false;
                }
            }
        }
        if (shouldEmit) {
            emit(blockPtr+offset, rem);
        }
        if (shouldAccum) {
            if (accumOffset+rem>4) {
                fprintf(stderr,"ACCUMULATION GONE WRONG %d %d\n",
                        accumOffset, rem);
                ::exit(1);
            }
            memcpy(lengthBytes.get()+accumOffset,blockPtr+offset,rem);
            accumOffset += rem;
        }
        offset += rem;
        length -= rem;
    }
    return true;
}

bool WireTwiddlerWriter::emit(const char *src, int len) {
    int noffset = -1;
    if (src != nullptr) {
        if (activeGap) {
            const WireTwiddlerGap& gap = *activeGap;
            if (gap.wire_unit_length==4 && gap.unit_length==8 &&
                len == 8 &&
                gap.flavor == BOTTLE_TAG_FLOAT64) {
                const auto* x = reinterpret_cast<const NetFloat64*>(src);
                if (scratchOffset+4>scratch.length()) {
                    scratch.allocateOnNeed(scratchOffset+4,scratchOffset+4);
                }
                auto* y = reinterpret_cast<NetFloat32*>(scratch.get()+scratchOffset);
                *y = (NetFloat32) *x;
                src = nullptr;
                noffset = scratchOffset;
                len = 4;
                scratchOffset += 4;
            } else {
                fprintf(stderr,"WireTwidder::emit needs to be extended to deal with this case\n");
                ::exit(1);
            }
            activeGap = nullptr;
        }
    }
    dbg_printf("  cache %ld len %d offset %d /// activeEmit %ld %d %d\n", (long int)src, len, noffset, (long int) activeEmit, activeEmitLength, activeEmitOffset);
    if (activeEmit != nullptr || activeEmitOffset >= 0) {
        bool push = false;
        if (activeEmitOffset>=0 && noffset<0) {
            push = true;
        } else if (activeEmitOffset<0 && noffset>=0) {
            push = true;
        } else if (noffset==-1 && activeEmit+activeEmitLength!=src) {
            push = true;
        }
        if (push) {
            dbg_printf("  ** emit %ld len %d offset %d\n", (long int)activeEmit,
                       activeEmitLength, activeEmitOffset);
            srcs.emplace_back((char*)activeEmit,activeEmitLength,activeEmitOffset);
            activeEmit = nullptr;
            activeEmitLength = 0;
            activeEmitOffset = -1;
        } else {
            activeEmitLength += len;
            dbg_printf("  ** extend %ld len %d offset %d\n", (long int)activeEmit,
                       activeEmitLength, activeEmitOffset);
            return true;
        }
    }
    if (src != nullptr || noffset >= 0) {
        activeEmit = src;
        activeEmitLength = len;
        activeEmitOffset = noffset;
    }
    return true;
}

bool WireTwiddlerWriter::transform(const WireTwiddlerGap& gap) {
    // So far, very crude, does not cover all cases
    if (gap.wire_unit_length>gap.unit_length) {
        pass(gap.unit_length);
        pad(gap.wire_unit_length-gap.unit_length);
        return true;
    }
    if (gap.wire_unit_length==4 && gap.unit_length==8 &&
        gap.flavor == BOTTLE_TAG_FLOAT64) {
        activeGap = &gap;
        pass(gap.unit_length);
        return true;
    }
    fprintf(stderr,"WireTwidder::transform needs to be extended to deal with this case\n");
    ::exit(1);
    return false;
}

yarp::conf::ssize_t WireTwiddlerReader::readMapped(yarp::os::InputStream& is,
                                                   yarp::os::Bytes& b,
                                                   const WireTwiddlerGap& gap) {
    if (gap.load_external) {
        int v = 0;
        if (gap.var_name[0]=='=') {
            Bottle bot;
            bot.fromString(gap.var_name.substr(1,gap.var_name.length()));
            v = bot.get(0).asInt32();
        } else {
            v = prop.find(gap.var_name).asInt32();
        }
        dbg_printf("Read %s: %d\n", gap.var_name.c_str(), v);
        for (size_t i=0; i<b.length(); i++) {
            b.get()[i] = 0;
        }
        auto* nn = reinterpret_cast<NetInt32 *> (b.get());
        if (b.length()>=4) {
            *nn = v;
        }
        return gap.unit_length;
    }
    if (gap.wire_unit_length==gap.unit_length) {
        return is.read(b);
    }
    for (size_t i=0; i<b.length(); i++) {
        b.get()[i] = 0;
    }
    size_t len = gap.wire_unit_length;
    if (len>b.length()) {
        len = b.length();
    }
    Bytes b2(b.get(),len);
    yarp::conf::ssize_t r = is.readFull(b2);
    if (r==(yarp::conf::ssize_t)len) {
        if (gap.flavor==BOTTLE_TAG_FLOAT64) {
            if (gap.wire_unit_length==4 &&
                gap.unit_length==8) {
                NetFloat32 x = *(reinterpret_cast<NetFloat32 *> (b2.get()));
                auto* y = reinterpret_cast<NetFloat64 *> (b2.get());
                *y = x;
            }
        }
        int len2 = gap.wire_unit_length-b.length();
        if (len2>0) {
            dump.allocateOnNeed(len2,len2);
            Bytes b3(dump.get(),len2);
            is.readFull(b3);
        }
        return gap.unit_length;
    }
    return -1;
}



//void write(OutputStream& os) {
//}
