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


/**
 *
 * @ingroup icub_tools
 * @ingroup icub_guis
 * \defgroup icub_robotMotorGui robotMotorGui
 *
 * A simple graphical interface for moving all
 * the joints of the iCub robot with sliders.
 * Uses remote control boards.
 *
 * \image html robotMotorGui.jpg
 * \image latex robotMotorGui.eps "A window of robotMotorGui running on Linux" width=15cm
 *
 * \section intro_sec Description
 *
 * This GUI can be used for the following pouposes:
 *
 * - continuosly reading the position of the ALL the robot joints
 * - running/idling single joints
 * - running ALL the robot joints
 * - position command of single joints
 * - changing the velocity of the position commands
 * - performing sequences of position commands with the ALL robot joints
 * - calibrating single joints
 * - checking if the robot is in position ("@")
 *
 * \section parameters_sec Parameters
 *
 * \code
 * --name: name of the robot (used to form port names)
 * --parts: a list of parts to be added.
 * --debug: opens the debugInterfaceClient (for firmware debugging).
 * --speed: enables the speed visualisation.
 * \endcode
 * Example:
 * \code
 * robotMotorGui --name icub --parts (head torso left_arm right_arm left_leg right_leg)
 * \endcode
 *
 * These parameters can be specified in a single file, passed with the
 * --from option.
 * Example:
 * \code
 * robotMotorGui --from robotMotorGui.ini
 * \endcode
 *
 * By default robotMotorGui starts using the file robotMotorGui.ini
 * in $ICUB_ROOT/app/default.
 *
 * An home position can be optionally defined in the supplied file.
 * This home position is specified using a group [part_zero] (e.g. [head_zero])
 * containing the home position and velocity  that will be commanded when the home
 * button is pressed:
 * \code
 * [head_zero]
 * PositionZero      0.0        0.0      0.0       0.0        0.0        0.0
 * VelocityZero     10.0       10.0     10.0      10.0       10.0       10.0
 * \endcode
 *
 * A set of calibration parameters can be optionally defined in the
 * supplied file. These calibration parameters follow the same standard
 * followed by the \ref icub_iCubInterface and can be specified within
 * the group [part_calib] (e.g. [head_calib]):
 * \code
 * [head_calib]
 * CalibrationType     0          0        0         0          0          0
 * Calibration1    500.0      1000.0    900.0     300.0     1333.0     1333.0
 * Calibration2     20.0        20.0     20.0     -20.0        5.0        5.0
 * Calibration3      0.0         0.0      0.0       0.0        0.0        0.0
 * \endcode
 *
 * A set of parameters can be optionally specified in order to
 * open a set of tabs which allow cartesian movements trough the cartesian
 * interfaces (see the tutorial \ref icub_cartesian_interface). These cartesian
 * interfaces can be enabled by inserting a group [cartesian] in the
 * robotMotorGui intialization file. This group should contain the name
 * of the robot parts which should be controlled in the cartesian space:
 * \code
 * [cartesian]
 * left_arm
 * ...
 * right_leg
 * \endcode
 * Each part initialized with the cartesian interface should be properly
 * configured by specifying the limits for the cartesian workspace.
 * \code
 * [left_arm_workspace]
 * xmin xm
 * xmax xM
 *
 * ymin ym
 * ymax yM
 *
 * zmin zm
 * zmax zM
 * \endcode
 * In order to make the cartesian tabs working you need to be sure that
 * that the \ref iKinCartesianSolver "Cartesian Solvers" are running and working.
 *
 * \section portsa_sec Ports Accessed
 * For each part initalized (e.g. right_leg):
 * - /icub/right_leg/rpc:i
 * - /icub/right_leg/command:i
 * - /icub/gui/right_leg/state:i
 *
 * \section portsc_sec Ports Created
 * For each part initalized (e.g. right_leg):
 * - /icub/gui/right_leg/rpc:o
 * - /icub/gui/right_leg/command:o
 * - /icub/right_leg/state:o
 *
 * \section conf_file_sec Configuration Files
 *
 * Passed with the paremter --from, configure the layout of the gui.
 * \code
 * name icub
 * parts (head torso right_arm left_arm)
 * \endcode
 *
 * Creates a gui. Connects automatically to:
 *
 * \code
 * /icub/head/*
 * /icub/torso/*
 * /icub/right_arm/*
 * ...
 * \endcode
 *
 * \section tested_os_sec Tested OS
 * Linux and Windows.
 *
 * \author Francesco Nori
 *
 *Copyright (C) 2008 RobotCub Consortium
 *
 *CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 *This file can be edited at src/gui/robotMotorGui/src/main.cpp.
 **/

#include "mainwindow.h"
#include <QApplication>
#include "log.h"
#include "robotMotorGui.h"
#include "startdlg.h"
#include "sequencewindow.h"
#include <qDebug>
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
    finder->setDefaultConfigFile("robotMotorGui.ini");
    finder->setDefault("name", "icub");
    finder->configure(argc,argv);

    qRegisterMetaType<Pid>("Pid");
    qRegisterMetaType<SequenceItem>("SequenceItem");
    qRegisterMetaType<QList<SequenceItem>>("QList<SequenceItem>");


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
