/*
 * Copyright: (C) 2012 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>

using namespace yarp::os;

class Responder : public PortReader {
public:
    ConstString getCss() {
        return "\n\
body { background: black; color: white; } \n\
h1 { font-size: 300%; color: yellow; } \n\
div { padding-bottom: 10px; } \n\
";
    }

    virtual bool read(ConnectionReader& in) {
        Bottle request, response;
        if (!request.read(in)) return false;
        printf("Request: %s\n", request.toString().c_str());
        ConnectionWriter *out = in.getWriter();
        if (out==NULL) return true;
        response.addString("web");

        ConstString code = request.get(0).asString();
        if (code=="css") {
            response.addString(getCss());
            response.addString("mime");
            response.addString("text/css");
            return response.write(*out);
        }

        ConstString prefix = "<html>\n<head>\n<title>YARP web test</title>\n";
        prefix += "<link href=\"/css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />\n";
        prefix += "</head>\n<body>\n";

        if (code=="push") {
            prefix += "<h1>Potato count</h1>\n";
            prefix += "<div>(<a href='/test'>back</a>)</div>\n";

            response.addString(prefix);
            response.addString("stream");
            response.addInt(1);
            return response.write(*out);
        }

        ConstString postfix = "</body>\n</html>";

        ConstString txt = prefix;
        txt += ConstString("<h1>") + code + "</h1>\n";
        txt += "<div>Is this working for you? <a href='/yes'>yes</a> <a href='/no'>no</a></div>\n";
        if (!request.check("day")) {
            txt += "<div>By the way, what day is it?</div>\n<form><input type='text' id='day' name='day' value='Sunday' /><input type='submit' value='tell me' /></form>\n";
        } else {
            txt += ConstString("<div>So today is ") + request.find("day").asString() + ", is it? Hmm. I don't think I'm going to bother remembering that.</div>\n";
        }
        txt += "<div><a href='/push'>How many potatoes?</a> (streaming example)</div>\n";
        txt += postfix;

        response.addString(txt);
        return response.write(*out);
    }
};

int main(int argc, char *argv[]) {
    Property options;
    options.fromCommand(argc,argv);

    ConstString name = options.check("name",Value("/web")).asString();
    int port_number = options.check("p",Value(0)).asInt();

    Network yarp;
    Port server;
    Responder responder;
    server.setReader(responder);

    Contact contact = Contact::byName(name);
    if (port_number!=0) {
        contact = contact.addSocket("","",port_number);
    }
    if (!server.open(contact)) return 1;
    contact = server.where();

    int at = 0;
    while (true) {
        if (at%10==0) {
            printf("Server running, count at %d, visit: http://%s:%d/test\n",
                   at,
                   contact.getHost().c_str(),
                   contact.getPort());
        }
        Bottle push;
        push.addString("web");
        ConstString div = ConstString("<div>")+ConstString::toString(at)+
            " potatoes</div>";
        push.addString(div);
        server.write(push);
        at++;
        Time::delay(1);
    }

    return 0;
}
