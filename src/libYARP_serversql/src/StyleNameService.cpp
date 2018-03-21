/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/serversql/impl/StyleNameService.h>

#include <yarp/os/Value.h>

using namespace yarp::os;
using namespace yarp::serversql::impl;

bool StyleNameService::apply(yarp::os::Bottle& cmd,
                             yarp::os::Bottle& reply,
                             yarp::os::Bottle& event,
                             const yarp::os::Contact& remote) {
    if (cmd.get(0).asString()!="web") return false;

    if (!content.check("main.css")) {
        if (!options.check("web")) {
            content.put("main.css","\n\
body {\n\
  background: white;\n\
  color: black;\n\
}\n\
h1 {\n\
  font-family: verdana, arial, sans-serif;\n\
  font-size: 300%;\n\
  color: #339966;\n\
}\n\
div {\n\
  padding-bottom: 10px;\n\
}\n\
a:link{\n\
  color: #114499;\n\
  text-decoration: none;\n\
}\n\
a:visited {\n\
  color: #114499;\n\
  text-decoration: none;\n\
}\n\
a:hover{\n\
  color: red;\n\
  text-decoration: none;\n\
}\n\
");
            mime.put("main.css","text/css");
        }
    }

    Bottle& bot = reply;
    bot.addString("web");

    ConstString code = cmd.get(1).asString();
    ConstString uri = cmd.check("REQUEST_URI",Value("")).toString();
    if (uri.length()>=4) { uri = uri.substr(4); } else { uri = ""; }
    ConstString fileName = uri;

    if ((!content.check(uri))||options.check("no-web-cache")) {
        if (options.check("web")) {
            ConstString accum = "";
            bool first = true;
            for (size_t i=0; i<fileName.length(); i++) {
                char ch = fileName[i];
                if (ch == '.' && !first) continue;
                if (ch == '/') { first = true; continue; }
                first = false;
                if (ch>='a'&&ch<='z') continue;
                if (ch>='A'&&ch<='Z') continue;
                if (ch>='0'&&ch<='9') continue;
                if (ch == '-' || ch == '_') continue;
                ((char*)fileName.c_str())[i] = '_';
            }
            if (fileName == "") {
                fileName = "index.html";
            }
            fileName = options.find("web").asString() + "/" + fileName;
            char buf[25600];
            FILE *fin = fopen(fileName.c_str(),"rb");
            if (fin != nullptr) {
                size_t len = 0;
                do {
                    len = fread(buf,1,sizeof(buf),fin);
                    if (len>=1) {
                        accum += ConstString(buf,len);
                    }
                } while (len>=1);
                fclose(fin);
                fin = nullptr;
            }
            content.put(uri,accum);
            if (uri.find(".css")!=ConstString::npos) {
                mime.put(uri,"text/css");
            } else if (uri.find(".png")!=ConstString::npos) {
                mime.put(uri,"image/png");
            } else if (uri.find(".jpg")!=ConstString::npos) {
                mime.put(uri,"image/jpeg");
            } else if (uri.find(".js")!=ConstString::npos) {
                mime.put(uri,"text/javascript");
            } else {
                mime.put(uri,"text/html");
            }
        }
    }

    if (content.check(uri)) {
        ConstString txt = content.find(uri).asString();
        ConstString txtMime = mime.find(uri).asString();
        printf(" * %s %s %d bytes, %s\n",
               cmd.toString().c_str(),
               (fileName!=uri)?fileName.c_str():"",
               (int)txt.length(),
               txtMime.c_str());
        bot.addString(txt);
        bot.addString("mime");
        bot.addString(txtMime);
    } else {
        printf(" * %s - Unknown\n", cmd.toString().c_str());
        bot.addString("Hmm.");
    }
    return true;
}
