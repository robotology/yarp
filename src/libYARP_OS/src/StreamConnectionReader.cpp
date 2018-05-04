/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Protocol.h>

#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;

StreamConnectionReader::StreamConnectionReader() : ConnectionReader(),
    writer(nullptr),
    in(nullptr),
    str(nullptr),
    protocol(nullptr),
    messageLen(0),
    textMode(false),
    bareMode(false),
    valid(false),
    err(false),
    shouldDrop(false),
    writePending(false),
    ref(nullptr),
    convertedTextMode(false),
    pushedIntFlag(false),
    pushedInt(-1),
    parentConnectionReader(nullptr)
{ }

StreamConnectionReader::~StreamConnectionReader()
{
    if (writer!=nullptr)
    {
        delete writer;
        writer = nullptr;
    }
}

void StreamConnectionReader::reset(InputStream &in,
                              TwoWayStream *str,
                              const Route &route,
                              size_t len,
                              bool textMode,
                              bool bareMode)
{
    this->in = &in;
    this->str = str;
    this->route = route;
    this->messageLen = len;
    this->textMode = textMode;
    this->bareMode = bareMode;
    this->valid = true;
    ref = nullptr;
    err = false;
    convertedTextMode = false;
    pushedIntFlag = false;
}

void StreamConnectionReader::setProtocol(Protocol *protocol)
{
    this->protocol = protocol;
}

void StreamConnectionReader::suppressReply()
{
    str = nullptr;
}

bool StreamConnectionReader::dropRequested()
{
    return shouldDrop;
}

bool StreamConnectionReader::expectBlock(const Bytes &b)
{
    if (!isGood()) {
        return false;
    }
    yAssert(in!=nullptr);
    size_t len = b.length();
    if (len==0) {
        return true;
    }
    //if (len<0) len = messageLen;
    if (len>0) {
        YARP_SSIZE_T rlen = in->readFull(b);
        if (rlen>=0) {
            messageLen -= len;
            return true;
        }
    }
    err = true;
    return false;
}

ConstString StreamConnectionReader::expectString(int len)
{
    if (!isGood()) {
        return "";
    }
    char *buf = new char[len];
    yarp::os::Bytes b(buf, len);
    yAssert(in!=nullptr);
    YARP_SSIZE_T r = in->read(b);
    if (r<0 || (size_t)r<b.length()) {
        err = true;
        delete[] buf;
        return "";
    }
    messageLen -= b.length();
    ConstString s = buf;
    delete[] buf;
    return s;
}

ConstString StreamConnectionReader::expectLine()
{
    if (!isGood()) {
        return "";
    }
    yAssert(in!=nullptr);
    bool success = false;
    ConstString result = in->readLine('\n', &success);
    if (!success) {
        err = true;
        return "";
    }
    messageLen -= result.length()+1;
    return result;
}

void StreamConnectionReader::flushWriter()
{
    if (writer!=nullptr) {
        if (writePending) {
            if (str!=nullptr) {
                if (protocol!=nullptr) {
                    protocol->reply(*writer);
                } else {
                    writer->write(str->getOutputStream());
                }
                writer->clear();
            }
        }
    }
    writePending = false;
}

void StreamConnectionReader::setReference(yarp::os::Portable *obj)
{
    ref = obj;
}

bool StreamConnectionReader::setSize(size_t len)
{
    reset(*in, str, route, len, textMode, bareMode);
    return true;
}

size_t StreamConnectionReader::getSize()
{
    return messageLen + (pushedIntFlag?sizeof(yarp::os::NetInt32):0);
}

bool StreamConnectionReader::pushInt(int x)
{
    if (pushedIntFlag) {
        return false;
    }
    pushedIntFlag = true;
    pushedInt = x;
    return true;
}

int StreamConnectionReader::expectInt()
{
    if (pushedIntFlag) {
        pushedIntFlag = false;
        return pushedInt;
    }
    if (!isGood()) {
        return 0;
    }
    NetInt32 x = 0;
    yarp::os::Bytes b((char*)(&x), sizeof(x));
    yAssert(in!=nullptr);
    YARP_SSIZE_T r = in->read(b);
    if (r<0 || (size_t)r<b.length()) {
        err = true;
        return 0;
    }
    messageLen -= b.length();
    return x;
}

YARP_INT64 StreamConnectionReader::expectInt64()
{
    if (!isGood()) {
        return 0;
    }
    NetInt64 x = 0;
    yarp::os::Bytes b((char*)(&x), sizeof(x));
    yAssert(in!=nullptr);
    YARP_SSIZE_T r = in->read(b);
    if (r<0 || (size_t)r<b.length()) {
        err = true;
        return 0;
    }
    messageLen -= b.length();
    return x;
}

double StreamConnectionReader::expectDouble()
{
    if (!isGood()) {
        return 0;
    }
    NetFloat64 x = 0;
    yarp::os::Bytes b((char*)(&x), sizeof(x));
    yAssert(in!=nullptr);
    YARP_SSIZE_T r = in->read(b);
    if (r<0 || (size_t)r<b.length()) {
        err = true;
        return 0;
    }
    messageLen -= b.length();
    return x;
}

bool StreamConnectionReader::expectBlock(const char *data, size_t len)
{
    return expectBlock(yarp::os::Bytes((char*)data, len));
}

yarp::os::ConstString StreamConnectionReader::expectText(int terminatingChar)
{
    if (!isGood()) {
        return "";
    }
    yAssert(in!=nullptr);
    bool lsuccess = false;
    in->setReadTimeout(2.0);
    ConstString result = in->readLine(terminatingChar, &lsuccess);
    in->setReadTimeout(0.0);
    if (lsuccess) {
        messageLen -= result.length()+1;
    }
    return ::yarp::os::ConstString(result.c_str());
}

bool StreamConnectionReader::isTextMode()
{
    return textMode;
}

bool StreamConnectionReader::isBareMode()
{
    return bareMode;
}

bool StreamConnectionReader::convertTextMode()
{
    if (isTextMode()) {
        if (!convertedTextMode) {
            Bottle bot;
            bot.read(*this);
            BufferedConnectionWriter writer;
            bot.write(writer);
            ConstString s = writer.toString();
            altStream.reset(s);
            in = &altStream;
            convertedTextMode = true;
        }
    }

    return true;
}

yarp::os::ConnectionWriter* StreamConnectionReader::getWriter()
{
    if (str==nullptr) {
        return nullptr;
    }
    if (writer==nullptr) {
        writer = new BufferedConnectionWriter(isTextMode(), isBareMode());
        yAssert(writer!=nullptr);
    }
    writer->clear();
    writePending = true;
    if (protocol!=nullptr) {
        protocol->willReply();
    }
    return writer;
}

yarp::os::Contact StreamConnectionReader::getRemoteContact()
{
    if (str!=nullptr) {
        Contact remote = str->getRemoteAddress();
        remote.setName(route.getFromName());
        return remote;
    }
    Contact remote = yarp::os::Contact(route.getFromName(), route.getCarrierName());
    return remote;
}

yarp::os::Contact StreamConnectionReader::getLocalContact()
{
    if (str!=nullptr) {
        Contact local = str->getLocalAddress();
        local.setName(route.getToName());
        return local;
    }
    return yarp::os::Contact();
}

bool StreamConnectionReader::isValid()
{
    return valid;
}

bool StreamConnectionReader::isError()
{
    if (err) {
        return true;
    }
    return !isActive();
}

bool StreamConnectionReader::isActive()
{
    if (shouldDrop) {
        return false;
    }
    if (!isValid()) {
        return false;
    }
    if (in!=nullptr) {
        if (in->isOk()) {
            return true;
        }
    }
    return false;
}

yarp::os::Portable* StreamConnectionReader::getReference()
{
    return ref;
}

Bytes StreamConnectionReader::readEnvelope()
{
    if (protocol != nullptr) {
        const ConstString& env = protocol->getEnvelope();
        return Bytes((char*)env.c_str(), env.length());
    }
    if (parentConnectionReader != nullptr) {
        return parentConnectionReader->readEnvelope();
    }
    return Bytes(nullptr, 0);
}

void StreamConnectionReader::requestDrop()
{
    shouldDrop = true;
}

Searchable& StreamConnectionReader::getConnectionModifiers()
{
    if (config.size()==0) {
        if (protocol) {
            config.fromString(protocol->getSenderSpecifier().c_str());
        }
    }
    return config;
}

void StreamConnectionReader::setParentConnectionReader(ConnectionReader *parentConnectionReader)
{
    this->parentConnectionReader = parentConnectionReader;
}

bool StreamConnectionReader::isGood()
{
    return isActive()&&isValid()&&!isError();
}
