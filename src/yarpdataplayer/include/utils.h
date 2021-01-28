/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
