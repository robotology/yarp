/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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
class MasterThread : public QObject
{
    friend class QUtilities;

protected:
    QUtilities *qutilities;
    

public:
  
    QMainWindow* wnd;

    /**
     * Master thread class
     */
    MasterThread(QUtilities *qutilities, int numPart, QMainWindow *gui, QObject *parent = NULL);
    
    bool init();
    
    void tick();
    void stepFromCmd();
    void runNormally();
    void goToPercentage(int value);
    void run();
    
    void forward(int steps);
    void backward(int steps);
    
    void pause();
    void resume();
    void release();
};

#endif
