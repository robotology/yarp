/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
#include <QtGlobal>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <csignal>

using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


QStringList partsName;
MainWindow* mainW = nullptr;

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("auto"));
#endif

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    if (!yarp.checkNetwork())
    {
        LOG_ERROR("Error initializing yarp network (is yarpserver running?)\n");
        QMessageBox::critical(nullptr, "Error", "Error initializing yarp network (is yarpserver running?)");
        return 1;
    }

    bool           ret;
    int            appRet;
    QApplication   a(argc, argv);
    ResourceFinder &finder = ResourceFinder::getResourceFinderSingleton();
    //retrieve information for the list of parts
    finder.setDefaultConfigFile("yarpmotorgui.ini");
    finder.configure(argc, argv);

    Bottle         pParts;
    QStringList    enabledParts;
    vector<bool>   enabled;
    MainWindow     w;

    qRegisterMetaType<Pid>("Pid");
    qRegisterMetaType<SequenceItem>("SequenceItem");
    qRegisterMetaType<QList<SequenceItem> >("QList<SequenceItem>");

    if (finder.check("help"))
    {
        yInfo("yarpmotorgui options:");
        yInfo("--robot <name>: name of the robot");
        yInfo("--parts ""( <name1> <name2> )"": parts of the robot to add to the list. (e.g. left_arm)");
        yInfo("--names ""( <name1> <name2> )"": full name of the ports of the robot to add to the list. (e.g. /icub/left_arm). This option is mutually exclusive with --robot --parts options");
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
        enable_calib_all = true;
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

    if (finder.check("skip_description_server")==false) //option --skip_description_server is for debug only, users should not use it
    {
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

        if (yarp::os::Network::exists("/robotDescription/rpc"))
        {
            PolyDriver* desc_driver = nullptr;
            desc_driver = new PolyDriver;
            std::vector<DeviceDescription> cbw2_list;
            Property desc_driver_options;
            desc_driver_options.put("device", "robotDescriptionClient");
            desc_driver_options.put("local", descLocalName);
            desc_driver_options.put("remote", "/robotDescription");
            if (desc_driver && desc_driver->open(desc_driver_options))
            {
                IRobotDescription* idesc = nullptr;
                desc_driver->view(idesc);
                if (idesc)
                {
                    idesc->getAllDevicesByType("controlboardwrapper2", cbw2_list);
                    std::vector<DeviceDescription> wrappers_list;
                    wrappers_list.reserve(cbw2_list.size());
                    wrappers_list.insert(wrappers_list.end(), cbw2_list.begin(), cbw2_list.end());
                    for (auto& i : wrappers_list)
                    {
                        yDebug() << i.device_name;
                        pParts.addString(i.device_name);
                    }
                }
            }

            if (desc_driver)
            {
                desc_driver->close();
                delete desc_driver;
            }
        }
        else
        {
            yWarning() << "robotDescriptionServer not found, robot parts will be set manually.";
        }
    }

    std::string robotName = finder.find("robot").asString();
    Bottle* b_part_skip = finder.find("skip_parts").asList();
    Bottle* b_part = finder.find("parts").asList();
    Bottle* b_name = finder.find("names").asList();    if (pParts.size() == 0)
    {
        if (robotName != "" && b_name != nullptr)
        {
            LOG_ERROR("You cannot use both --robot and --names options simultaneously\n");
            QMessageBox::critical(nullptr, "Error", "You cannot use both --robot and --names options simultaneously");
            return 1;
        }

        if (b_name != nullptr && b_part != nullptr)
        {
            LOG_ERROR("You cannot use both --parts and --names options simultaneously\n");
            QMessageBox::critical(nullptr, "Error", "You cannot use both --parts and --names options simultaneously");
            return 1;
        }

        if (b_name != nullptr && b_part == nullptr)
        {
            //check port names from config file
            for (size_t i = 0; i < b_name->size(); i++)
            {
                pParts.addString(b_name->get(i).asString());
            }
        }
        else if (robotName != "" && b_part != nullptr)
        {
            //check parts from config file
            for (size_t i = 0; i < b_part->size(); i++)
            {
                string ss = b_part->get(i).asString();
                if (ss.at(0) != '/')
                {
                    ss.insert(0, "/" + robotName + "/");
                }
                else
                {
                    LOG_ERROR("Option --parts should not contain /, please remove it\n");
                    QMessageBox::critical(nullptr, "Error", "Option --parts should not contain /, please remove it");
                    return 1;
                }
                pParts.addString(ss);
            }
        }
        else if (robotName != "" && b_part == nullptr)
        {
            pParts.addString("/" + robotName + "/head");
            pParts.addString("/" + robotName + "/torso");
            pParts.addString("/" + robotName + "/left_arm");
            pParts.addString("/" + robotName + "/right_arm");
            pParts.addString("/" + robotName + "/left_leg");
            pParts.addString("/" + robotName + "/right_leg");
        }
        else
        {
            //use default names
            pParts = Bottle("/icub/head /icub/torso /icub/left_arm /icub/right_arm /icub/left_leg /icub/right_leg");
        }
    }

    //Check 1 in the panel
    for(size_t n = 0; n < pParts.size(); n++)
    {
        QString part = QString("%1").arg(pParts.get(n).asString().c_str());
        if (b_part_skip)
        {
            if (b_part_skip->check(part.toStdString())) {
                continue;
            }
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

    std::signal(SIGINT, sighandler);
    std::signal(SIGTERM, sighandler);

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
