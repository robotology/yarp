/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/ymm-dir.h>

#ifdef WITH_GEOMETRY
#include <yarp/os/Property.h> // for parsing geometry information
#endif

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include <tinyxml.h>


using namespace std;
using namespace yarp::manager;


/**
 * if szAppName is NULL, XmlAppLoader will load all the applications
 * found in szPath otherwise only one application named szAppname
 * will be loaded.
 */
XmlAppLoader::XmlAppLoader(const char* szPath, const char* szAppName)
{
    app.clear();
    if(szAppName)
        strAppName = szAppName;

    if(strlen(szPath))
    {
        strPath = szPath;
        if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);
    }
}

/**
 * load only one application indicated by its xml file name
 */
XmlAppLoader::XmlAppLoader(const char* szFileName)
{
    app.clear();
    if(szFileName)
        strFileName = szFileName;
}


XmlAppLoader::~XmlAppLoader()
{
}


bool XmlAppLoader::init(void)
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
    if ((dir = opendir(strPath.c_str())) == NULL)
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

void XmlAppLoader::reset(void)
{
    fini();
    init();
}


void XmlAppLoader::fini(void)
{
    fileNames.clear();
    app.clear();
}


Application* XmlAppLoader::getNextApplication(void)
{
    if(strAppName.empty())
    {
        Application* app = NULL;
        while(!app)
        {
            if(fileNames.empty())
                return NULL;
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
 return NULL;
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
        return NULL;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        OSTRINGSTREAM err;
        err<<"Syntax error while loading "<<szFile<<" . ";
        err<<"No root element.";
        logger->addError(err);
        return NULL;
    }

    if(!compareString(root->Value(), "application"))
    {
        //OSTRINGSTREAM err;
        //err<<"File "<<szFile<<" has no tag <application>.";
        //logger->addError(err);
        return NULL;
    }

    /* retrieving name */
    TiXmlElement* name = (TiXmlElement*) root->FirstChild("name");
    if(!name || !name->GetText())
    {
        OSTRINGSTREAM err;
        err<<"Module from "<<szFile<<" has no name.";
        logger->addError(err);
        //return NULL;
    }

    app.setXmlFile(szFile);

    if(name)
    {
        string strname = name->GetText();
        for(unsigned int i=0; i<strname.size(); i++)
            if(strname[i] == ' ')
                strname[i] = '_';
        app.setName(strname.c_str());
    }

    /* retrieving description */
    TiXmlElement* desc;
    if((desc = (TiXmlElement*) root->FirstChild("description")))
        app.setDescription(desc->GetText());

    /* retrieving version */
    TiXmlElement* ver;
    if((ver = (TiXmlElement*) root->FirstChild("version")))
        app.setVersion(ver->GetText());

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
                    author.setName(ath->GetText());
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
                    ResYarpPort resource(res->GetText());
                    resource.setPort(res->GetText());
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
    for(TiXmlElement* mod = root->FirstChildElement(); mod;
            mod = mod->NextSiblingElement())
    {
        if(compareString(mod->Value(), "module"))
        {
            TiXmlElement* element;
            if((element = (TiXmlElement*) mod->FirstChild("name")))
            {
                ModuleInterface module(element->GetText());
                if((element = (TiXmlElement*) mod->FirstChild("node")))
                    module.setHost(element->GetText());

                if((element = (TiXmlElement*) mod->FirstChild("parameters")))
                    module.setParam(element->GetText());

                if((element = (TiXmlElement*) mod->FirstChild("stdio")))
                    module.setStdio(element->GetText());

                if((element = (TiXmlElement*) mod->FirstChild("workdir")))
                    module.setWorkDir(element->GetText());

                if((element = (TiXmlElement*) mod->FirstChild("deployer")))
                    module.setBroker(element->GetText());
                if((element = (TiXmlElement*) mod->FirstChild("prefix")))
                    module.setPrefix(element->GetText());

                if((element = (TiXmlElement*) mod->FirstChild("rank")))
                    module.setRank(atoi(element->GetText()));

#ifdef WITH_GEOMETRY
                element = (TiXmlElement*) mod->FirstChild("geometry");
                if(element && element->GetText())
                {
                    yarp::os::Property prop(element->GetText());
                    GraphicModel model;
                    GyPoint pt;
                    if(prop.check("Pos"))
                    {
                        pt.x = prop.findGroup("Pos").find("x").asDouble();
                        pt.y = prop.findGroup("Pos").find("y").asDouble();
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
                                ResYarpPort resource(res->GetText());
                                resource.setPort(res->GetText());
                                if(res->Attribute("timeout"))
                                    resource.setTimeout(atof(res->Attribute("timeout")));
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
                            Portmap portmap(first->GetText(), second->GetText());
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
                ApplicationInterface IApp(name->GetText());
                if((prefix=(TiXmlElement*) embApp->FirstChild("prefix")))
                    IApp.setPrefix(prefix->GetText());
#ifdef WITH_GEOMETRY
                TiXmlElement* element = (TiXmlElement*) embApp->FirstChild("geometry");
                if(element && element->GetText())
                {
                    yarp::os::Property prop(element->GetText());
                    GraphicModel model;
                    GyPoint pt;
                    if(prop.check("Pos"))
                    {
                        pt.x = prop.findGroup("Pos").find("x").asDouble();
                        pt.y = prop.findGroup("Pos").find("y").asDouble();
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
            TiXmlElement* port = (TiXmlElement*) arb->FirstChild("port");
            if(port && port->GetText())
            {
                Arbitrator arbitrator(port->GetText());

                // retrieving rules
                for(TiXmlElement* rule = arb->FirstChildElement(); rule;
                    rule = rule->NextSiblingElement())
                {
                    if(compareString(rule->Value(), "rule"))
                    {
                        if(rule->Attribute("connection"))
                            arbitrator.addRule(rule->Attribute("connection"), rule->GetText());
                    }
                }
#ifdef WITH_GEOMETRY
                TiXmlElement* geometry = (TiXmlElement*) arb->FirstChild("geometry");
                if(geometry && geometry->GetText())
                {
                    yarp::os::Property prop(geometry->GetText());
                    GraphicModel model;
                    if(prop.check("Pos"))
                    {
                        yarp::os::Bottle pos = prop.findGroup("Pos");
                        for(int i=1; i<pos.size(); i++)
                        {
                            GyPoint pt;
                            pt.x = pos.get(i).find("x").asDouble();
                            pt.y = pos.get(i).find("y").asDouble();
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
            TiXmlElement* from = (TiXmlElement*) cnn->FirstChild("from");
            TiXmlElement* to = (TiXmlElement*) cnn->FirstChild("to");

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
                    strCarrier = protocol->GetText();
                Connection connection(from->GetText(),
                                    to->GetText(),
                                    strCarrier.c_str());
                if(from->Attribute("external") &&
                    compareString(from->Attribute("external"), "true"))
                {
                    connection.setFromExternal(true);
                    if(from->GetText())
                    {
                        ResYarpPort resource(from->GetText());
                        resource.setPort(from->GetText());
                        app.addResource(resource);
                    }
                }
                if(to->Attribute("external") &&
                    compareString(to->Attribute("external"), "true"))
                {
                    if(to->GetText())
                    {
                        connection.setToExternal(true);
                        ResYarpPort resource(to->GetText());
                        resource.setPort(to->GetText());
                        app.addResource(resource);
                    }
                }

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
                TiXmlElement* geometry = (TiXmlElement*) cnn->FirstChild("geometry");
                if(geometry && geometry->GetText())
                {
                    yarp::os::Property prop(geometry->GetText());
                    GraphicModel model;
                    if(prop.check("Pos"))
                    {
                        yarp::os::Bottle pos = prop.findGroup("Pos");
                        for(int i=1; i<pos.size(); i++)
                        {
                            GyPoint pt;
                            pt.x = pos.get(i).find("x").asDouble();
                            pt.y = pos.get(i).find("y").asDouble();
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
