/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2012 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Elena Ceseracciu, Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/xmlappsaver.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/ymm-dir.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>

#include <tinyxml.h>


using namespace std;
using namespace yarp::manager;


XmlAppSaver::XmlAppSaver(const char* szFileName)
{
    if(szFileName) strFileName = szFileName;
}

bool XmlAppSaver::save(Application* application)
{
    if(!strFileName.size())
         return serialXml(application, application->getXmlFile());

    return serialXml(application, strFileName.c_str());
}

XmlAppSaver::~XmlAppSaver(){}

bool XmlAppSaver::serialXml(Application* app, const char* szFile)
{

    // updating application xml file name
    app->setXmlFile(szFile);

    ErrorLogger* logger  = ErrorLogger::Instance();

    TiXmlDocument doc; //(szFile);
    TiXmlElement * root = new TiXmlElement("application");
    doc.LinkEndChild( root );
    TiXmlElement * appName = new TiXmlElement("name");  //are all these NEW ok?
    appName->LinkEndChild(new TiXmlText(app->getName()));
    root->LinkEndChild(appName);


    if (strcmp(app->getDescription(), ""))
    {
        TiXmlElement * desc= new TiXmlElement( "description");
        desc->LinkEndChild(new TiXmlText( app->getDescription()));
        root->LinkEndChild(desc);
    }

    if(strcmp (app->getVersion(), ""))
    {
        TiXmlElement * vers= new TiXmlElement( "version");
        vers->LinkEndChild(new TiXmlText( app->getVersion()));
        root->LinkEndChild(vers);

    }

    /*
     * TODO: setting prefix of the main application is inactivated.
     * Check this should be supported in future or not!
     */
    /*
    if(strcmp (app->getPrefix(), ""))
        {
        TiXmlElement * prefix= new TiXmlElement( "prefix");
        prefix->LinkEndChild(new TiXmlText( app->getPrefix()));
        root->LinkEndChild(prefix);

    }
    */

    if(app->authorCount()>0)
    {
        TiXmlElement *auths=new TiXmlElement("authors");
        for (int i=0; i<app->authorCount(); i++)
        {
            //app->getAuthorAt(i);
            TiXmlElement *auth=new TiXmlElement("author");
            auth->SetAttribute("email", app->getAuthorAt(i).getEmail());
            auth->LinkEndChild(new TiXmlText(app->getAuthorAt(i).getName()));
            auths->LinkEndChild(auth);
        }
        root->LinkEndChild(auths);
    }

    // iterate over modules
    {
        int nModules=app->imoduleCount();
        for (int modCt=0; modCt<nModules; ++modCt)
        {
            TiXmlElement * newMod = new TiXmlElement("module");
            root->LinkEndChild(newMod); //add module element
            ModuleInterface curMod=app->getImoduleAt(modCt);

            TiXmlElement *name = new TiXmlElement("name");
            name->LinkEndChild(new TiXmlText(curMod.getName()));
            newMod->LinkEndChild(name);

            TiXmlElement *parameters=new TiXmlElement("parameters");
            parameters->LinkEndChild(new TiXmlText(curMod.getParam()));
            newMod->LinkEndChild(parameters);

            TiXmlElement *node = new TiXmlElement("node");
            node->LinkEndChild(new TiXmlText(curMod.getHost())); //is host the same as node?
            newMod->LinkEndChild(node);

            TiXmlElement *prefix=new TiXmlElement("prefix");
            prefix->LinkEndChild(new TiXmlText(curMod.getPrefix()));
            newMod->LinkEndChild(prefix);

            if(strcmp(curMod.getStdio(), ""))
            {
                TiXmlElement *stdio=new TiXmlElement("stdio");
                stdio->LinkEndChild(new TiXmlText(curMod.getStdio()));
                newMod->LinkEndChild(stdio);
            }

            if(strcmp(curMod.getWorkDir(), ""))
            {
                TiXmlElement *workdir=new TiXmlElement("workdir");
                workdir->LinkEndChild(new TiXmlText(curMod.getWorkDir()));
                newMod->LinkEndChild(workdir);
            }

            if(strcmp(curMod.getBroker(), ""))
            {
                TiXmlElement *broker=new TiXmlElement("deployer");
                broker->LinkEndChild(new TiXmlText(curMod.getBroker()));
                newMod->LinkEndChild(broker);
            }
            /*
            if(curMod.getRank()>0) //TODO check here how is rank handled
            {
                TiXmlElement *rank=new TiXmlElement("rank");
                char str[256];
                sprintf(str,"%d", curMod.getRank());
                rank->LinkEndChild(new TiXmlText(str));
                newMod->LinkEndChild(rank);
            }
            */

            GraphicModel model = curMod.getModelBase();
            OSTRINGSTREAM txt;
            if(model.points.size()>0)
            {
                txt<<"(Pos (x "<<model.points[0].x<<") "<<"(y "<<model.points[0].y<<"))";
                TiXmlElement *geometry=new TiXmlElement("geometry");
                geometry->LinkEndChild(new TiXmlText(txt.str().c_str()));
                newMod->LinkEndChild(geometry);
            }
        }

    }

    // iterate over embedded applications
    {
        int nApps=app->iapplicationCount();
        for (int appCt=0; appCt<nApps; ++appCt)
        {
            TiXmlElement *newApp  = new TiXmlElement("application");
            root->LinkEndChild(newApp); //add application element
            ApplicationInterface curApp=app->getIapplicationAt(appCt);

            TiXmlElement *name = new TiXmlElement("name");
            name->LinkEndChild(new TiXmlText(curApp.getName()));
            newApp->LinkEndChild(name);


            TiXmlElement *prefix=new TiXmlElement("prefix");
            prefix->LinkEndChild(new TiXmlText(curApp.getPrefix()));
            newApp->LinkEndChild(prefix);

            GraphicModel model = curApp.getModelBase();
            OSTRINGSTREAM txt;
            if(model.points.size()>0)
            {
                txt<<"(Pos (x "<<model.points[0].x<<") "<<"(y "<<model.points[0].y<<"))";
                TiXmlElement *geometry=new TiXmlElement("geometry");
                geometry->LinkEndChild(new TiXmlText(txt.str().c_str()));
                newApp->LinkEndChild(geometry);
            }

        }
    }

    // iterate over connections
    {
        int nConns=app->connectionCount();
        for (int connCt=0; connCt<nConns; ++connCt)
        {
            TiXmlElement *newConn=new TiXmlElement("connection");
            Connection curConn=app->getConnectionAt(connCt);

            if(strlen(curConn.getId()))
                newConn->SetAttribute("id", curConn.getId());

            if(curConn.isPersistent())
                newConn->SetAttribute("persist", "true");

            TiXmlElement *from = new TiXmlElement("from");
            if (curConn.isExternalFrom())
                from->SetAttribute("external", "true");
            from->LinkEndChild(new TiXmlText(curConn.from()));
            newConn->LinkEndChild(from);

            TiXmlElement *to = new TiXmlElement("to");
            if (curConn.isExternalTo())
                to->SetAttribute("external", "true");
            to->LinkEndChild(new TiXmlText(curConn.to()));
            newConn->LinkEndChild(to);

            TiXmlElement *protocol = new TiXmlElement("protocol");
            protocol->LinkEndChild(new TiXmlText(curConn.carrier()));
            newConn->LinkEndChild(protocol);

            GraphicModel model = curConn.getModelBase();
            OSTRINGSTREAM txt;
            if(model.points.size()>0)
            {
                txt<<"(Pos ";
                for(unsigned int i=0; i<model.points.size(); i++)
                    txt<<"((x "<<model.points[i].x<<") "<<"(y "<<model.points[i].y<<")) ";
                txt<<" )";
                TiXmlElement *geometry=new TiXmlElement("geometry");
                geometry->LinkEndChild(new TiXmlText(txt.str().c_str()));
                newConn->LinkEndChild(geometry);
            }

            root->LinkEndChild(newConn);
        }

    }

    // iterate over arbitrators
        for(int i=0; i<app->arbitratorCount(); i++)
        {
            Arbitrator& arb = app->getArbitratorAt(i);
            TiXmlElement *newArb = new TiXmlElement("arbitrator");

            TiXmlElement *port = new TiXmlElement("port");
            port->LinkEndChild(new TiXmlText(arb.getPort()));
            newArb->LinkEndChild(port);

            std::map<string, string> &rules = arb.getRuleMap();
            for(std::map<string, string>::iterator it=rules.begin(); it!=rules.end(); ++it)
            {
                TiXmlElement *rule = new TiXmlElement("rule");
                rule->SetAttribute("connection", it->first.c_str());
                rule->LinkEndChild(new TiXmlText(it->second.c_str()));
                newArb->LinkEndChild(rule);
            }

            GraphicModel model = arb.getModelBase();
            OSTRINGSTREAM txt;
            if(model.points.size()>0)
            {
                txt<<"(Pos ";
                for(unsigned int i=0; i<model.points.size(); i++)
                    txt<<"((x "<<model.points[i].x<<") "<<"(y "<<model.points[i].y<<")) ";
                txt<<" )";
                TiXmlElement *geometry=new TiXmlElement("geometry");
                geometry->LinkEndChild(new TiXmlText(txt.str().c_str()));
                newArb->LinkEndChild(geometry);
            }
            root->LinkEndChild(newArb);
        }


    bool ok=doc.SaveFile(app->getXmlFile());
    if (!ok)
    {

        OSTRINGSTREAM err;
        err<<"tinyXml error for file " << app->getXmlFile();
        if (doc.Error())
            err <<" at line " << doc.ErrorRow() << ", column " << doc.ErrorCol() << ": " << doc.ErrorDesc();
        logger->addError(err);
        err <<"\n";
        return false;
    }
    else return true;

}
