/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/utility.h>
#include <yarp/conf/filesystem.h>
#include <dirent.h>
#include <tinyxml.h>
#include <yarp/os/Value.h>
#ifdef WITH_GEOMETRY
#include <yarp/os/Property.h> // for parsing geometry information
#endif

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <yarp/manager/impl/textparser.h>



using namespace std;
using namespace yarp::manager;


/**
 * if szAppName is NULL, XmlAppLoader will load all the applications
 * found in szPath otherwise only one application named szAppname
 * will be loaded.
 */
XmlAppLoader::XmlAppLoader(const char* szPath, const char* szAppName)
{
    parser = new(TextParser);
    app.clear();
    if(szAppName)
        strAppName = szAppName;

    if(strlen(szPath))
    {
        const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
        strPath = szPath;
        if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1))
            strPath = strPath + string(directorySeparator);
    }
}

/**
 * load only one application indicated by its xml file name
 */
XmlAppLoader::XmlAppLoader(const char* szFileName)
{
    parser = new(TextParser);
    app.clear();
    if(szFileName)
        strFileName = szFileName;
}


XmlAppLoader::~XmlAppLoader()
{
    if(parser)
    {
        delete parser;
    }
}


bool XmlAppLoader::init()
{
    app.clear();
    fileNames.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();

    /**
     * loading single applicaition indicated by its xml file name
     */
    if(!strFileName.empty())
    {
        fileNames.push_back(strFileName);
        return true;
    }

    if(strPath.empty())
    {
        logger->addError("No application path is introduced.");
        return false;
    }

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == nullptr)
    {
        OSTRINGSTREAM err;
        err<<"Cannot access "<<strPath;
        logger->addError(err);
        return false;
    }

    /* we need to load all xml apps */
    while((entry = readdir(dir)))
    {
        string name = entry->d_name;
        if(name.size() > 3)
        {
            string ext = name.substr(name.size()-3,3);
            if(compareString(ext.c_str(), "xml"))
                fileNames.push_back(strPath+name);
        }
    }
    closedir(dir);
/*
    if(fileNames.empty())
    {
        OSTRINGSTREAM err;
        err<<"No xml application file found in "<<strPath;
        logger->addWarning(err);
        //return false;
    }
*/
    return true;
}

void XmlAppLoader::reset()
{
    fini();
    init();
}


void XmlAppLoader::fini()
{
    fileNames.clear();
    app.clear();
}


Application* XmlAppLoader::getNextApplication()
{
    if(strAppName.empty())
    {
        Application* app = nullptr;
        while(!app)
        {
            if(fileNames.empty())
                return nullptr;
            string fname = fileNames.back();
            fileNames.pop_back();
            app = parsXml(fname.c_str());
        }
        return app;
    }
    else
    {
        vector<string>::iterator itr;
        for(itr=fileNames.begin(); itr<fileNames.end(); itr++)
        {
         Application* app = parsXml((*itr).c_str());
         if(app && (string(app->getName())==strAppName))
            return app;
        }
    }
 return nullptr;
}

Application* XmlAppLoader::parsXml(const char* szFile)
{
    app.clear();

    ErrorLogger* logger  = ErrorLogger::Instance();

    TiXmlDocument doc(szFile);
    if(!doc.LoadFile())
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" at line "\
           <<doc.ErrorRow()<<": ";
        err<<doc.ErrorDesc();
        logger->addError(err);
        return nullptr;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return nullptr;
    }

    if(!compareString(root->Value(), "application"))
    {
        //OSTRINGSTREAM err;
        //err<<"File "<<szFile<<" has no tag <application>.";
        //logger->addError(err);
        return nullptr;
    }

    /* retrieving name */
    auto* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        OSTRINGSTREAM err;
        err<<"Module from "<<szFile<<" has no name.";
        logger->addError(err);
        //return NULL;
    }

    for(TiXmlElement* var = root->FirstChildElement("var"); var; var = var->NextSiblingElement())
    {
        if(var->Attribute("name") && var->GetText())
        {
            parser->addVariable(var->Attribute("name"), var->GetText());
        }
    }

    app.setXmlFile(szFile);

    if(name)
    {
        string strname = parser->parseText(name->GetText());
        for(char& i : strname)
            if(i == ' ')
                i = '_';
        app.setName(strname.c_str());
    }

    /* retrieving description */
    TiXmlElement* desc;
    if((desc = (TiXmlElement*) root->FirstChild("description")))
        app.setDescription(parser->parseText(desc->GetText()).c_str());

    /* retrieving version */
    TiXmlElement* ver;
    if((ver = (TiXmlElement*) root->FirstChild("version")))
        app.setVersion(parser->parseText(ver->GetText()).c_str());

    /*
     * TODO: setting prefix of the main application is inactivated.
     * Check this should be supported in future or not!
     */
    /*
    //retrieving application prefix
    TiXmlElement* pref;
    if((pref = (TiXmlElement*) root->FirstChild("prefix")))
        app.setPrefix(pref->GetText());
    */

    /* retrieving authors information*/
    TiXmlElement* authors;
    if((authors = (TiXmlElement*) root->FirstChild("authors")))
        for(TiXmlElement* ath = authors->FirstChildElement(); ath;
                ath = ath->NextSiblingElement())
        {
            if(compareString(ath->Value(), "author"))
            {
                Author author;
                if(ath->GetText())
                    author.setName(parser->parseText(ath->GetText()).c_str());
                if(ath->Attribute("email"))
                    author.setEmail(ath->Attribute("email"));
                app.addAuthor(author);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<ath->Row()<<".";
                logger->addWarning(war);
            }
        }

    /* retrieving resources information*/
    TiXmlElement* resources;
    if((resources = (TiXmlElement*) root->FirstChild("dependencies")))
        for(TiXmlElement* res = resources->FirstChildElement(); res;
                res = res->NextSiblingElement())
        {
            if(compareString(res->Value(), "port"))
            {
                if(res->GetText())
                {
                    ResYarpPort resource(parser->parseText(res->GetText()).c_str());
                    resource.setPort(parser->parseText(res->GetText()).c_str());
                    app.addResource(resource);
                }
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Unrecognized tag from "<<szFile<<" at line "\
                   <<res->Row()<<".";
                logger->addWarning(war);
            }
        }

    /* retrieving modules information*/
    using setter = void (ModuleInterface::*)(const char*);

    vector<pair<const char*, setter> > modList;
    pair<const char*, setter>          pairNode;

    pairNode.first = "node";        pairNode.second = &ModuleInterface::setHost;        modList.push_back(pairNode);
    pairNode.first = "parameters";  pairNode.second = &ModuleInterface::setParam;       modList.push_back(pairNode);
    pairNode.first = "stdio";       pairNode.second = &ModuleInterface::setStdio;       modList.push_back(pairNode);
    pairNode.first = "workdir";     pairNode.second = &ModuleInterface::setWorkDir;     modList.push_back(pairNode);
    pairNode.first = "deployer";    pairNode.second = &ModuleInterface::setBroker;      modList.push_back(pairNode);
    pairNode.first = "prefix";      pairNode.second = &ModuleInterface::setPrefix;      modList.push_back(pairNode);
    pairNode.first = "environment"; pairNode.second = &ModuleInterface::setEnvironment; modList.push_back(pairNode);
    pairNode.first = "display";     pairNode.second = &ModuleInterface::setDisplay;     modList.push_back(pairNode);
    for(TiXmlElement* mod = root->FirstChildElement(); mod; mod = mod->NextSiblingElement())
    {
        if(compareString(mod->Value(), "module"))
        {
            TiXmlElement* element;
            if((element = (TiXmlElement*) mod->FirstChild("name")))
            {
                string                             elemText;
                const char*                        text;


                text = nullptr;
                if(element->GetText())
                {
                    elemText = parser->parseText(element->GetText());
                    text     = elemText.c_str();
                }

                ModuleInterface module(text);

                for(auto& i : modList)
                {
                    if((element = (TiXmlElement*) mod->FirstChild(i.first)))
                    {
                        text = nullptr;
                        if(element->GetText())
                        {
                            elemText = parser->parseText(element->GetText());
                            text     = elemText.c_str();
                        }

                        (module.*(i.second))(text);
                    }
                }

                if((element = (TiXmlElement*) mod->FirstChild("rank")))
                {
                    if(element->GetText())
                    {
                        elemText = parser->parseText(element->GetText());
                        text     = elemText.c_str();
                    }

                    module.setRank(atoi(text));
                }

#ifdef WITH_GEOMETRY
                element = (TiXmlElement*) mod->FirstChild("geometry");
                if(element && element->GetText())
                {
                    yarp::os::Property prop(parser->parseText(element->GetText()).c_str());
                    GraphicModel model;
                    GyPoint pt;
                    if(prop.check("Pos"))
                    {
                        pt.x = prop.findGroup("Pos").find("x").asFloat64();
                        pt.y = prop.findGroup("Pos").find("y").asFloat64();
                        model.points.push_back(pt);
                        module.setModelBase(model);
                    }
                }
#endif
                /* retrieving resources information*/
                TiXmlElement* resources;
                if((resources = (TiXmlElement*) mod->FirstChild("dependencies")))
                {
                    for(TiXmlElement* res = resources->FirstChildElement(); res;
                            res = res->NextSiblingElement())
                    {
                        if(compareString(res->Value(), "port"))
                        {
                            if(res->GetText())
                            {
                                ResYarpPort resource(parser->parseText(res->GetText()).c_str());
                                resource.setPort(parser->parseText(res->GetText()).c_str());
                                if(res->Attribute("timeout"))
                                    resource.setTimeout(atof(res->Attribute("timeout")));
                                if(res->Attribute("request"))
                                    resource.setRequest(res->Attribute("request"));
                                if(res->Attribute("reply"))
                                    resource.setReply(res->Attribute("reply"));
                                module.addResource(resource);
                            }
                        }
                        else
                        {
                            OSTRINGSTREAM war;
                            war<<"Unrecognized tag from "<<szFile<<" at line "\
                               <<res->Row()<<".";
                            logger->addWarning(war);
                        }
                    }
                }

                /* retrieving resources information*/
                TiXmlElement* ensure;
                if((ensure = (TiXmlElement*) mod->FirstChild("ensure")))
                {
                    for(TiXmlElement* res = ensure->FirstChildElement(); res;
                            res = res->NextSiblingElement())
                    {
                        if(compareString(res->Value(), "wait"))
                        {
                            if (res->Attribute("when") && compareString(res->Attribute("when"), "start")) {
                                if(parser->parseText(res->GetText()).c_str())
                                    module.setPostExecWait(atof(parser->parseText(res->GetText()).c_str()));
                            }
                            else if (res->Attribute("when") && compareString(res->Attribute("when"), "stop")) {
                                if(parser->parseText(res->GetText()).c_str())
                                    module.setPostStopWait(atof(parser->parseText(res->GetText()).c_str()));
                            }
                            else if (res->Attribute("when") && strlen(res->Attribute("when"))) {
                                OSTRINGSTREAM war;
                                war << "Unrecognized value for 'when' property from " << szFile << " at line "<< res->Row() << ".";
                                logger->addWarning(war);
                            }
                            else {  // if "when" has not been specified, use setPostExecWait!
                                if(parser->parseText(res->GetText()).c_str())
                                    module.setPostExecWait(atof(parser->parseText(res->GetText()).c_str()));
                            }
                        }
                        else
                        {
                            OSTRINGSTREAM war;
                            war<<"Unrecognized tag from "<<szFile<<" at line "\
                               <<res->Row()<<".";
                            logger->addWarning(war);
                        }
                    }
                }
                /* retrieving portmaps */
                for(TiXmlElement* map = mod->FirstChildElement(); map;
                            map = map->NextSiblingElement())
                    if(compareString(map->Value(), "portmap"))
                    {
                        TiXmlElement* first;
                        TiXmlElement* second;
                        if((first=(TiXmlElement*) map->FirstChild("old")) &&
                           (second=(TiXmlElement*) map->FirstChild("new")) )
                        {
                            Portmap portmap(parser->parseText(first->GetText()).c_str(), parser->parseText(second->GetText()).c_str());
                            module.addPortmap(portmap);
                        }
                    }

                app.addImodule(module);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Module from "<<szFile<<" at line "\
                   <<mod->Row()<<" has not name tag.";
                logger->addWarning(war);
            }

        }
    }

    /* retrieving embedded application information*/
    for(TiXmlElement* embApp = root->FirstChildElement(); embApp;
            embApp = embApp->NextSiblingElement())
    {
        if(compareString(embApp->Value(), "application"))
        {
            TiXmlElement* name;
            TiXmlElement* prefix;
            if((name=(TiXmlElement*) embApp->FirstChild("name")))
            {
                ApplicationInterface IApp(parser->parseText(name->GetText()).c_str());
                if((prefix=(TiXmlElement*) embApp->FirstChild("prefix")))
                    IApp.setPrefix(parser->parseText(prefix->GetText()).c_str());
#ifdef WITH_GEOMETRY
                auto* element = (TiXmlElement*) embApp->FirstChild("geometry");
                if(element && element->GetText())
                {
                    yarp::os::Property prop(parser->parseText(element->GetText()).c_str());
                    GraphicModel model;
                    GyPoint pt;
                    if(prop.check("Pos"))
                    {
                        pt.x = prop.findGroup("Pos").find("x").asFloat64();
                        pt.y = prop.findGroup("Pos").find("y").asFloat64();
                        model.points.push_back(pt);
                        IApp.setModelBase(model);
                    }
                }
#endif
                app.addIapplication(IApp);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Incomplete application tag from "<<szFile<<" at line "\
                   <<embApp->Row()<<". (no name)";
                logger->addWarning(war);
            }
        }
    }


    /* retrieving arbitrator information*/
    for(TiXmlElement* arb = root->FirstChildElement(); arb;
            arb = arb->NextSiblingElement())
    {
        if(compareString(arb->Value(), "arbitrator"))
        {
            auto* port = (TiXmlElement*) arb->FirstChild("port");
            if(port && port->GetText())
            {
                Arbitrator arbitrator(parser->parseText(port->GetText()).c_str());

                // retrieving rules
                for(TiXmlElement* rule = arb->FirstChildElement(); rule;
                    rule = rule->NextSiblingElement())
                {
                    if(compareString(rule->Value(), "rule"))
                    {
                        if(rule->Attribute("connection"))
                            arbitrator.addRule(rule->Attribute("connection"), parser->parseText(rule->GetText()).c_str());
                    }
                }
#ifdef WITH_GEOMETRY
                auto* geometry = (TiXmlElement*) arb->FirstChild("geometry");
                if(geometry && geometry->GetText())
                {
                    yarp::os::Property prop(parser->parseText(geometry->GetText()).c_str());
                    GraphicModel model;
                    if(prop.check("Pos"))
                    {
                        yarp::os::Bottle pos = prop.findGroup("Pos");
                        for(size_t i=1; i<pos.size(); i++)
                        {
                            GyPoint pt;
                            pt.x = pos.get(i).find("x").asFloat64();
                            pt.y = pos.get(i).find("y").asFloat64();
                            model.points.push_back(pt);
                        }
                        arbitrator.setModelBase(model);
                    }
                }
#endif
                app.addArbitrator(arbitrator);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Incomplete arbitrator tag from "<<szFile<<" at line "\
                   <<arb->Row()<<".";
                logger->addWarning(war);
            }
        }
    }

    /* retrieving connections information*/
    for(TiXmlElement* cnn = root->FirstChildElement(); cnn;
            cnn = cnn->NextSiblingElement())
    {
        if(compareString(cnn->Value(), "connection"))
        {
            auto* from = (TiXmlElement*) cnn->FirstChild("from");
            auto* to = (TiXmlElement*) cnn->FirstChild("to");

            if(!from)
                from = (TiXmlElement*) cnn->FirstChild("output");
            if(!to)
                to = (TiXmlElement*) cnn->FirstChild("input");

            TiXmlElement* protocol;
            if(from && to)
            {
                string strCarrier;
                if((protocol=(TiXmlElement*) cnn->FirstChild("protocol")) &&
                    protocol->GetText())
                    strCarrier = parser->parseText(protocol->GetText());
                Connection connection(parser->parseText(from->GetText()).c_str(),
                                    parser->parseText(to->GetText()).c_str(),
                                    strCarrier.c_str());

                // check if Qos is set for the connection
                if(cnn->Attribute("qos")) {
                    connection.setQosTo(cnn->Attribute("qos"));
                    connection.setQosFrom(cnn->Attribute("qos"));
                }

                if(from->Attribute("external") &&
                    compareString(from->Attribute("external"), "true"))
                {
                    connection.setFromExternal(true);
                    if(from->GetText())
                    {
                        ResYarpPort resource(parser->parseText(from->GetText()).c_str());
                        resource.setPort(parser->parseText(from->GetText()).c_str());
                        app.addResource(resource);
                    }
                }
                if(from->Attribute("qos"))
                    connection.setQosFrom(from->Attribute("qos"));
                if(to->Attribute("external") &&
                    compareString(to->Attribute("external"), "true"))
                {
                    if(to->GetText())
                    {
                        connection.setToExternal(true);
                        ResYarpPort resource(parser->parseText(to->GetText()).c_str());
                        resource.setPort(parser->parseText(to->GetText()).c_str());
                        app.addResource(resource);
                    }
                }
                if(to->Attribute("qos"))
                    connection.setQosTo(to->Attribute("qos"));

                //Connections which have the same port name in Port Resources
                // should also be set as external
                for(int i=0; i<app.resourcesCount(); i++)
                {
                    ResYarpPort res = app.getResourceAt(i);
                    if(compareString(res.getPort(), connection.from()))
                        connection.setFromExternal(true);
                    if(compareString(res.getPort(), connection.to()))
                        connection.setToExternal(true);
                }

                if(cnn->Attribute("id"))
                    connection.setId(cnn->Attribute("id"));

                if(cnn->Attribute("persist") &&
                        compareString(cnn->Attribute("persist"), "true"))
                    connection.setPersistent(true);

#ifdef WITH_GEOMETRY
                auto* geometry = (TiXmlElement*) cnn->FirstChild("geometry");
                if(geometry && geometry->GetText())
                {
                    yarp::os::Property prop(parser->parseText(geometry->GetText()).c_str());
                    GraphicModel model;
                    if(prop.check("Pos"))
                    {
                        yarp::os::Bottle pos = prop.findGroup("Pos");
                        for(size_t i=1; i<pos.size(); i++)
                        {
                            GyPoint pt;
                            pt.x = pos.get(i).find("x").asFloat64();
                            pt.y = pos.get(i).find("y").asFloat64();
                            model.points.push_back(pt);
                        }
                        connection.setModelBase(model);
                    }
                }
#endif

                app.addConnection(connection);
            }
            else
            {
                OSTRINGSTREAM war;
                war<<"Incomplete connection tag from "<<szFile<<" at line "\
                   <<cnn->Row()<<".";
                logger->addWarning(war);
            }
        }
    }


    return &app;

}
