/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

using namespace yarp::os;

class Responder : public PortReader {
public:
    std::string getCss() {
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

        std::string code = request.get(0).asString();
        if (code=="css") {
            response.addString(getCss());
            response.addString("mime");
            response.addString("text/css");
            return response.write(*out);
        }

        std::string prefix = "<html>\n<head>\n<title>YARP web test</title>\n";
        prefix += "<link href=\"/css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />\n";
        prefix += "</head>\n<body>\n";

        if (code=="push") {
            prefix += "<h1>Potato count</h1>\n";
            prefix += "<div>(<a href='/test'>back</a>)</div>\n";

            response.addString(prefix);
            response.addString("stream");
            response.addInt32(1);
            return response.write(*out);
        }

        std::string postfix = "</body>\n</html>";

        std::string txt = prefix;
        txt += std::string("<h1>") + code + "</h1>\n";
        txt += "<div>Is this working for you? <a href='/yes'>yes</a> <a href='/no'>no</a></div>\n";
        if (!request.check("day")) {
            txt += "<div>By the way, what day is it?</div>\n<form><input type='text' id='day' name='day' value='Sunday' /><input type='submit' value='tell me' /></form>\n";
        } else {
            txt += std::string("<div>So today is ") + request.find("day").asString() + ", is it? Hmm. I don't think I'm going to bother remembering that.</div>\n";
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

    std::string name = options.check("name",Value("/web")).asString();
    int port_number = options.check("p",Value(0)).asInt32();

    Network yarp;
    Port server;
    Responder responder;
    server.setReader(responder);

    Contact contact(name);
    if (port_number!=0) {
        contact.setSocket("", "", port_number);
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
        std::stringstream ss;
        ss << at;
        std::string div = std::string("<div>")+ss.str()+
            " potatoes</div>";
        push.addString(div);
        server.write(push);
        at++;
        Time::delay(1);
    }

    return 0;
}
