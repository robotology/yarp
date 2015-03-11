/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori
 * email:  francesco.nori@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


#include "mainwindow.h"
#include <QApplication>
#include "log.h"
#include "yarpmotorgui.h"
#include "startdlg.h"
#include "sequencewindow.h"
#include <QDebug>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>

using namespace yarp::dev;
using namespace yarp::os;

//YARP_DECLARE_DEVICES(icubmod)
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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //YARP_REGISTER_DEVICES(icubmod)


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
    LOG("%s\n",robotName.data());

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
        qDebug() << "APPENDING " << part;
        partsName.append(part);

        if(n < ENA.count()){
            ENA.replace(n,1);
        }else{
            ENA.append(1);
        }

        //partsName[n] = new char[80];
        //*ENA[n] = 1;
        //LOG("Getting part %d \n", n);
        //strcpy(partsName[n], pParts->get(n).asString().c_str());
        //LOG_ERROR("%s \n", partsName[n]);
        /*
        GtkWidget *check= gtk_check_button_new_with_mnemonic (partsName[n]);
        gtk_fixed_put   (GTK_FIXED(inv1), check, 100*n, 0);
        gtk_widget_set_size_request     (check, 80, 50);
        gtk_toggle_button_set_active((GtkToggleButton*) check, true);
        g_signal_connect (check, "clicked", G_CALLBACK (check_pressed),ENA[n]);*/
    }

    QString newRobotName;
    StartDlg dlg;
    dlg.init(QString(robotName.data()),partsName,ENA);
    if(dlg.exec() == QDialog::Accepted){
        ENA.clear();
        ENA = dlg.getEnabledParts();
        newRobotName = dlg.getRobotName();
    }else{
        return 0;
    }


    QStringList enabledParts;
    for(int i=0; i<partsName.count();i++){
        if(ENA.at(i) == 1){
            enabledParts.append(partsName.at(i));
        }
    }

    MainWindow w;
    w.init(newRobotName,enabledParts,finder,debug_param_enabled,speedview_param_enabled,enable_calib_all,position_direct_enabled,openloop_enabled);
    w.show();
    return a.exec();

    delete finder;
    delete pParts;
}
