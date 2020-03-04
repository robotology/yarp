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

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <dirent.h>

#if defined(_WIN32)
    #undef max                  /*conflict with pango lib coverage.h*/
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #include <cerrno>
    #include <sys/stat.h>
    #define GetCurrentDir getcwd
#endif

#include <yarp/os/Time.h>
#include <cstdio>              /* defines FILENAME_MAX */
#include "include/utils.h"

#include <iostream>
#include <utility>
#include "include/mainwindow.h"
#include "include/log.h"

 //ROS messages
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

/**********************************************************/
Utilities::~Utilities()
{
    if(masterThread){
        //masterThread->stop();
        delete masterThread;
        masterThread = nullptr;
    }

    if(partDetails){
        for (int i=0; i < totalThreads; i++){
            delete partDetails[i].worker;
        }
        delete[] partDetails;
    }

}
/**********************************************************/
Utilities::Utilities(string name, bool _add_prefix, QObject *parent) : QObject(parent),
    dir_count(0),
    moduleName(std::move(name)),
    add_prefix(_add_prefix),
    partDetails(nullptr),
    speed(1.0),
    repeat(false),
    sendStrict(false),
    totalSent(0),
    totalThreads(0),
    recursiveIterations(0),
    masterThread(nullptr),
    wnd(nullptr),
    withExtraColumn(false),
    column(0),
    maxTimeStamp(0.0),
    minTimeStamp(0.0)
{
    connect(this,SIGNAL(updateGuiThread()),(MainWindow*)parent,
            SLOT(onUpdateGuiRateThread()),Qt::BlockingQueuedConnection);

    connect(this,SIGNAL(pause()),(MainWindow*)parent,
            SLOT(resetButtonOnStop()),Qt::BlockingQueuedConnection);

    connect(this,SIGNAL(errorMessage(QString)),(MainWindow*)parent,
            SLOT(onErrorMessage(QString)),Qt::QueuedConnection);
}
/**********************************************************/
string Utilities::getCurrentPath()
{
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath) )){// sizeof(TCHAR)))
        LOG("ERROR GETTING THE CURRENT DIR\n");
    }
    //cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    string currentPath = cCurrentPath;

    return currentPath;
}
/**********************************************************/
int Utilities::getRecSubDirList(const string& dir, vector<RowInfo>& rowInfoVec, int recursive)
{

    struct dirent *direntp = nullptr;
    DIR *dirp = nullptr;
    size_t path_len;
    const char *path = dir.c_str();

    /* Check validity of directory */
    if (!path){
        return errno;
    }
    /* Get length of the directory */
    path_len = strlen(path);
    /* Check length of the directory */
    if (!path || !path_len || (path_len > FILENAME_MAX)){
        return errno;
    }
    /* Check if file is opened */
    if((dirp  = opendir(path)) == nullptr){
        LOG("Error( %d opening %s\n",errno, dir.c_str());
        return errno;
    }
    /* read through */
    while ((direntp = readdir(dirp)) != nullptr){
        struct stat fstat;
        char full_name[FILENAME_MAX + 1];

        /* Calculate full name, check we are in file length limts */
        if ((path_len + strlen(direntp->d_name) + 1) > FILENAME_MAX){
            continue;
        }

        strcpy(full_name, path);
        if (full_name[path_len - 1] != '/'){
            strcat(full_name, "/");
        }
        strcat(full_name, direntp->d_name);

        /* Ignore special directories. */
        if ((strcmp(direntp->d_name, ".") == 0) ||
            (strcmp(direntp->d_name, "..") == 0)){
            continue;
        }

        /*Ignore if not really a directory. */
        if (stat(full_name, &fstat) < 0){
            continue;
        }
        if (S_ISDIR(fstat.st_mode)) {
            string recDir = full_name;
            struct stat st;
            string fullName = string(dir + "/" + direntp->d_name + "/info.log");
            const char * filename = fullName.c_str();
            if(stat(filename,&st) == 0) {
                string dataFileName = string(dir + "/" + direntp->d_name + "/data.log");

                bool checkLog = checkLogValidity( filename );
                bool checkData = checkLogValidity( dataFileName.c_str() );
                //check log file validity before proceeding
                if ( checkLog && checkData && (stat(dataFileName.c_str(), &st) == 0)) {
                    LOG(" %s IS present adding it to the gui\n",filename);
                    RowInfo row;

                    if (recursiveName.empty()){
                        row.name = direntp->d_name;//pass also previous subDir name
                    } else {
                        row.name = recursiveName + "_" + direntp->d_name;//pass only subDir name
                    }

                    row.info  = dir + "/" + direntp->d_name + "/info.log";
                    row.log   = dir + "/" + direntp->d_name + "/data.log";
                    row.path = dir + "/" + direntp->d_name + "/"; //pass full path
                    rowInfoVec.emplace_back(row);
                    dir_count++;
                } else {
                    if (!checkLog){
                        LOG(" %s IS present BUT corrupted not using file\n", filename);
                        //masterThread->wnd->onErrorMessage(filename);
                        errorMessage(QString("%1").arg(filename));
                    }
                    if (!checkData){
                        LOG(" %s IS present BUT corrupted not using file\n", dataFileName.c_str());
                        //wnd->onErrorMessage(dataFileName.c_str());
                        errorMessage(QString("%1").arg(dataFileName.c_str()));
                    }
                }
            }

            if (recursive){
                recursiveIterations ++;
                if (recursiveIterations > 1){
                    if (recursiveName.empty()){
                        recursiveName = string( direntp->d_name );
                    }
                } else {
                    recursiveName = string( direntp->d_name );
                }

                getRecSubDirList(recDir, rowInfoVec, 1);
            }
            if (recursiveIterations < 2 || recursiveIterations > 2){
                recursiveName.erase();
                recursiveIterations = 0;
            }
        }
    }
    /* close the dir */
    (void)closedir(dirp);
    //avoid this for alphabetical order in linux
    std::sort(rowInfoVec.begin(), rowInfoVec.end(), [](const RowInfo& lhs, const RowInfo& rhs)
    {
        return lhs.name < rhs.name;
    });

    return dir_count;
}
/**********************************************************/
bool Utilities::checkLogValidity(const char *filename)
{
    bool check = true;
    fstream str;
    str.open (filename, ios_base::in);//, ios::binary);

    if (str.is_open()){
        string line;
        int itr = 0;
        while( getline( str, line ) && itr < 3){
            Bottle b( line );
            if ( b.size() == 0){
                check = false;
                break;
            }
            itr++;
        }
        str.close();
        fprintf (stdout, "The file contains at least %d non-empty lines\n",itr );
    }
    return check;
}
/**********************************************************/
bool Utilities::setupDataFromParts(partsData &part)
{
    fstream str;

    // info part
    LOG("opening file %s\n", part.infoFile.c_str() );

    str.open (part.infoFile.c_str(), ios_base::in);//, ios::binary);
    if (str.is_open()){
        string line;
        int itr = 0;
        while( getline( str, line ) && (itr < 3) ){
            Bottle b( line );
            if (itr == 0){
                part.type = b.get(1).toString();
                part.type.erase(part.type.size() -1 );          // remove the ";" character
            }
            else{
                string stamp_tag = b.get(0).toString();
                if (stamp_tag.find("Stamp") == string::npos){   // skip stamp information
                    part.portName = b.get(1).toString();
                    LOG( "the name of the port is %s\n",part.portName.c_str());
                    break;
                }
            }
            itr++;
        }
        str.close();
    } else {
        return false;
    }

    // data part
    LOG("opening file %s\n", part.logFile.c_str() );
    str.open (part.logFile.c_str(), ios_base::in);//, ios::binary);

    //read throughout
    if (str.is_open()){
        string line;
        int itr = 0;
        while( getline( str, line ) ){
            Bottle b( line );
            part.bot.addList() = b;
            int timeStampCol = 1;
            if (withExtraColumn){
                timeStampCol = column;
            }

            part.timestamp.push_back( b.get(timeStampCol).asFloat64() );
            itr++;
        }
        allTimeStamps.push_back( part.timestamp[0] );   //save all first timeStamps dumped for later ease of use
        part.maxFrame= itr-1;                           //set max frame to the total iteration minus first line type;
        part.currFrame = 0;                             //initialize current frame to 0
        str.close();                                    //close the file
    } else {
        return false;
    }

    return true;
}
/**********************************************************/
void Utilities::getMaxTimeStamp()
{
    maxTimeStamp = 0.0;
    int index = 0;
    for (int i=0; i< (int)allTimeStamps.size(); i++ ){
        LOG("%lf\n",allTimeStamps[i]);
        if ( maxTimeStamp < allTimeStamps[i] ){
            maxTimeStamp = allTimeStamps[i];
            index = i;
        }
    }
    LOG("the biggest timestamp is: index %d with value %lf\n",index, allTimeStamps[index] );
}

/**********************************************************/
void Utilities::getMinTimeStamp()
{
    minTimeStamp = allTimeStamps[0];
    int index = 0;
    for (int i=0; i< (int)allTimeStamps.size(); i++ ){
        LOG("%lf\n",allTimeStamps[i]);
        if ( minTimeStamp > allTimeStamps[i] ){
            minTimeStamp = allTimeStamps[i];
            index = i;
        }
    }
    LOG("the smallest timestamp is: index %d with value %lf\n",index, allTimeStamps[index] );
}

/**********************************************************/
int Utilities::amendPartFrames(partsData &part)
{
    while (part.timestamp[part.currFrame] < maxTimeStamp){
        part.currFrame++;
    }
    LOG("the first frame of part %s is %d\n",part.name.c_str(), part.currFrame);
    return part.currFrame;
}
/**********************************************************/
void Utilities::resetMaxTimeStamp()
{
    allTimeStamps.clear();
    maxTimeStamp = 0.0;
}
/**********************************************************/
void Utilities::resetDirCount()
{
    dir_count = 0;
}
/**********************************************************/
bool Utilities::configurePorts(partsData &part)
{
    string tmp_port_name=part.portName;
    if(add_prefix){
        tmp_port_name="/"+moduleName+tmp_port_name;
    }

    if (strcmp (part.type.c_str(),"Bottle") == 0)   {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::os::Bottle>; }
    }
    else if (strcmp (part.type.c_str(),"Image:ppm") == 0 || strcmp (part.type.c_str(),"Image") == 0)  {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::sig::Image>; }
    }
    else if (strcmp(part.type.c_str(), "sensor_msgs/LaserScan") == 0 ) {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::rosmsg::sensor_msgs::LaserScan>; }
    }
    else if (strcmp(part.type.c_str(), "nav_msgs/Odometry") == 0) {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::rosmsg::nav_msgs::Odometry>; }
    }
    else if (strcmp(part.type.c_str(), "tf2_msgs/tf") == 0) {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::rosmsg::tf2_msgs::TFMessage>; }
    }
    else if (strcmp(part.type.c_str(), "geometry_msgs/Pose") == 0) {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::rosmsg::geometry_msgs::Pose>; }
    }
    else if (strcmp(part.type.c_str(), "geometry_msgs/Pose2D") == 0) {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::rosmsg::geometry_msgs::Pose2D>; }
    }
    else
    {
        LOG("Something is wrong with the data...%s\nIt seems its missing a type \n",part.name.c_str());
        return false;
    }

    if (!yarp::os::Network::exists(tmp_port_name))
    {
        LOG("need to create new port %s for %s\n", part.type.c_str(), part.name.c_str());
        part.outputPort->close();
        LOG("creating and opening %s port for part %s\n", part.type.c_str(), part.name.c_str());
        part.outputPort->open(tmp_port_name);
    }
    else
    {
        LOG("port %s already exists, skipping\n", tmp_port_name.c_str());
    }


    return true;
}
/**********************************************************/
bool Utilities::interruptPorts(partsData &part)
{
    if (part.outputPort == nullptr)  {
        return false;
    }
    part.outputPort->interrupt();
    return true;
}
/**********************************************************/
bool Utilities::closePorts(partsData &part)
{
    if (part.outputPort == nullptr)  {
        return false;
    }
    part.outputPort->close();
    return true;
}

/**********************************************************/
void Utilities::stopAtEnd()
{
    LOG( "asking utils to stop the thread \n");
    masterThread->askToStop();
    for (int i=0; i < totalThreads; i++){
        partDetails[i].currFrame = (int)initialFrame[i];
    }

    //TODO SIGNAL
    //masterThread->wnd->resetButtonOnStop();
    pause();
    LOG( "ok................ \n");
}
/**********************************************************/
void Utilities::stepThread()
{
    masterThread->stepfromCmd = true;
    LOG( "asking utils to step the thread \n");
    if ( masterThread->isRunning() ){
        masterThread->resume();
    } else {
        for (int i=0; i < totalThreads; i++){
            partDetails[i].worker->init();
        }
        masterThread->start();
    }
    LOG( "ok................ \n");
}
/**********************************************************/
void Utilities::pauseThread()
{   LOG("asking the thread to pause\n");
    masterThread->pause();
    LOG( "ok................ \n");
    masterThread->stepfromCmd = false;
}
