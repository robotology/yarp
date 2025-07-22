/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "mainwindow.h"
#include <QApplication>

#include <yarp/os/Property.h>
#include <yarp/os/Network.h>

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    QApplication a(argc, argv);
    MainWindow *window = nullptr;

    yarp::os::Property options;
    options.fromCommand(argc,argv);

    if (options.check("help"))
    {
        yInfo("usage:");
        yInfo("yarpframegrabbergui --local <localportname> --remote <grabberport> [--width <gui width>] [--height <gui height>] [--x <gui x pos>] [--y <gui y pos>]");
        return 0;
    }

    yDebug("%s\n",options.toString().c_str());

    // switch to subsections if available
    yarp::os::Searchable *Network = &options.findGroup("NETWORK");
    yarp::os::Searchable *Window = &options.findGroup("WINDOW");

    if (Network->isNull()) { Network = &options; }
    if (Window->isNull()) { Window = &options; }

    yarp::os::Value *val =nullptr;

    std::string localPortName = "/yarpframegrabbergui/rpc";
    std::string remotePortName;
    if (Network->check("local",val)){
        localPortName = val->asString();
    }
    if (Network->check("remote",val)){
        remotePortName = val->asString();
    }

    //yDebug("using local=%s remote=%s x=%d y=%d\n",portName,outPortName,posX,posY);

    window = new MainWindow(localPortName, remotePortName);
    int posX=0,posY=0;
    int width,height;
    width = window->width();
    height = window->height();

    if (Window->check("PosX",val)||Window->check("x",val)){
        posX = val->asInt32();
    }
    if (Window->check("PosY",val)||Window->check("y",val)){
            posY = val->asInt32();
    }
    if (Window->check("width",val)){
        width = val->asInt32();
    }
    if (Window->check("height",val)){
        height = val->asInt32();
    }

    window->resize(width,height);
    window->move(posX,posY);
    window->show();

     int ret = a.exec();
     delete window;

     return (ret!=0?1:0);
}
