/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <yarp/os/ResourceFinder.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/BufferedPort.h>
#include <mutex>
#include <yarp/sig/Image.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>
#include "include/worker.h"
#include <yarp/dataplayer/YarpDataplayer.h>

class QMainWindow;
class QEngine;

/**********************************************************/
class QUtilities : public QObject, public yarp::yarpDataplayer::DataplayerUtilities
{
    Q_OBJECT

public:
    QUtilities(QObject *parent = NULL);
    ~QUtilities();

    QMainWindow         *wnd;
    QEngine             *qengine;

    void stepThread();

    void resetButton();

signals:
    void updateGuiThread();
    void pause();
    void errorMessage(QString);

};
#endif
