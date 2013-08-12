// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "WireTwiddler.h"

#include <vector>

#include <stdio.h>

#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/impl/StringOutputStream.h>
#include <yarp/os/impl/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;

#define dbg_flag 0
#define dbg_printf if (dbg_flag) printf

int WireTwiddler::configure(Bottle& desc, int offset, bool& ignored) {
    int start = offset;
    // example: list 4 int32 * float64 * vector int32 * vector int32 3 *
    bool is_vector = false;
    bool is_list = false;
    bool ignore = false;
    ConstString kind = desc.get(offset).asString();
    offset++;
    if (kind=="skip") {
        ignore = true;
        kind = desc.get(offset).asString();
        offset++;
    }

    is_vector = (kind=="vector");
    is_list = (kind=="list");
    if (is_vector) {
        kind = desc.get(offset).asString();
        offset++;
    }
    int len = 1;
    bool data = false;
    if (is_vector||is_list) {
        Value v = desc.get(offset);
        offset++;
        if (v.isInt()) {
            len = v.asInt();
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
    if (kind=="int32") {
        tag = BOTTLE_TAG_INT;
        unit_length = 4;
    } else if (kind=="float64") {
        tag = BOTTLE_TAG_DOUBLE;
        unit_length = 8;
    } else if (kind=="string") {
        tag = BOTTLE_TAG_STRING;
        unit_length = -1;
        //len = -1;
    }

    dbg_printf("Type %s (%s) len %d %s\n", 
               kind.c_str(),
               is_list?"LIST":(is_vector?"VECTOR":"PRIMITIVE"), len,
               ignore?"SKIP":"");

    if (!ignore) {
        if (is_list) {
            buffer.push_back(BOTTLE_TAG_LIST);
            buffer.push_back(len);
        } else if (is_vector) {
            buffer.push_back(BOTTLE_TAG_LIST+tag);
            if (len!=-1) {
                buffer.push_back(len);
            }
        } else {
            buffer.push_back(tag);
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
        gap.length = len;
        gap.ignore_external = ignore;
        Bottle tmp;
        tmp.copy(desc,start,offset-start-1);
        gap.origin = tmp.toString();
        gaps.push_back(gap);
    }
    ignored = ignore;

    if (is_list) {
        int i=0;
        while (i<len) {
            bool ign = false;
            offset = configure(desc,offset,ign);
            if (!ign) i++;
        }
    }
    return offset;
}

bool WireTwiddler::configure(const char *txt) {
    clear();
    ConstString str(txt);
    char *cstr = (char *)str.c_str();
    for (size_t i=0; i<str.length(); i++) {
        if (cstr[i]==',') {
            cstr[i] = ' ';
        }
    }

    Bottle desc(str.c_str());

    buffer_start = 0;
    buffer.clear();
    int at = 0;
    int next = 0;
    do {
        bool ign = false;
        at = next;
        dbg_printf("Configuring, length %d, at %d\n", desc.size(), at);
        next = configure(desc,at,ign);
    } while (next>at&&next<desc.size());
    if (buffer_start!=(int)buffer.size()) {
        WireTwiddlerGap gap;
        gap.buffer_start = buffer_start;
        gap.buffer_length = (int)buffer.size()-buffer_start;
        buffer_start = (int)buffer.size();
        gaps.push_back(gap);
    }
    dbg_printf("buffer has %d items\n", (int)buffer.size());
    dbg_printf("gaps has %d items\n", (int)gaps.size());
    for (int i=0; i<(int)gaps.size(); i++) {
        WireTwiddlerGap& gap = gaps[i];
        if (gap.buffer_length!=0) {
            gap.byte_start = (char *) (&buffer[gap.buffer_start]);
            gap.byte_length = gap.buffer_length*4;
        } else {
            gap.byte_start = NULL;
            gap.byte_length = 0;
        }
    }
    if (dbg_flag) show();
    return at == desc.size();
}

std::string nameThatCode(int code) {
    switch (code) {
    case BOTTLE_TAG_INT:
    case BOTTLE_TAG_VOCAB:
        return "int32";
        break;
    case BOTTLE_TAG_DOUBLE:
        return "float64";
        break;
    case BOTTLE_TAG_STRING:
        return "string";
        break;
    case BOTTLE_TAG_LIST:
        return "list";
        break;
    }
    return "unsupported";
}

std::string WireTwiddler::fromTemplate(const yarp::os::Bottle& msg) {
    string result = "";

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
    result += NetType::toString(len).c_str();
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
    for (int i=0; i<(int)gaps.size(); i++) {
        WireTwiddlerGap& gap = gaps[i];
        printf("Unit %d\n", i);
        if (gap.buffer_length!=0) {
            printf("  Buffer from %d to %d\n", gap.buffer_start, 
                   gap.buffer_start+gap.buffer_length-1);
        }
        if (gap.unit_length!=0) {
            printf("  Expect %d x %d\n", gap.length, gap.unit_length);
        }
        if (gap.ignore_external) {
            printf("  External data will be ignored\n");
        }
    }
}

yarp::os::ConstString WireTwiddler::toString() const {
    ConstString result = "";
    for (int i=0; i<(int)gaps.size(); i++) {
        ConstString item = "";
        const WireTwiddlerGap& gap = gaps[i];
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
    Route route;
    StreamConnectionReader reader2;
    reader2.reset(twiddled_input,NULL,route,0,false);
    return bot.read(reader2);
}


bool WireTwiddler::write(yarp::os::Bottle& bot, 
                         yarp::os::ManagedBytes& data) {
    StringOutputStream sos;
    BufferedConnectionWriter writer;
    bot.write(writer);
    WireTwiddlerWriter twiddled_output(writer,*this);
    twiddled_output.write(sos);
    String result = sos.toString();
    data = ManagedBytes(Bytes((char*)result.c_str(),result.length()),false);
    data.copy();
    return true;
}



YARP_SSIZE_T WireTwiddlerReader::read(const Bytes& b) {
    dbg_printf("Want %d bytes\n", (int)b.length());
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
        dbg_printf("*** index %d sent %d consumed %d / len %d unit %d\n", index, sent, consumed, gap.length, gap.unit_length);
        char *byte_start = gap.getStart();
        int byte_length = gap.getLength();
        if (byte_start!=NULL) {
            byte_start += consumed;
            byte_length -= consumed;
        }
        if (byte_start!=NULL && byte_length>0) {
            int len = b.length();
            if (len>byte_length) {
                len = byte_length;
            }
            memcpy(b.get(),byte_start,len);
            sent += len;
            consumed += len;
            NetInt32 *nn = (NetInt32 *)b.get();
            dbg_printf("[[[%d]]]\n", (int)(*nn));
            dbg_printf("WireTwidderReader sending %d boilerplate bytes\n",len);
            return len;
        }
        if ((gap.length==-1||gap.unit_length==-1) && override_length==-1) {
            dbg_printf("LOOKING TO EXTERNAL\n");
            int r = NetType::readFull(is,Bytes((char*)&lengthBuffer,
                                               sizeof(NetInt32)));
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
            dbg_printf("(((%d)))\n", lengthBuffer);
            dbg_printf("WireTwidderReader sending %d length bytes\n",len);
            return len;
        }
        while (pending_strings) {
            dbg_printf("### %d pending strings\n", pending_strings);
            if (pending_string_length==0&&pending_string_data==0) {
                dbg_printf("Checking string length\n");
                int r = NetType::readFull(is,Bytes((char*)&lengthBuffer,
                                                   sizeof(NetInt32)));
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
                dbg_printf("(((%d)))\n", lengthBuffer);
                dbg_printf("WireTwidderReader sending %d string length bytes\n",len);
                if (pending_string_length==0&&pending_string_data==0) { pending_strings--; }
                return len;
            }
            if (pending_string_data) {
                int len = b.length();
                if (len>pending_string_data) {
                    len = pending_string_data;
                }
                Bytes b2(b.get(),len);
                int r = is.read(b2);
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
                r = is.read(b2);
                NetInt32 *nn = (NetInt32 *)b.get();
                dbg_printf("[[[%d]]]\n", (int)(*nn));
                dbg_printf("WireTwidderReader sending %d payload bytes\n",r);
                if (r>0) {
                    sent += r;
                }
                if (r<0) {
                    fprintf(stderr,"No payload bytes available\n");
                    return r;
                }
                return r;
            } else {
                dump.allocateOnNeed(b2.length(),b2.length());
                r = NetType::readFull(is,dump.bytes());
                NetInt32 *nn = (NetInt32 *)dump.get();
                dbg_printf("[[[%d]]]\n", (int)(*nn));
                dbg_printf("WireTwidderReader sending %d payload bytes\n",r);
                dbg_printf("  (ignoring %d payload bytes)\n",r);
                if (r>0) {
                    sent += r;
                }
            }
        }
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
    errorState = false;
    srcs.clear();
    //int hdr = parent.length();
    //for (int i=0; i<parent.length(); i++) {
    //srcs.push_back(Bytes((char*)parent.data(i),parent.length(i)));
    //}

    lengthBytes = Bytes((char*)(&lengthBuffer),sizeof(yarp::os::NetInt32));
    offset = 0;
    blockPtr = NULL;
    blockLen = 0;
    block = parent->headerLength();
    lastBlock = parent->length()-block-1;
    activeEmit = NULL;
    activeEmitLength = 0;


    dbg_printf("Parent headers %d blocks %d\n", (int)parent->headerLength(), 
               (int)parent->length());

    for (int i=0; i<twiddler->getGapCount(); i++) {
        ConstString item = "";
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
                    dbg_printf("Pass [4-byte length] [<length>*%d bytes]", gap.unit_length);
                    readLengthAndPass(gap.unit_length);
                } else {
                    dbg_printf("Pass [%d*%d bytes]\n", gap.length, gap.unit_length);
                    pass(gap.length*gap.unit_length);
                }
            }
        }
    }
    emit(NULL,0);
    dbg_printf("%d write blocks\n", (int)srcs.size());
    return !errorState;
}


bool WireTwiddlerWriter::skip(const char *start, int len) {
    activeCheck = start;
    return advance(len,false,false,true);
}

bool WireTwiddlerWriter::pass(int len) {
    return advance(len,true);
}

bool WireTwiddlerWriter::readLengthAndPass(int unitLength) {
    int len = readLength();
    if (len==0) return false;
    if (unitLength!=-1) {
        advance(unitLength*len,true);
    } else {
        for (int i=0; i<len; i++) {
            bool ok = readLengthAndPass(1);
            if (!ok) return false;
        }
    }
    return true;
}

int WireTwiddlerWriter::readLength() {
    advance(4,true,true);
    if (accumOffset==4) return lengthBuffer;
    return 0;
}

bool WireTwiddlerWriter::advance(int length, bool shouldEmit, 
                                 bool shouldAccum, bool shouldCheck) {
    accumOffset = 0;
    if (length==0) return true;
    if (length<0) return false;
    while (length>0) {
        if (blockPtr==NULL) {
            blockPtr = parent->data(block);
            blockLen = parent->length(block);
            dbg_printf("  block %d is at %ld, length %d\n",block, 
                       (long int)blockPtr, blockLen);
            offset = 0;
        }
        int rem = blockLen-offset;
        if (rem==0) {
            block++;
            blockPtr = NULL;
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
                dbg_printf("Type check failed! >>>\n");
                for (int i=0; i<rem; i++) {
                    dbg_printf(" %03d <--> %03d\n", 
                            activeCheck[i], blockPtr[i+offset]);
                }
                YARP_SPRINTF0(Logger::get(),error,"Structure of message is unexpected");
                errorState = true;
                return false;
            }
        }
        if (shouldEmit) {
            dbg_printf("  emitting block %d with offset %d, length %d\n",
                       block, offset, rem);
            emit(blockPtr+offset, rem);
        } else {
            dbg_printf("  skipping block %d with offset %d, length %d\n",
                       block, offset, rem);
        }
        if (shouldAccum) {
            if (accumOffset+rem>4) {
                fprintf(stderr,"ACCUMULATION GONE WRONG %d %d\n",
                        accumOffset, rem);
                exit(1);
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
    if (src!=NULL) {
        dbg_printf("  cache %ld %d [%d]\n", (long int)src, len, *src);
    }
    if (activeEmit!=NULL) {
        dbg_printf("  activeEmit is currently %ld, len %d\n",
                   (long int)activeEmit, 
                   activeEmitLength);
        if (activeEmit+activeEmitLength!=src) {
            dbg_printf("  emit %ld %d [%d]\n", (long int)activeEmit, 
                       activeEmitLength, *activeEmit);
            srcs.push_back(Bytes((char*)activeEmit,activeEmitLength));
            activeEmit = NULL;
        } else {
            activeEmitLength += len;
            return true;
        }
    }
    if (src!=NULL) {
        activeEmit = src;
        activeEmitLength = len;
    }
    return true;
}


//void write(OutputStream& os) {
//}



