/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Francesco Nori <francesco.nori@iit.it>
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
#include <yarp/os/Os.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <yarp/os/Log.h>

using namespace yarp::dev;
using namespace yarp::os;


QStringList partsName;
QList <int> ENA;
//char *partsName[];
//int *ENA[];
int NUMBER_OF_ACTIVATED_PARTS = 0;
int NUMBER_OF_ACTIVATED_CARTESIAN = 0;
int NUMBER_OF_AVAILABLE_PARTS = 0;
int PART;
bool debug_param_enabled = false;
bool speedview_param_enabled =false;
bool enable_calib_all =false;
bool position_direct_enabled = false;
bool openloop_enabled = false;
bool old_impedance_enabled = false;

MainWindow *mainW = NULL;

static void sighandler(int sig) {
    Q_UNUSED(sig);
    yDebug("\nCAUGHT Ctrl-c\n");// << "\nCAUGHT Ctrl-c" << endl;
    if(mainW){
        mainW->term();
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //YARP_REGISTER_DEVICES(icubmod)

    Network yarp;

    if (!yarp.checkNetwork())
    {
        LOG_ERROR("Error initializing yarp network (is yarpserver running?)\n");
        QMessageBox::critical(0,"Error","Error initializing yarp network (is yarpserver running?)");
        return 0;
    }

    ResourceFinder *finder;

    Property p, q;
    finder = new ResourceFinder;

    //retrieve information for the list of parts
    finder->setVerbose();
    finder->setDefaultConfigFile("yarpmotorgui.ini");
    finder->setDefault("name", "icub");
    finder->configure(argc,argv);

    qRegisterMetaType<Pid>("Pid");
    qRegisterMetaType<SequenceItem>("SequenceItem");
    qRegisterMetaType<QList<SequenceItem> >("QList<SequenceItem>");


    if (finder->check("calib")){
        LOG("Calibrate buttons on\n");
        enable_calib_all = true;
    }
    if (finder->check("admin")){
        LOG("Admin mode on.\n");
        enable_calib_all = true;
        debug_param_enabled = false;
        position_direct_enabled = true;
        openloop_enabled = true;
        old_impedance_enabled = true;
    }
    if (finder->check("debug")){
        LOG("Debug interface requested.\n");
        debug_param_enabled = false;
    }
    if (finder->check("speed")){
        LOG("Speed view requested.\n");
        speedview_param_enabled = true;
    }
    if (finder->check("direct")){
        LOG("Position direct requested.\n");
        position_direct_enabled = true;
    }
    if (finder->check("openloop")){
        LOG("Openloop requested.\n");
        openloop_enabled = true;
    }

    bool deleteParts=false;
    std::string robotName=finder->find("name").asString().c_str();
    yDebug("Robot name: %s\n",robotName.data());

    Bottle *pParts=finder->find("parts").asList();
    if (pParts==NULL){
        printf("Setting default parts.\n");
        pParts=new Bottle("head torso left_arm right_arm left_leg right_leg");
        deleteParts=true;
    }

    NUMBER_OF_AVAILABLE_PARTS=pParts->size();
    if (NUMBER_OF_AVAILABLE_PARTS > MAX_NUMBER_ACTIVATED){
        LOG_ERROR("The number of parts exceeds the maximum! \n");
        return 0;
    }
    if (NUMBER_OF_AVAILABLE_PARTS<=0){
        LOG_ERROR("Invalid number of parts, check config file \n");
        return 0;
    }

    for(int n=0; n < MAX_NUMBER_ACTIVATED; n++){
        //ENA = 0: part available
        //ENA = -1: part unavailable
        //ENA = 1: part used
        ENA.append(-1);
    }

    //Check 1 in the panel
    for(int n=0;n<NUMBER_OF_AVAILABLE_PARTS;n++){
        QString part = QString("%1").arg(pParts->get(n).asString().c_str());
        yDebug("Appending %s",part.toUtf8().constData());
        partsName.append(part);

        if(n < ENA.count()){
            ENA.replace(n,1);
        }else{
            ENA.append(1);
        }
    }

    QString newRobotName = robotName.data();

    if (!finder->check("fast"))
    {
        StartDlg dlg;
        dlg.init(QString(robotName.data()),partsName,ENA);
        if(dlg.exec() == QDialog::Accepted){
            ENA.clear();
            ENA = dlg.getEnabledParts();
            newRobotName = dlg.getRobotName();
        }else{
            yInfo("Cancel Button pressed. Closing..");
            return 0;
        }
    }

    QStringList enabledParts;
    for(int i=0; i<partsName.count();i++){
        if(ENA.at(i) == 1){
            enabledParts.append(partsName.at(i));
        }
    }

    yarp::os::signal(yarp::os::YARP_SIGINT, sighandler);
    yarp::os::signal(yarp::os::YARP_SIGTERM, sighandler);
    MainWindow w;
    mainW = &w;
    int appRet = 0;
    bool ret = w.init(newRobotName,enabledParts,finder,debug_param_enabled,speedview_param_enabled,enable_calib_all,position_direct_enabled,openloop_enabled);
    if(ret){
        w.show();
        appRet = a.exec();
    }

    delete finder;
    if(deleteParts){
        delete pParts;
    }

    return appRet;
}
