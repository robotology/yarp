/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#include "mainwindow.h"
#include "log.h"
#include "yarpmotorgui.h"
#include "startdlg.h"
#include "sequencewindow.h"

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRobotDescription.h>
#include <yarp/dev/Drivers.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


QStringList partsName;
MainWindow* mainW = NULL;

bool debug_param_enabled     = false;
bool speedview_param_enabled = false;
bool enable_calib_all        = false;

static void sighandler(int sig)
{
    Q_UNUSED(sig);
    yDebug("\nCAUGHT Ctrl-c\n");// << "\nCAUGHT Ctrl-c" << endl;
    if(mainW)
    {
        mainW->term();
    }
}

int main(int argc, char *argv[])
{
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("auto"));


    Network yarp;
    if (!yarp.checkNetwork())
    {
        LOG_ERROR("Error initializing yarp network (is yarpserver running?)\n");
        QMessageBox::critical(0,"Error","Error initializing yarp network (is yarpserver running?)");
        return 1;
    }
    
    bool           ret;
    int            appRet;
    QApplication   a(argc, argv);
    ResourceFinder finder;
    Bottle         pParts;
    QStringList    enabledParts;
    vector<bool>   enabled;
    MainWindow     w;

    //retrieve information for the list of parts
    finder.setVerbose();
    finder.setDefaultConfigFile("yarpmotorgui.ini");
    finder.setDefault("name", "icub");
    finder.configure(argc,argv);

    qRegisterMetaType<Pid>("Pid");
    qRegisterMetaType<SequenceItem>("SequenceItem");
    qRegisterMetaType<QList<SequenceItem> >("QList<SequenceItem>");

    if (finder.check("help"))
    {
        yInfo("yarpmotorgui options:");
        yInfo("--robot <name>: name of the robot");
        yInfo("--parts ""( <name1> <name2> )"": parts of the robot to add to the list.");
        yInfo("--skip_parts ""( <name1> <name2> )"": parts of the robot to skip.");
        yInfo("--calib to enable calibration buttons (be careful!)");
        return 0;
    }

    if (finder.check("calib"))
    {
        LOG("Calibrate buttons on\n");
        enable_calib_all = true;
    }

    if (finder.check("admin"))
    {
        LOG("Admin mode on.\n");
        enable_calib_all      = true;
    }

    if (finder.check("debug"))
    {
        LOG("Debug interface requested.\n");
    }

    if (finder.check("speed"))
    {
        LOG("Speed view requested.\n");
        speedview_param_enabled = true;
    }

    //ask the robot part to the description server
    int count = 0;
    std::string descLocalName = "/yarpmotorgui" + std::to_string(count) + "/descriptionClient";
    Contact adr = Network::queryName(descLocalName);
    while (adr.isValid())
    {
        count++;
        descLocalName = "/yarpmotorgui" + std::to_string(count) + "/descriptionClient";
        adr = Network::queryName(descLocalName);
    }

    PolyDriver* desc_driver = 0;
    desc_driver = new PolyDriver;
    std::vector<DeviceDescription> cbw2_list;
    Property desc_driver_options;
    desc_driver_options.put("device", "robotDescriptionClient");
    desc_driver_options.put("local", descLocalName);
    desc_driver_options.put("remote", "/robotDescription");
    desc_driver->open(desc_driver_options);
    if (desc_driver && desc_driver->isValid())
    {
        IRobotDescription* idesc = 0;
        desc_driver->view(idesc);
        if (idesc)
        {
            idesc->getAllDevicesByType("controlboardwrapper2", cbw2_list);
            std::vector<DeviceDescription> wrappers_list;
            wrappers_list.reserve(cbw2_list.size());
            wrappers_list.insert(wrappers_list.end(), cbw2_list.begin(), cbw2_list.end());
            for (size_t i = 0; i < wrappers_list.size(); i++)
            {
                yDebug() << wrappers_list[i].device_name;
                pParts.addString(wrappers_list[i].device_name);
            }
        }
    }

    std::string robotName = finder.find("robot").asString();
    Bottle* b_part_skip = finder.find("skip_parts").asList();
    Bottle* b_part = finder.find("parts").asList();
    if (pParts.size() == 0)
    {
        if (robotName != "" && b_part != 0)
        {
            //check parts from config file
            for (int i = 0; i < b_part->size(); i++)
            {
                string ss = b_part->get(i).asString();
                if (ss.at(0) != '/')
                {
                    ss = "/" + robotName + "/" + ss;
                }
                pParts.addString(ss);
            }
        }
        else
        {
            //use default names
            pParts = Bottle("/icub/head /icub/torso /icub/left_arm /icub/right_arm /icub/left_leg /icub/right_leg");
        }
    }
    
    //Check 1 in the panel
    for(int n = 0; n < pParts.size(); n++)
    {
        QString part = QString("%1").arg(pParts.get(n).asString().c_str());
        if (b_part_skip)
        {
            if (b_part_skip->check(part.toStdString().c_str())) continue;
        }
        yDebug("Appending %s", part.toUtf8().constData());
        partsName.append(part);
    }

    if(!finder.check("skip"))
    {
        StartDlg dlg;
        dlg.init(partsName);

        if(dlg.exec() == QDialog::Accepted)
        {
            enabled      = dlg.getEnabledParts();
        }
        else
        {
            yInfo("Cancel Button pressed. Closing..");
            return 0;
        }
    }

    for(int i = 0; i < partsName.count(); i++)
    {
        if(enabled.at(i))
        {
            std::string debug_s2 = partsName.at(i).toStdString();
            enabledParts.append(partsName.at(i));
        }
    }

    yarp::os::signal(yarp::os::YARP_SIGINT,  sighandler);
    yarp::os::signal(yarp::os::YARP_SIGTERM, sighandler);

    mainW  = &w;
    appRet = 0;
    ret = w.init(enabledParts, finder, debug_param_enabled, speedview_param_enabled, enable_calib_all);

    if(ret)
    {
        w.show();
        appRet = a.exec();
    }

    return (appRet != 0 ? 1 : 0);
}
