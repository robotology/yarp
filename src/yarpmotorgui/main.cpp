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
#include <yarp/dev/Drivers.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <yarp/os/Log.h>

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

    bool           deleteParts, ret;
    int            appRet;
    QApplication   a(argc, argv);
    ResourceFinder finder;
    string         robotName;
    QString        newRobotName;
    Bottle*        pParts;
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

    deleteParts = false;
    robotName   = finder.find("name").asString();
    pParts      = finder.find("parts").asList();

    yDebug() << "Robot name: %s\n" << robotName;

    if (pParts == NULL)
    {
        yInfo() << "Setting default parts.\n";

        pParts      = new Bottle("head torso left_arm right_arm left_leg right_leg");
        deleteParts = true;
    }



    if(pParts->size() < 1)
    {
        LOG_ERROR("Invalid number of parts, check config file \n");
        return 1;
    }

    //Check 1 in the panel
    for(int n = 0; n < pParts->size(); n++)
    {
        QString part = QString("%1").arg(pParts->get(n).asString().c_str());
        yDebug("Appending %s",part.toUtf8().constData());
        partsName.append(part);
    }

    newRobotName = robotName.c_str();

    if(!finder.check("skip"))
    {
        StartDlg dlg;
        dlg.init(newRobotName, partsName);

        if(dlg.exec() == QDialog::Accepted)
        {
            enabled      = dlg.getEnabledParts();
            newRobotName = dlg.getRobotName();
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
            enabledParts.append(partsName.at(i));
        }
    }

    yarp::os::signal(yarp::os::YARP_SIGINT,  sighandler);
    yarp::os::signal(yarp::os::YARP_SIGTERM, sighandler);

    mainW  = &w;
    appRet = 0;
    ret    = w.init(newRobotName, enabledParts, finder, debug_param_enabled, speedview_param_enabled, enable_calib_all);

    if(ret)
    {
        w.show();
        appRet = a.exec();
    }

    if(deleteParts)
    {
        delete pParts;
    }

    return (appRet != 0 ? 1 : 0);
}
