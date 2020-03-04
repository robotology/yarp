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

class WorkerClass;
class MasterThread;
class QMainWindow;

struct partsData
{
    WorkerClass             *worker;                            //personal rate thread
    std::mutex              mutex;                              //mutex
    std::string             name;                               //string containing the name of the part
    std::string             infoFile;                           //string containing the path of the infoFile
    std::string             logFile;                            //string containing the path of the logFile
    std::string             path;                               //string containing the path of the part
    std::string             type;                               //string containing the type of the data
    int                     currFrame;                          //integer containing the current frame
    int                     maxFrame;                           //integer containing the maxFrame
    yarp::os::Bottle        bot;                                //yarp Bottle containing all the data
    yarp::sig::Vector       timestamp;                          //yarp Vector containing all the timestamps
    yarp::os::Contactable*  outputPort;                         //yarp port for sending out data
    std::string             portName;                           //the name of the port
    int                     sent;                               //integer used for step from command
    bool                    hasNotified;                        //boolean used for individual part notification that it has reached eof

    partsData() { outputPort = nullptr; worker = nullptr;}
};

struct RowInfo {
    std::string name;
    std::string info;
    std::string log;
    std::string path;
};

/**********************************************************/
class Utilities : public QObject
{
    Q_OBJECT

protected:
    int                             dir_count;      //integer containing the directory count
    yarp::os::ResourceFinder        rf;             //resource finder
    std::string                     moduleName;     //string containing module name
    bool                            add_prefix;     //true if /<moduleName> must be added to every port opened
    yarp::sig::Vector               allTimeStamps;  //save all timestamps

public:
    Utilities(std::string name, bool _add_prefix=false, QObject *parent = NULL);
    ~Utilities();

    partsData           *partDetails;
    double              speed;
    yarp::sig::Vector   initialFrame;
    bool                repeat;
    bool                sendStrict;
    int                 totalSent;
    int                 totalThreads;
    std::string         recursiveName;
    int                 recursiveIterations;

    MasterThread        *masterThread;
    QMainWindow         *wnd;

    bool                withExtraColumn;
    int                 column;
    double              maxTimeStamp;   //get the max Time stamp
    double              minTimeStamp;


    /**
    * function that returns the current path string
    */
    std::string getCurrentPath();
    /**
    * function that returns a vector containing path directories - works in a recursive way
    */
    int getRecSubDirList(const std::string& dir, std::vector<RowInfo>& rowInfoVec, int recursive);
    /**
    * function that checks validity of log files
    */
    bool checkLogValidity (const char * filename);
    /**
    * function that resets the directory count
    */
    void resetDirCount();
    /**
    * function that loads and returns the data from each part
    */
    bool setupDataFromParts(partsData &part);
    /**
    * function that configures and opens all the ports required
    */
    bool configurePorts(partsData &part);
    /**
    * function that interrupts all the opened ports
    */
    bool interruptPorts(partsData &part);
    /**
    * function that closes all the opened ports
    */
    bool closePorts(partsData &part);
    /**
    * function that clears all previously saved maximum timestamp
    */
    void resetMaxTimeStamp();
    /**
    * function that gets the maximum timestamp of all parts
    */
    void getMaxTimeStamp();
    /**
     * function that gets the minimum timestamp of all parts
     */
    void getMinTimeStamp();
    /**
    * function that amends the first frame of all parts
    */
    int amendPartFrames(partsData &part);
    /**
    * function that stops the main thread
    */
    void stopAtEnd();
    /**
    * function that gets input from the rpc port
    */
    bool execReq(const yarp::os::Bottle &command, yarp::os::Bottle &reply);
    /**
    * function that starts the main thread for cmd step
    */
    void stepThread();
    /**
    * function that pauses the main thread for cmd step
    */
    void pauseThread();

signals:
    void updateGuiThread();
    void pause();
    void errorMessage(QString);

};

#endif
