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

#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/ImageFile.h>
#include "include/utils.h"
#include <yarp/os/Event.h>
#include <yarp/os/Time.h>
#include <QMainWindow>
#include <yarp/dataplayer/YarpDataplayer.h>

#include <chrono>

#ifdef HAS_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yarp/cv/Cv.h>
#endif

class QUtilities;
//class MainWindow;

/**********************************************************/
class QEngine : public QObject, public yarp::yarpDataplayer::DataplayerEngine
{

public:
    QUtilities *qutils;
    QMainWindow* gui;

    /**
     * QEngine class
     */
    QEngine(QUtilities *qutilities, int subDirCnt, QMainWindow *gui, QObject *parent = NULL);
    ~QEngine();

    class QMasterThread : public yarp::yarpDataplayer::DataplayerEngine::dataplayer_thread
    {
        QEngine *qEngine;
        public:
        void setEngine(QEngine &qEngine)
                    { this->qEngine = &qEngine; }
        void        run() override;
    } *thread;

    void stepFromCmd();
    void runNormally();

};

#endif
