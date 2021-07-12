/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/HttpCarrier.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Property.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/impl/NameClient.h>

using namespace yarp::os;
using namespace yarp::os::impl;

static std::string quoteFree(const std::string& src)
{
    std::string result;
    for (char ch : src) {
        if (ch == '"') {
            result += "&quot;";
        } else {
            result += ch;
        }
    }
    return result;
}

static bool asJson(std::string& accum,
                   yarp::os::Bottle* bot,
                   std::string* hint = nullptr);

static bool asJson(std::string& accum,
                   yarp::os::Value& v)
{
    if (v.isInt32() || v.isFloat64()) {
        accum += v.toString();
        return true;
    }
    if (v.isString() || v.isVocab32()) {
        std::string x = v.toString();
        accum += "\"";
        for (char ch : x) {
            if (ch == '\n') {
                accum += '\\';
                accum += 'n';
            } else if (ch == '\r') {
                accum += '\\';
                accum += 'r';
            } else if (ch == '\0') {
                accum += '\\';
                accum += '0';
            } else {
                if (ch == '\\' || ch == '\"') {
                    accum += '\\';
                }
                accum += ch;
            }
        }
        accum += "\"";
    }
    if (v.isList()) {
        yarp::os::Bottle* bot = v.asList();
        return asJson(accum, bot);
    }
    return false;
}

static bool asJson(std::string& accum,
                   yarp::os::Bottle* bot,
                   std::string* hint)
{
    if (bot == nullptr) {
        return false;
    }
    bool struc = false;
    bool struc_set = false;
    int offset = 0;
    int offset2 = 0;
    std::string tag = bot->get(0).asString();
    if (hint != nullptr) {
        if ((*hint) == "list") {
            struc = false;
            struc_set = true;
        } else if ((*hint) == "dict") {
            struc = true;
            struc_set = true;
        }
    }
    if (!struc_set) {
        if (tag == "list") {
            struc = false;
            offset = 1;
        } else if (tag == "dict") {
            struc = true;
            offset = 1;
        } else {
            // auto-detect
            struc = (bot->size() > 1);
            if (bot->size() > 0) {
                yarp::os::Value& v0 = bot->get(0);
                if (!v0.isList()) {
                    offset2 = 1;
                    offset = 1;
                }
            }
            for (size_t i = offset2; i < bot->size(); i++) {
                yarp::os::Value& vi = bot->get(i);
                if (!vi.isList()) {
                    struc = false;
                    break;
                }
                if (vi.asList()->size() != 2) {
                    struc = false;
                    break;
                }
            }
        }
    }
    if (struc) {
        // { ... }
        accum += "{";
        bool need_comma = false;
        if (offset2 != 0) {
            accum += "\"type\": ";
            asJson(accum, bot->get(0));
            need_comma = true;
        }
        for (size_t i = offset; i < bot->size(); i++) {
            yarp::os::Bottle* boti = bot->get(i).asList();
            if (boti == nullptr) {
                continue;
            }
            if (need_comma) {
                accum += ", ";
            }
            asJson(accum, boti->get(0));
            accum += ": ";
            asJson(accum, boti->get(1));
            need_comma = true;
        }
        accum += "}";
        return true;
    }

    // [ ... ]
    accum += "[";
    if (offset2 != 0) {
        offset--;
    }
    for (int i = offset; (size_t)i < bot->size(); i++) {
        if (i > offset) {
            accum += ", ";
        }
        asJson(accum, bot->get(i));
    }
    accum += "]";
    return true;
}


yarp::os::impl::HttpTwoWayStream::HttpTwoWayStream(TwoWayStream* delegate, const char* txt, const char* prefix, yarp::os::Property& prop, bool writer) :
        delegate(delegate)
{
    this->isWriter = writer;
    data = false;
    filterData = false;
    chunked = false;
    if (writer) {
        Bottle b;
        b.addString(txt);
        sis.add(b.toString());
        sis.add("\n");
        return;
    }
    std::string s(txt);
    std::string sData;
    Property& p = prop;
    //p.fromQuery(txt);
    format = p.check("format", Value("html")).asString();
    outer = p.check("outer", Value("auto")).asString();
    bool admin = p.check("admin");
    bool req = p.check("req");
    if (p.check("cmd")) {
        s = p.check("cmd", Value("")).asString();
    } else if (p.check("data") || req) {
        if (req) {
            s = p.check("req", Value("")).asString();
            if (!p.check("format")) {
                format = "json";
                p.put("format", "json");
            }
        } else {
            s = p.check("data", Value("")).asString();
        }
        s += " ";
        std::string sFixed;
        std::string var;
        bool arg = false;
        for (unsigned int i = 0; i < s.length(); i++) {
            char ch = s[i];
            if (arg) {
                if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch == '_')) {
                    var += ch;
                } else {
                    arg = false;
                    sFixed += p.check(var, Value("")).toString();
                    if (i != s.length() - 1) {
                        sFixed += ch; // omit padding
                    }
                    var = "";
                }
            } else {
                if (ch == '$') {
                    arg = true;
                } else {
                    if (i != s.length() - 1) {
                        sFixed += ch; // omit padding
                    }
                }
            }
        }

        sData = sFixed;
        if (admin) {
            s = std::string("a\n") + sFixed;
        } else {
            s = std::string("d\n") + sFixed;
        }
    }


    std::string from = prefix;
    from += "<input type=text name=data value=\"";
    from += quoteFree(sData);

    from += "\"><input type=submit value=\"send data\"></form></p>\n";
    from += "<pre>\n";

    bool classic = false;
    // support old-style messages
    if (s.length() <= 1) {
        classic = true;
    }
    if (s.length() > 1) {
        if (s[0] == '?' || s[1] == '?') {
            classic = true;
        }
        if (s[0] == 'd' && s[1] == '\n') {
            classic = true;
        }
    }
    if (s.length() >= 4) {
        if (s[0] == 'f' && s[1] == 'o' && s[2] == 'r' && s[3] == 'm') {
            classic = true;
        }
        if (s[0] == 'd' && s[1] == 'a' && s[2] == 't' && s[3] == 'a') {
            classic = true;
        }
        if (s[0] == 'f' && s[1] == 'a' && s[2] == 'v' && s[3] == 'i') {
            // kill favicon.ico, sorry
            classic = true;
        }
    }
    if (req) {
        classic = false;
    }

    if (classic) {
        std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
        chunked = false;
        if (s[0] == 'r') {
            header += "Transfer-Encoding: chunked\r\n";
            chunked = true;
        }
        header += "\r\n";
        int N = 2 * 1024;
        std::string body = from;
        if (chunked) {
            body += "Reading data from port...\n";
            header += yarp::conf::numeric::to_hex_string((int)(body.length() + N));
            header += "\r\n";
        }

        Bytes b1((char*)header.c_str(), header.length());
        delegate->getOutputStream().write(b1);

        if (chunked) {
            // chrome etc won't render until enough chars are received.
            for (int i = 0; i < N; i++) {
                delegate->getOutputStream().write(' ');
            }
        }

        Bytes b2((char*)body.c_str(), body.length());
        delegate->getOutputStream().write(b2);
        delegate->getOutputStream().write('\r');
        delegate->getOutputStream().write('\n');
        delegate->getOutputStream().flush();

        if (s.empty()) {
            s = "*";
        }
        if (chunked) {
            filterData = true;
        }
        for (char& ch : s) {
            if (ch == ',') {
                ch = '\n';
            }
            if (ch == '+') {
                ch = ' ';
            }
        }
        if (chunked) {
            sis.add("r\n");
        } else {
            sis.add(s);
            sis.add("\nq\n");
        }

    } else {
        chunked = true;
        if (!req) {
            if (admin) {
                sis.add("a\n");
            } else {
                sis.add("d\n");
            }
            for (int i = 0; i < (int)s.length(); i++) {
                if (s[i] == '/') {
                    s[i] = ' ';
                }
                if (s[i] == '?') {
                    s = s.substr(0, i);
                    break;
                }
            }
        }
        sis.add(s);
        sis.add(" ");
        sis.add(p.toString());
        sis.add("\n");
    }
}

yarp::os::impl::HttpTwoWayStream::~HttpTwoWayStream()
{
    if (delegate != nullptr) {
        delete delegate;
        delegate = nullptr;
    }
}

yarp::os::InputStream& yarp::os::impl::HttpTwoWayStream::getInputStream()
{
    return sis;
}

yarp::os::OutputStream& yarp::os::impl::HttpTwoWayStream::getOutputStream()
{
    return *this;
}

const Contact& yarp::os::impl::HttpTwoWayStream::getLocalAddress() const
{
    return delegate->getLocalAddress();
}

const Contact& yarp::os::impl::HttpTwoWayStream::getRemoteAddress() const
{
    return delegate->getRemoteAddress();
}

bool yarp::os::impl::HttpTwoWayStream::isOk() const
{
    return true; //delegate->isOk();
}

void yarp::os::impl::HttpTwoWayStream::reset()
{
    delegate->reset();
}

void yarp::os::impl::HttpTwoWayStream::write(const yarp::os::Bytes& b)
{ // throws
    if (chunked || isWriter) {
        delegate->getOutputStream().write(b);
    } else {
        for (size_t i = 0; i < b.length(); i++) {
            apply(b.get()[i]);
        }
    }
}

void yarp::os::impl::HttpTwoWayStream::apply(char ch)
{
    if (ch == '\r') {
        return;
    }
    if (ch == '\n') {
        proc = "";
        Contact addr = yarp::os::impl::NameClient::extractAddress(part);
        if (addr.isValid()) {
            if (addr.getCarrier() == "tcp" && (addr.getRegName().find("/quit") == std::string::npos)) {
                proc += "<a href=\"http://";
                proc += addr.getHost();
                proc += ":";
                proc += yarp::conf::numeric::to_string(addr.getPort());
                proc += "\">";
                proc += addr.getRegName();
                proc += "</A> ";
                size_t len = addr.getRegName().length();
                size_t target = 30;
                if (len < target) {
                    for (size_t i = 0; i < target - len; i++) {
                        proc += " ";
                    }
                }
                proc += "(";
                proc += addr.toString();
                proc += ")";
                proc += "\n";
            } else {
                // Don't show non tcp connections
                //proc += part;
                //proc += "\n";
            }
        } else {
            if ((part[0] == '\"' && part[1] == '[') || (part[0] == '+')) {
                // translate this to a form
                if (part[0] == '+') {
                    part[0] = ' ';
                }
                std::string org = part;
                part = "<p><form method=post action='/form'>";
                size_t i = 0;
                for (i = 0; i < org.length(); i++) {
                    if (org[i] == '"') {
                        org[i] = ' ';
                    }
                }
                part += "<input type=hidden name=data value=\"";
                part += org;
                part += "\">";
                part += org;
                org += " ";
                bool arg = false;
                std::string var;
                for (i = 0; i < org.length(); i++) {
                    char ch = org[i];
                    if (arg) {
                        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch == '_')) {
                            var += ch;
                        } else {
                            arg = false;
                            part += "\n    ";
                            part += var;
                            part += " ";
                            part += "<input type=text name=";
                            part += var;
                            part += " size=5 value=\"\">";
                            var = "";
                        }
                    }
                    if (ch == '$') {
                        arg = true;
                    }
                }
                part += "<input type=submit value=\"go\">";
                part += "</form></p>";
            }
            proc += part;
            proc += "\n";
        }
        if (data || !filterData) {
            Bytes tmp((char*)proc.c_str(), proc.length());
            delegate->getOutputStream().write(tmp);
            delegate->getOutputStream().flush();
        }
        data = false;
        if (proc[0] == 'd' || proc[0] == 'D') {
            data = true;
        }
        part = "";
    } else {
        part += ch;
    }
}

void yarp::os::impl::HttpTwoWayStream::close()
{
    apply('\n');
    apply('\n');
    delegate->close();
}

void yarp::os::impl::HttpTwoWayStream::beginPacket()
{
    delegate->beginPacket();
}

void yarp::os::impl::HttpTwoWayStream::endPacket()
{
    delegate->endPacket();
}

void yarp::os::impl::HttpTwoWayStream::flip()
{
    sis.add("r\n");
}

void yarp::os::impl::HttpTwoWayStream::finish()
{
    sis.add("q\n");
}

bool yarp::os::impl::HttpTwoWayStream::useJson()
{
    return format == "json";
}

std::string* yarp::os::impl::HttpTwoWayStream::typeHint()
{
    return &outer;
}


yarp::os::impl::HttpCarrier::HttpCarrier() :
        url(""),
        input(""),
        urlDone(false),
        expectPost(false),
        contentLength(0),
        stream(nullptr)
{
}

yarp::os::Carrier* yarp::os::impl::HttpCarrier::create() const
{
    return new HttpCarrier();
}

std::string yarp::os::impl::HttpCarrier::getName() const
{
    return "http";
}

bool yarp::os::impl::HttpCarrier::checkHeader(const Bytes& header, const char* prefix)
{
    if (header.length() == 8) {
        std::string target = prefix;
        for (unsigned int i = 0; i < target.length(); i++) {
            if (!(target[i] == header.get()[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool yarp::os::impl::HttpCarrier::checkHeader(const Bytes& header)
{
    bool ok = checkHeader(header, "GET /");
    if (!ok) {
        // http carrier accepts POST /form but not general posts
        // (leave that to xmlrpc carrier)
        ok = checkHeader(header, "POST /fo");
    } else {
        // make sure it isn't a MJPEG stream get or a websocket request
        ok = !checkHeader(header, "GET /?ac") && !checkHeader(header, "GET /?ws");
    }
    return ok;
}

void yarp::os::impl::HttpCarrier::setParameters(const Bytes& header)
{
    if (header.length() == 8) {
        bool adding = false;
        for (unsigned int j = 0; j < 8; j++) {
            char ch = header.get()[j];
            if (adding) {
                if (ch != ' ') {
                    url += ch;
                } else {
                    urlDone = true;
                    break;
                }
            }
            if (ch == '/') {
                adding = true;
            }
        }
    }
}

void yarp::os::impl::HttpCarrier::getHeader(Bytes& header) const
{
    if (header.length() == 8) {
        std::string target = "GET / HT";
        for (int i = 0; i < 8; i++) {
            header.get()[i] = target[i];
        }
    }
}

bool yarp::os::impl::HttpCarrier::requireAck() const
{
    return false;
}

bool yarp::os::impl::HttpCarrier::isTextMode() const
{
    return true;
}


bool yarp::os::impl::HttpCarrier::supportReply() const
{
    return true;
}

bool yarp::os::impl::HttpCarrier::sendHeader(ConnectionState& proto)
{
    std::string target = "GET / HTTP/1.0\r\n";
    std::string path = proto.getRoute().getToName();
    if (path.size() >= 2) {
        target = "GET " + path + " HTTP/1.0\r\n";
    }
    Contact host = proto.getRoute().getToContact();
    if (!host.getHost().empty()) {
        target += "Host: ";
        target += host.getHost();
        target += "\r\n";
    }
    target += "\r\n";
    Bytes b((char*)target.c_str(), target.length());
    proto.os().write(b);
    return true;
}

bool yarp::os::impl::HttpCarrier::expectSenderSpecifier(ConnectionState& proto)
{
    Route route = proto.getRoute();
    route.setFromName("web");
    proto.setRoute(route);
    std::string remainder = proto.is().readLine();
    if (!urlDone) {
        for (char i : remainder) {
            if (i != ' ') {
                url += i;
            } else {
                break;
            }
        }
    }

    bool done = false;
    expectPost = false;
    contentLength = 0;
    while (!done) {
        std::string result = proto.is().readLine();
        if (result.empty()) {
            done = true;
        } else {
            //printf(">>> %s\n", result.c_str());
            Bottle b;
            b.fromString(result);
            if (b.get(0).asString() == "Content-Length:") {
                //printf("]]] got length %d\n", b.get(1).asInt32());
                contentLength = b.get(1).asInt32();
            }
            if (b.get(0).asString() == "Content-Type:") {
                //printf("]]] got type %s\n", b.get(1).asString());
                if (b.get(1).asString() == "application/x-www-form-urlencoded") {
                    expectPost = true;
                }
            }
        }
    }

    if (expectPost) {
        //printf("[[[this is a post message of length %d]]]\n", contentLength);
        ManagedBytes blk(contentLength + 1);
        Bytes start(blk.get(), contentLength);
        proto.is().readFull(start);
        blk.get()[contentLength] = '\0';
        //printf("message: %s\n", blk.get());
        input = blk.get();
    } else {
        //printf("message: %s\n", url.c_str());
        input = url;
    }
    prop.fromQuery(input.c_str());
    prop.put("REQUEST_URI", url);
    //printf("Property %s\n", prop.toString().c_str());

    Contact home = NetworkBase::getNameServerContact();
    Contact me = proto.getStreams().getLocalAddress();

    std::string from = "<html><head><link href=\"http://";
    from += home.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(home.getPort());
    from += R"(/web/main.css" rel="stylesheet" type="text/css"/></head><body bgcolor='#ffffcc'><h1>yarp port )";
    from += proto.getRoute().getToName();
    from += "</h1>\n";

    from += "<p>(<a href=\"http://";
    from += home.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(home.getPort());
    from += "/data=list\">All ports</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(me.getPort());
    from += "/\">connections</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(me.getPort());
    from += "/data=help\">help</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(me.getPort());
    from += "/r\">read</a>)&nbsp;&nbsp;\n";

    from += "</p>\n";
    from += "<p>\n";
    from += R"(<form method="post" action="http://)";
    from += me.getHost();
    from += ":";
    from += yarp::conf::numeric::to_string(me.getPort());
    from += "/form\">";

    prefix = from;


    //Bytes b2((char*)from.c_str(), from.length());
    //proto.os().write(b2);
    //proto.os().flush();
    // Message gets finished by the stream

    return proto.os().isOk();
}

bool yarp::os::impl::HttpCarrier::expectReplyToHeader(ConnectionState& proto)
{
    input = "";
    yarp::conf::ssize_t len = 1;
    while (len > 0) {
        char buf[2];
        Bytes b((char*)&buf[0], 1);
        len = proto.is().read(b);
        if (len > 0) {
            buf[len] = '\0';
            input += std::string(buf, len);
        }
    }
    stream = new HttpTwoWayStream(proto.giveStreams(),
                                  input.c_str(),
                                  prefix.c_str(),
                                  prop,
                                  true);
    proto.takeStreams(stream);
    return true;
}


bool yarp::os::impl::HttpCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    YARP_UNUSED(proto);
    YARP_UNUSED(writer);
    // no index
    return true;
}

bool yarp::os::impl::HttpCarrier::expectIndex(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    // no index
    return true;
}

bool yarp::os::impl::HttpCarrier::sendAck(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    // no acknowledgement
    return true;
}

bool yarp::os::impl::HttpCarrier::expectAck(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    // no acknowledgement
    return true;
}

bool yarp::os::impl::HttpCarrier::respondToHeader(ConnectionState& proto)
{
    stream = new HttpTwoWayStream(proto.giveStreams(),
                                  input.c_str(),
                                  prefix.c_str(),
                                  prop,
                                  false);
    proto.takeStreams(stream);
    return true;
}

bool yarp::os::impl::HttpCarrier::write(ConnectionState& proto, SizedWriter& writer)
{
    DummyConnector con;
    con.setTextMode(true);
    for (size_t i = writer.headerLength(); i < writer.length(); i++) {
        con.getWriter().appendBlock(writer.data(i), writer.length(i));
    }
    Bottle b;
    b.read(con.getReader());

    std::string body = b.find("web").toString();
    if (body.length() != 0) {
        std::string header;
        header += yarp::conf::numeric::to_hex_string((int)body.length());
        header += "\r\n";

        Bytes b2((char*)header.c_str(), header.length());
        proto.os().write(b2);

        Bytes b3((char*)body.c_str(), body.length());
        proto.os().write(b3);

        proto.os().write('\r');
        proto.os().write('\n');

    } else {
        std::string txt = b.toString() + "\r\n";
        std::string header;
        header += yarp::conf::numeric::to_hex_string((int)txt.length());
        header += "\r\n";
        Bytes b2((char*)header.c_str(), header.length());
        proto.os().write(b2);
        Bytes b3((char*)txt.c_str(), txt.length());
        proto.os().write(b3);
        proto.os().write('\r');
        proto.os().write('\n');
    }
    proto.os().flush();
    return proto.os().isOk();
}

bool yarp::os::impl::HttpCarrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    DummyConnector con;
    con.setTextMode(true);
    for (size_t i = writer.headerLength(); i < writer.length(); i++) {
        con.getWriter().appendBlock(writer.data(i), writer.length(i));
    }
    Bottle b;
    b.read(con.getReader());

    std::string mime = b.check("mime", Value("text/html")).asString();

    std::string body;

    bool using_json = false;
    if (stream != nullptr) {
        if (stream->useJson()) {
            mime = "text/json";
            asJson(body, &b, stream->typeHint());
            using_json = true;
        }
    }

    if (b.check("web") && !using_json) {
        body = b.find("web").toString();
    }

    if (b.check("stream") && !using_json) {
        std::string header("HTTP/1.1 200 OK\r\nContent-Type: ");
        header += mime;
        header += "\r\n";
        header += "Transfer-Encoding: chunked\r\n";
        header += "\r\n";
        int N = 2 * 1024;
        header += yarp::conf::numeric::to_hex_string((int)body.length() + N);
        header += "\r\n";

        Bytes b2((char*)header.c_str(), header.length());
        proto.os().write(b2);

        // chrome etc won't render until enough chars are received.
        for (int i = 0; i < N; i++) {
            proto.os().write(' ');
        }

        Bytes b3((char*)body.c_str(), body.length());
        proto.os().write(b3);

        proto.os().write('\r');
        proto.os().write('\n');


        if (stream != nullptr) {
            stream->flip();
        }
        return true;
    }

    if (stream != nullptr) {
        stream->finish();
    }

    // Could check response codes, mime types here.

    if (body.length() != 0 || using_json) {
        std::string mime = b.check("mime", Value(using_json ? "application/json" : "text/html")).asString();
        std::string header("HTTP/1.1 200 OK\nContent-Type: ");
        header += mime;
        header += "\n";
        header += "Access-Control-Allow-Origin: *\n";
        header += "\n";
        Bytes b2((char*)header.c_str(), header.length());
        proto.os().write(b2);

        //body = b.toString();
        Bytes b3((char*)body.c_str(), body.length());
        proto.os().write(b3);
    } else {
        writer.write(proto.os());
    }
    proto.os().flush();
    return proto.os().isOk();
}
