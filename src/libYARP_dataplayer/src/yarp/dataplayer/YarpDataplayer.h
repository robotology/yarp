/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DATAPLAYER
#define YARP_DATAPLAYER

#if defined(_WIN32)
#pragma warning (disable : 4099)
#pragma warning (disable : 4250)
#pragma warning (disable : 4520)
#endif

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/ImageFile.h>

#include <yarp/os/Thread.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>

#include <list>
#include <mutex>
#include <vector>
#include <string>
#include <ctime>

namespace yarp
{
namespace yarpDataplayer
{
class  DataplayerEngine;
class  DataplayerWorker;
class  DataplayerUtilities;

struct PartsData;
struct RowInfo;
}
}

struct yarp::yarpDataplayer::PartsData
{
    DataplayerWorker        *worker;            //personal rate thread
    std::mutex              mutex;              //mutex
    std::string             name;               //string containing the name of the part
    std::string             infoFile;           //string containing the path of the infoFile
    std::string             logFile;            //string containing the path of the logFile
    std::string             path;               //string containing the path of the part
    std::string             type;               //string containing the type of the data
    int                     currFrame;          //integer containing the current frame
    int                     maxFrame;           //integer containing the maxFrame
    yarp::os::Bottle        bot;                //yarp Bottle containing all the data
    yarp::sig::Vector       timestamp;          //yarp Vector containing all the timestamps
    yarp::os::Contactable*  outputPort;         //yarp port for sending out data
    std::string             portName;           //the name of the port
    int                     sent;               //integer used for step from command
    bool                    hasNotified;        //boolean used for individual part notification that it has reached eof

    PartsData() { outputPort = nullptr; worker = nullptr;}
};

struct yarp::yarpDataplayer::RowInfo
{
    std::string name;
    std::string info;
    std::string log;
    std::string path;
};

class yarp::yarpDataplayer::DataplayerUtilities
{
protected:
    int                 dir_count;      //integer containing the directory count
    std::string         moduleName;     //string containing module name
    bool                add_prefix;     //true if /<moduleName> must be added to every port opened
    yarp::sig::Vector   allTimeStamps;  //save all timestamps

public:
    DataplayerUtilities();
    DataplayerUtilities(std::string name, bool _add_prefix=false, bool _verbose=false);
    ~DataplayerUtilities();
    PartsData           *partDetails;
    double              speed;
    yarp::sig::Vector   initialFrame;
    bool                repeat;
    bool                sendStrict;
    int                 totalSent;
    int                 totalThreads;
    std::string         recursiveName;
    int                 recursiveIterations;

    DataplayerEngine    *dataplayerEngine;

    bool                withExtraColumn;
    int                 column;
    double              maxTimeStamp;   //get the max Time stamp
    double              minTimeStamp;
    bool                verbose;

    /**
    * function that returns the current path string
    */
    std::string getCurrentPath();
    /**
    *  function that returns a vector containing path directories - works in a recursive way
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
    bool setupDataFromParts(PartsData &part);
    /**
    * function that configures and opens all the ports required
    */
    bool configurePorts(PartsData &part);
    /**
    * function that interrupts all the opened ports
    */
    bool interruptPorts(PartsData &part);
    /**
    * function that closes all the opened ports
    */
    bool closePorts(PartsData &part);
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
    int amendPartFrames(PartsData &part);
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

    /**
    * function that sets the module name
    */
    void setModuleName(const std::string &name);

    /**
    * function that sets the add prefix
    */
    void addPrefix(const bool &prefix);

    /**
    * function that sets verbosity
    */
    void setVerbose(const bool &verbose);

};



class yarp::yarpDataplayer::DataplayerWorker
{
protected:
    DataplayerUtilities *utilities;

    void run();
    int part;
    int percentage;
    int numThreads;
    int currFrame;
    bool isActive;
    double frameRate, initTime, virtualTime;
    yarp::os::Semaphore semIndex;
    double startTime;

public:
    /**
    * Worker class that does the work of sending the data for each part
    */
    DataplayerWorker(int part, int numThread);
    /**
    * Function that sets the manager to utilities class
    */
    void setManager(DataplayerUtilities *utilities);
    /**
    * Functions that sends data (many different types)
    */
    int sendBottle(int part, int id);
    int sendImages( int part, int id);

    template <class T>
    int sendGenericData(int part, int id);

    /**
    * Function that returns the frame rate
    */
    double getFrameRate();
    /**
     * Function that returns the time taken
     */
    double getTimeTaken();
    /**
    * Function that pepares and "steps" the sending of the data
    */
    void sendData(int id, bool shouldSend, double virtualTime);
    /**
    * init
    */
    bool init();
    /**
    * release
    */
    void release();
    /**
    * Function that resets the time
    */
    void resetTime();

};

class yarp::yarpDataplayer::DataplayerEngine
{

protected:
    class dataplayer_thread : public yarp::os::PeriodicThread
    {
        DataplayerEngine *dataplayerEngine;
    public:
        void SetDataplayerEngine(DataplayerEngine &dataplayerEngine)
        { this->dataplayerEngine = &dataplayerEngine; }
        dataplayer_thread (double _period=0.002);

        bool        threadInit() override;
        void        run() override;
        void        threadRelease() override;

    } *dataplayer_updater;

    DataplayerUtilities *utilities;

public:

    int                     numThreads;
    double                  timePassed, initTime, virtualTime;
    double                  pauseStart, pauseEnd;
    bool                    stepfromCmd;
    int                     numPart;
    bool                    allPartsStatus;
    std::vector<bool>       isPartActive;

    using Moment = std::chrono::time_point<std::chrono::high_resolution_clock>;

    //static void initialize();

    void tick();

    float diff_seconds() const { return dtSeconds; }
    float framesPerSecond() const { return fps; }

    DataplayerEngine    ();
    DataplayerEngine    (DataplayerUtilities *utilities);
    DataplayerEngine    (DataplayerUtilities *utilities, int numPart);
    ~DataplayerEngine   ();

    /**
     * Function that sets the numPart
     */
    bool setNumPart(int numPart);

    /**
     * Function that steps forwards the data set
     */
    void forward(int steps);
    /**
     * Function that steps backwards the data set
     */
    void backward(int steps);
    /**
     * Function that pauses data set
     */
    void pause();
    /**
     * Function that resumes the data set
     */
    void resume();
    /**
     * Function that steps from command rpc
     */
    void stepFromCmd();
    /**
      * Function that steps normally (without using terminal or rpc)
    */
    void runNormally();

    void setPart(const int partID, const bool on) { isPartActive[partID] = on; }

    bool getAllPartsStatus() { return allPartsStatus; }
    void setAllPartsStatus(bool status) { allPartsStatus = status; }

    void setUtilities(yarp::yarpDataplayer::DataplayerUtilities *utilities);
    void setThread(yarp::yarpDataplayer::DataplayerEngine::dataplayer_thread *dataplayer_updater);

    void goToPercentage(int value);

    bool initThread(){ return dataplayer_updater->threadInit(); }
    void runThread(){ return dataplayer_updater->run(); }
    void releaseThread(){ return dataplayer_updater->threadRelease(); }

    bool isSuspended(){ return dataplayer_updater->isSuspended(); }
    void stop(){ return dataplayer_updater->stop(); }
    bool isRunning(){ return dataplayer_updater->isRunning(); }
    bool start(){ return dataplayer_updater->start(); }
    void askToStop() {return dataplayer_updater->askToStop(); }

protected:
    Moment lastUpdate;
    float dtSeconds, fps;
};
#endif
