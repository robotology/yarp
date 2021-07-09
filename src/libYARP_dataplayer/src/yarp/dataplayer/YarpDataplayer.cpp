/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <dirent.h>

#if defined(_WIN32)
    #undef max
    #include <direct.h>
   // #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #include <cerrno>
    #include <sys/stat.h>
//    #define GetCurrentDir getcwd
#endif


#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <yarp/os/RpcClient.h>
#include <yarp/os/SystemClock.h>
#include <yarp/dataplayer/YarpDataplayer.h>

//ROS messages
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>


using namespace std;
using namespace yarp::yarpDataplayer;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

#ifdef HAS_OPENCV
  using namespace cv;
#endif

/**********************************************************/
DataplayerUtilities::~DataplayerUtilities()
{
    if(dataplayerEngine){
        delete dataplayerEngine;
        dataplayerEngine = nullptr;
    }

    if(partDetails){
        for (int i=0; i < totalThreads; i++){
            delete partDetails[i].worker;
        }
        delete[] partDetails;
    }
}

/**********************************************************/
DataplayerUtilities::DataplayerUtilities() :
    dir_count(0),
    moduleName(""),
    add_prefix(false),
    partDetails(nullptr),
    speed(1.0),
    repeat(false),
    sendStrict(false),
    totalSent(0),
    totalThreads(0),
    recursiveIterations(0),
    dataplayerEngine(nullptr),
    withExtraColumn(false),
    column(0),
    maxTimeStamp(0.0),
    minTimeStamp(0.0),
    verbose(false)
{
    dataplayerEngine = new DataplayerEngine(this);
}

/**********************************************************/
DataplayerUtilities::DataplayerUtilities(string name, bool _add_prefix, bool _verbose) :
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
    dataplayerEngine(nullptr),
    withExtraColumn(false),
    column(0),
    maxTimeStamp(0.0),
    minTimeStamp(0.0),
    verbose(_verbose)
{
    dataplayerEngine = new DataplayerEngine(this);
}

/**********************************************************/
void DataplayerUtilities::setModuleName(const std::string &name)
{
    moduleName = std::move(name);
}

/**********************************************************/
void DataplayerUtilities::addPrefix(const bool &prefix)
{
    add_prefix = prefix;
}

/**********************************************************/
void DataplayerUtilities::setVerbose(const bool &verbose)
{
    this->verbose = verbose;
}

/**********************************************************/
string DataplayerUtilities::getCurrentPath()
{
    char cCurrentPath[FILENAME_MAX];
    if (!yarp::os::getcwd(cCurrentPath, sizeof(cCurrentPath) )){
        yError() << "ERROR GETTING THE CURRENT DIR";
    }
    //cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    string currentPath = cCurrentPath;

    return currentPath;
}

/**********************************************************/
int DataplayerUtilities::getRecSubDirList(const string& dir, vector<RowInfo>& rowInfoVec, int recursive)
{
    struct dirent *direntp = nullptr;
    DIR *dirp = nullptr;
    size_t path_len;
    const char *path = dir.c_str();

    /* Check validity of directory */
    if (!path){
        return 0;
    }
    /* Get length of the directory */
    path_len = strlen(path);
    /* Check length of the directory */
    if (!path || !path_len || (path_len > FILENAME_MAX)){
        return 0;
    }
    /* Check if file is opened */
    if((dirp  = opendir(path)) == nullptr){
        yError() << "Error opening " << dir.c_str();
        return 0;
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
                    if (verbose){
                        yInfo() << filename << " IS present adding it to the gui";
                    }
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
                        if (verbose){
                            yInfo() << filename << " IS present BUT corrupted not using file";
                            //errorMessage(QString("%1").arg(filename));
                        }
                    }
                    if (!checkData){
                        if (verbose){
                            yInfo() << dataFileName.c_str() << " IS present BUT corrupted not using file";
                            //errorMessage(QString("%1").arg(dataFileName.c_str()));
                        }
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
bool DataplayerUtilities::checkLogValidity(const char *filename)
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
        if (verbose){
            fprintf (stdout, "The file contains at least %d non-empty lines\n",itr );
        }
    }
    return check;
}

/**********************************************************/
bool DataplayerUtilities::setupDataFromParts(PartsData &part)
{
    fstream str;

    // info part
    if (verbose){
        yInfo() << "opening file " <<  part.infoFile.c_str() ;
    }

    str.open (part.infoFile.c_str(), ios_base::in);//, ios::binary);
    if (str.is_open()){
        string line;
        int itr = 0;
        while( getline( str, line ) && (itr < 3) ){
            Bottle b( line );
            if (itr == 0){
                part.type = b.get(1).toString();
                if (part.type == "")
                {
                    if (verbose){
                        yError() << "Invalid format, missing data type?!";
                    }
                    return false;
                }
                part.type.erase(part.type.size() -1 );          // remove the ";" character
            }
            else{
                string stamp_tag = b.get(0).toString();
                if (stamp_tag.find("Stamp") == string::npos){   // skip stamp information
                    part.portName = b.get(1).toString();
                    if (verbose) {
                        yInfo() << "the name of the port is " << part.portName.c_str();
                    }
                    break;
                }
            }
            itr++;
        }
        str.close();
    } else {
        return false;
    }

    if (verbose){
        yInfo() <<"opening file " << part.logFile.c_str();
    }
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
void DataplayerUtilities::getMaxTimeStamp()
{
    maxTimeStamp = 0.0;
    int index = 0;
    for (int i=0; i< (int)allTimeStamps.size(); i++ ){
        if (verbose){
            yInfo() << allTimeStamps[i];
        }
        if ( maxTimeStamp < allTimeStamps[i] ){
            maxTimeStamp = allTimeStamps[i];
            index = i;
        }
    }
    if (verbose){
        yInfo() << "the biggest timestamp is: index " << index << "with value " << allTimeStamps[index];
    }
}

/**********************************************************/
void DataplayerUtilities::getMinTimeStamp()
{
    minTimeStamp = allTimeStamps[0];
    int index = 0;
    for (int i=0; i< (int)allTimeStamps.size(); i++ ){
        if (verbose){
            yInfo() << allTimeStamps[i];
        }
        if ( minTimeStamp > allTimeStamps[i] ){
            minTimeStamp = allTimeStamps[i];
            index = i;
        }
    }
    if (verbose){
        yInfo() << "the smallest timestamp is: index " << index << "with value " << allTimeStamps[index];
    }
}

/**********************************************************/
int DataplayerUtilities::amendPartFrames(PartsData &part)
{
    while (part.timestamp[part.currFrame] < maxTimeStamp){
        part.currFrame++;
    }
    if (verbose) {
        yInfo() << "the first frame of part " << part.name.c_str() << " is " << part.currFrame;
    }
    return part.currFrame;
}

/**********************************************************/
void DataplayerUtilities::resetMaxTimeStamp()
{
    allTimeStamps.clear();
    maxTimeStamp = 0.0;
}

/**********************************************************/
void DataplayerUtilities::resetDirCount()
{
    dir_count = 0;
}

/**********************************************************/
bool DataplayerUtilities::configurePorts(PartsData &part)
{
    string tmp_port_name=part.portName;
    if(add_prefix){
        tmp_port_name="/"+moduleName+tmp_port_name;
    }

    if (strcmp (part.type.c_str(),"Bottle") == 0)   {
        if (part.outputPort == nullptr) { part.outputPort = new BufferedPort<yarp::os::Bottle>; }
    }
    else if (strcmp (part.type.c_str(),"Image") == 0 ||
             strcmp(part.type.c_str(), "Image:ppm") == 0 ||
             strcmp(part.type.c_str(), "Image:jpg") == 0 ||
             strcmp (part.type.c_str(),"Image:png") == 0 ||
             strcmp(part.type.c_str(), "Depth") == 0 ||
             strcmp(part.type.c_str(), "DepthCompressed") == 0
            )
    {
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
        if (verbose){
            yError() << "Something is wrong with the data..." << part.name.c_str() << "Type (" << part.type.c_str()  << ") is unrecognized";
        }
        return false;
    }

    if (!yarp::os::Network::exists(tmp_port_name))
    {
        if (verbose){
            yInfo() << "need to create new port " << part.type.c_str() << " for " << part.name.c_str();
        }
        part.outputPort->close();
        if (verbose){
            yInfo() << "creating and opening " << part.type.c_str() << " port for part " << part.name.c_str();
        }
        part.outputPort->open(tmp_port_name);
    }
    else
    {
        if (verbose){
            yInfo() << "port " << tmp_port_name.c_str() << "already exists, skipping";
        }
    }

    return true;
}

/**********************************************************/
bool DataplayerUtilities::interruptPorts(PartsData &part)
{
    if (part.outputPort == nullptr)  {
        return false;
    }
    part.outputPort->interrupt();
    return true;
}

/**********************************************************/
bool DataplayerUtilities::closePorts(PartsData &part)
{
    if (part.outputPort == nullptr)  {
        return false;
    }
    part.outputPort->close();
    return true;
}

/**********************************************************/
void DataplayerUtilities::stopAtEnd()
{
    if (verbose){
        yInfo() << "asking utils to stop the thread";
    }
    dataplayerEngine->askToStop();
    for (int i=0; i < totalThreads; i++){
        partDetails[i].currFrame = (int)initialFrame[i];
    }
    if (verbose){
        yInfo() << "ok................";
    }
}
/**********************************************************/
void DataplayerUtilities::stepThread()
{
    dataplayerEngine->stepfromCmd = true;
    if (verbose){
        yInfo() << "asking utils to step the thread";
    }
    if ( dataplayerEngine->isRunning() ){
        dataplayerEngine->pause();
    }
    if ( dataplayerEngine->isSuspended() ){
        dataplayerEngine->resume();
    } else if ( !dataplayerEngine->isRunning() ) {
        for (int i=0; i < totalThreads; i++){
            partDetails[i].worker->init();
        }
        dataplayerEngine->start();
    }
    if (verbose){
        yInfo() << "ok................ \n";
    }

}
/**********************************************************/
void DataplayerUtilities::pauseThread()
{
    if (verbose){
        yInfo() << "asking the thread to pause";
    }
    dataplayerEngine->pause();
    if (verbose){
        yInfo() << "ok................ ";
    }
    dataplayerEngine->stepfromCmd = false;
}

/**********************************************************/
DataplayerWorker::DataplayerWorker(int part, int numThreads) :
    utilities(nullptr),
    part(part),
    percentage(0),
    numThreads(numThreads),
    currFrame(0),
    isActive(true),
    frameRate(0.0),
    initTime(0.0),
    virtualTime(0.0),
    startTime(0.0)
{}

/**********************************************************/
bool DataplayerWorker::init()
{
    utilities->partDetails[part].sent = 0;
    initTime = 0.0;
    frameRate = 0.0;
    utilities->partDetails[part].hasNotified = false;

    if (utilities->verbose){
#ifdef HAS_OPENCV
        yInfo() << "USING OPENCV FOR SENDING THE IMAGES";
#else
        yInfo() << "USING YARP IMAGE FOR SENDING THE IMAGES";
#endif
    }

    return true;
}

/**********************************************************/
void DataplayerWorker::release()
{
}

/**********************************************************/
void DataplayerWorker::resetTime()
{
    initTime = 0.0;
    virtualTime = 0.0;
}

/**********************************************************/
void DataplayerWorker::sendData(int frame, bool isActive, double virtualTime)
{
    this->virtualTime = virtualTime;
    this->isActive = isActive;
    currFrame = frame;
    this->run();
}

/**********************************************************/
void DataplayerWorker::run()
{
    int frame = currFrame;
    if (initTime == 0 ){
        initTime = Time::now();
        startTime = yarp::os::Time::now();
    } else {
        double t = Time::now();
        frameRate = t-initTime;

        //LOG("initTime %lf t= %lf frameRate %lf\n", initTime, t, frameRate);
        initTime = t;
    }

    if (isActive)
    {
        int ret=-1;
        if (strcmp (utilities->partDetails[part].type.c_str(),"Image") == 0 ||
            strcmp (utilities->partDetails[part].type.c_str(),"Image:ppm") == 0 ||
            strcmp(utilities->partDetails[part].type.c_str(), "Image:png") == 0 ||
            strcmp(utilities->partDetails[part].type.c_str(), "Image:jpg") == 0)
        {
            ret = sendImages(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "Depth") == 0 ||
                 strcmp(utilities->partDetails[part].type.c_str(), "DepthCompressed") == 0) {
            //I am keeping depth separated from images only for clarity
            ret = sendImages(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "Bottle") == 0)  {
            ret = sendBottle(part, frame);
            // the above line can be safely replaced with sendGenericData<Bottle>.
            // I kept it for no particular reason, thinking that maybe it could be convenient (later)
            // to process Bottles in a different way.
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "sensor_msgs/LaserScan") == 0)  {
            ret = sendGenericData<yarp::rosmsg::sensor_msgs::LaserScan>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "nav_msgs/Odometry") == 0) {
            ret = sendGenericData<yarp::rosmsg::nav_msgs::Odometry>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "tf2_msgs/tf") == 0) {
            ret = sendGenericData<yarp::rosmsg::tf2_msgs::TFMessage>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "geometry_msgs/Pose") == 0) {
            ret = sendGenericData<yarp::rosmsg::geometry_msgs::Pose>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "geometry_msgs/Pose2D") == 0) {
            ret = sendGenericData<yarp::rosmsg::geometry_msgs::Pose2D>(part, frame);
        }
        else  {
            if (utilities->verbose){
                yInfo() << "Unknown data type: " << utilities->partDetails[part].type.c_str();
            }
        }

        if (ret==-1)  {
            if (utilities->verbose){
                yInfo() << "Failed to send data:" << utilities->partDetails[part].type.c_str();
            }
        }
    }
    utilities->partDetails[part].sent++;
}

/**********************************************************/
double DataplayerWorker::getFrameRate()
{
    //LOG("FRAME RATE %lf\n", frameRate );
    frameRate = frameRate*1000;
    return frameRate;
}

/**********************************************************/
double DataplayerWorker::getTimeTaken()
{
    return yarp::os::Time::now()-startTime;
}

/**********************************************************/
int DataplayerWorker::sendBottle(int part, int frame)
{
    Bottle tmp;
    if (utilities->withExtraColumn) {
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail();
    }
    else {
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail();
    }

    yarp::os::BufferedPort<Bottle>* the_port = dynamic_cast<yarp::os::BufferedPort<yarp::os::Bottle>*> (utilities->partDetails[part].outputPort);
    if (the_port == nullptr){
        if (utilities->verbose){
            yError() << "dynamic_cast failed";
        }
        return -1;
    }

    Bottle& outBot = the_port->prepare();
    outBot = tmp;

    //propagate timestamp
    std::string time = yarp::conf::numeric::to_string(utilities->partDetails[part].timestamp[frame]);
    Bottle ts(time);
    the_port->setEnvelope(ts);

    if (utilities->sendStrict) {
        the_port->writeStrict();
    }
    else {
        the_port->write();
    }
    return 0;
}

/**********************************************************/
int DataplayerWorker::sendImages(int part, int frame)
{
    string tmpPath = utilities->partDetails[part].path;
    string tmpName, tmp;
    bool fileValid = false;
    if (utilities->withExtraColumn) {
        tmpName = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().get(1).asString();
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail().tail().toString();
    } else {
        tmpName = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().get(0).asString();
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail().toString();
    }

    int code = 0;
    if (tmp.size()>0) {
        tmp.erase(tmp.begin());
        tmp.erase(tmp.end()-1);
        code = Vocab32::encode(tmp);
    }

    tmpPath = tmpPath + tmpName;
    unique_ptr<Image> img_yarp = nullptr;

#ifdef HAS_OPENCV
    cv::Mat cv_img;
    if (code==VOCAB_PIXEL_MONO_FLOAT) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelFloat>);
        fileValid = read(*static_cast<ImageOf<PixelFloat>*>(img_yarp.get()),tmpPath);
    }
    else {
        cv_img = cv::imread(tmpPath, cv::ImreadModes::IMREAD_UNCHANGED);
        if ( cv_img.data != nullptr ) {
            if (code==VOCAB_PIXEL_RGB)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgb>(cv_img);
            }
            else if (code==VOCAB_PIXEL_BGR)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelBgr>);
                *img_yarp = yarp::cv::fromCvMat<PixelBgr>(cv_img);
            }
            else if (code==VOCAB_PIXEL_RGBA)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgba>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgba>(cv_img);
            }
            else if (code==VOCAB_PIXEL_MONO){
                img_yarp = unique_ptr<Image>(new ImageOf<PixelMono>);
                *img_yarp = yarp::cv::fromCvMat<PixelMono>(cv_img);
            }
            else
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgb>(cv_img);
            }
            fileValid = true;
        }
    }
#else
    if (code==VOCAB_PIXEL_RGB) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
        fileValid = read(*static_cast<ImageOf<PixelRgb>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_BGR) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelBgr>);
        fileValid = read(*static_cast<ImageOf<PixelBgr>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_RGBA) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgba>);
        fileValid = read(*static_cast<ImageOf<PixelRgba>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_MONO_FLOAT) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelFloat>);
        fileValid = read(*static_cast<ImageOf<PixelFloat>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_MONO) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelMono>);
        fileValid = read(*static_cast<ImageOf<PixelMono>*>(img_yarp.get()),tmpPath.c_str());
    } else {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
        fileValid = read(*static_cast<ImageOf<PixelRgb>*>(img_yarp.get()),tmpPath.c_str());
    }
#endif
    if (!fileValid) {
        if (utilities->verbose){
            yError() << "Cannot load file " << tmpPath.c_str() ;
        }
        return 1;
    }
    else
    {
        yarp::os::BufferedPort<yarp::sig::Image>* the_port = dynamic_cast<yarp::os::BufferedPort<yarp::sig::Image>*> (utilities->partDetails[part].outputPort);
        if (the_port == nullptr) { yFatal() << "dynamic_cast failed"; }

        the_port->prepare()=*img_yarp;

        Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
        the_port->setEnvelope(ts);

        if (utilities->sendStrict) {
            the_port->writeStrict();
        } else {
            the_port->write();
        }
    }

    return 0;
}

/**********************************************************/
void DataplayerWorker::setManager(yarp::yarpDataplayer::DataplayerUtilities *utilities)
{
    this->utilities= utilities;
}

/**********************************************************/
template int DataplayerWorker::sendGenericData<yarp::os::Bottle>(int,int);
template int DataplayerWorker::sendGenericData<yarp::rosmsg::sensor_msgs::LaserScan>(int, int);
template int DataplayerWorker::sendGenericData<yarp::rosmsg::nav_msgs::Odometry>(int, int);
template int DataplayerWorker::sendGenericData<yarp::rosmsg::tf2_msgs::TFMessage>(int, int);
template int DataplayerWorker::sendGenericData<yarp::rosmsg::geometry_msgs::Pose>(int, int);
template int DataplayerWorker::sendGenericData<yarp::rosmsg::geometry_msgs::Pose2D>(int, int);

/**********************************************************/
template <class T>
int DataplayerWorker::sendGenericData(int part, int id)
{
    yarp::os::Bottle tmp;
    if (utilities->withExtraColumn) {
        tmp = utilities->partDetails[part].bot.get(id).asList()->tail().tail().tail();
    }
    else {
        tmp = utilities->partDetails[part].bot.get(id).asList()->tail().tail();
    }

    yarp::os::BufferedPort<T>* the_port = dynamic_cast<yarp::os::BufferedPort<T>*> (utilities->partDetails[part].outputPort);
    if (the_port == nullptr) { yError() << "dynamic_cast failed"; return -1; }

    auto& dat = the_port->prepare();
    yarp::os::Portable::copyPortable(tmp, dat);

    //propagate timestamp
    yarp::os::Stamp ts(id, utilities->partDetails[part].timestamp[id]);
    the_port->setEnvelope(ts);

    if (utilities->sendStrict) {
        the_port->writeStrict();
    }
    else {
        the_port->write();
    }
    return 0;
}

/**********************************************************/
DataplayerEngine::DataplayerEngine()
{
    this->dataplayer_updater = new dataplayer_thread ();
    this->dataplayer_updater->SetDataplayerEngine(*this);
    numThreads = 0;
    timePassed = 0.0;
    initTime = 0;
    virtualTime = 0.0;
    stepfromCmd = false;
    pauseStart = 0.0;
    pauseEnd = 0.0;
    allPartsStatus = false;
    this->utilities = nullptr;
}

/**********************************************************/
DataplayerEngine::DataplayerEngine(DataplayerUtilities *utilities)
{
    this->dataplayer_updater = new dataplayer_thread ();
    this->dataplayer_updater->SetDataplayerEngine(*this);
    numThreads = 0;
    timePassed = 0.0;
    initTime = 0;
    virtualTime = 0.0;
    stepfromCmd = false;
    pauseStart = 0.0;
    pauseEnd = 0.0;
    allPartsStatus = false;
    this->utilities = utilities;
}

/**********************************************************/
DataplayerEngine::DataplayerEngine(DataplayerUtilities *utilities, int numPart)
{
    this->dataplayer_updater = new dataplayer_thread ();
    this->dataplayer_updater->SetDataplayerEngine(*this);
    numThreads = 0;
    timePassed = 0.0;
    initTime = 0;
    virtualTime = 0.0;
    stepfromCmd = false;
    pauseStart = 0.0;
    pauseEnd = 0.0;
    allPartsStatus = false;
    this->numPart = numPart;
    isPartActive.resize(this->numPart);
    std::fill(isPartActive.begin(), isPartActive.end(), true);
    this->utilities = utilities;
}

/**********************************************************/
DataplayerEngine::~DataplayerEngine()
{
    delete this->dataplayer_updater;
}

/**********************************************************/
bool DataplayerEngine::setNumPart(int numPart)
{
    this->numPart = numPart;
    isPartActive.resize(this->numPart);
    std::fill(isPartActive.begin(), isPartActive.end(), true);
    return true;
}

/**********************************************************/
void DataplayerEngine::setUtilities(DataplayerUtilities *utilities)
{
    this->utilities = utilities;
}

/**********************************************************/
void DataplayerEngine::setThread(dataplayer_thread *dataplayer_updater)
{
    this->dataplayer_updater = dataplayer_updater;
    this->dataplayer_updater->SetDataplayerEngine(*this);
}

/**********************************************************/
DataplayerEngine::dataplayer_thread::dataplayer_thread (double _period) : PeriodicThread(_period)
{

}

/**********************************************************/
bool DataplayerEngine::dataplayer_thread::threadInit()
{
    this->dataplayerEngine->allPartsStatus = false;
    this->dataplayerEngine->initTime = 0;
    for (int i =0; i < this->dataplayerEngine->numPart; i++)
    {
        this->dataplayerEngine->utilities->partDetails[i].currFrame = 0;
    }

    //virtualTime = utilities->partDetails[0].timestamp[ utilities->partDetails[0].currFrame ];
    this->dataplayerEngine->virtualTime = this->dataplayerEngine->utilities->minTimeStamp;

    if (this->dataplayerEngine->utilities->verbose)
    {
        yDebug() << "virtual time is " << this->dataplayerEngine->virtualTime ;
    }

    this->dataplayerEngine->lastUpdate = std::chrono::high_resolution_clock::now();
    this->dataplayerEngine->dtSeconds = 0.f;
    this->dataplayerEngine->fps = 0.f;

    return true;
}

/**********************************************************/
void DataplayerEngine::dataplayer_thread::threadRelease()
{

}

/**********************************************************/
void DataplayerEngine::dataplayer_thread::run()
{
    if (this->dataplayerEngine->stepfromCmd){
        this->dataplayerEngine->stepFromCmd();
    } else {
        this->dataplayerEngine->runNormally();
    }
}

/**********************************************************/
void DataplayerEngine::stepFromCmd()
{
    if (utilities->verbose){
        yInfo() << "Step from command";
    }
    bool sentAll = false;
    for (int i=0; i < this->numPart; i++){
        this->utilities->partDetails[i].sent = 0;
    }
    this->utilities->totalSent = 0;

    while ( !sentAll ){
        runNormally();
        for (int i=0; i < this->numPart; i++){
            if (this->utilities->partDetails[i].sent == 1){
                this->utilities->totalSent ++;
                this->utilities->partDetails[i].sent = 2;
            }
            if (this->utilities->totalSent == this->numPart){
                sentAll = true;
            }
        }
    }

    for (int i=0; i < this->numPart; i++){
        this->utilities->partDetails[i].sent = 0;
    }

    this->utilities->totalSent = 0;
    this->utilities->pauseThread();
    for (int i=0; i < this->numPart; i++){
        this->virtualTime = this->utilities->partDetails[i].timestamp[this->utilities->partDetails[i].currFrame];
    }
    this->virtualTime = this->utilities->partDetails[0].timestamp[this->utilities->partDetails[0].currFrame];
}

/**********************************************************/
void DataplayerEngine::runNormally()
{
    for (int i=0; i < this->numPart; i++){
        bool isActive = this->isPartActive[i];
        if ( this->utilities->partDetails[i].currFrame <= this->utilities->partDetails[i].maxFrame ){
            if ( this->virtualTime >= this->utilities->partDetails[i].timestamp[ this->utilities->partDetails[i].currFrame ] ){
                if ( this->initTime > 300 && this->virtualTime < this->utilities->partDetails[i].timestamp[this->utilities->partDetails[i].timestamp.length()-1]){
                    this->initTime = 0;
                }
                if (!this->utilities->partDetails[i].hasNotified){
                    this->utilities->partDetails[i].worker->sendData(this->utilities->partDetails[i].currFrame, isActive, this->virtualTime );
                    this->utilities->partDetails[i].currFrame++;
                }
            }
        } else {
            if (this->utilities->repeat) {
                this->initThread();
                this->utilities->partDetails[i].worker->init();
            } else {
                if ( !this->utilities->partDetails[i].hasNotified ) {
                    if (utilities->verbose){
                        yInfo() << "partID:" << i << "has finished";
                    }
                    this->utilities->partDetails[i].hasNotified = true;
                }

                int stopAll = 0;
                for (int x=0; x < this->numPart; x++){
                    if (this->utilities->partDetails[x].hasNotified){
                        stopAll++;
                    }

                    if (stopAll == this->numPart){
                        if (utilities->verbose) {
                            yInfo() << "All parts have Finished!";
                        }
                        this->utilities->stopAtEnd();
                        this->allPartsStatus = true;
                    }
                }
            }
        }
    }

    this->virtualTime += this->diff_seconds() * this->utilities->speed;
    this->tick();
    this->initTime++;
}

/**********************************************************/
void DataplayerEngine::tick()
{
    Moment now = std::chrono::high_resolution_clock::now();

    const unsigned diff = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count();
    lastUpdate = now;

    dtSeconds = diff / (1000.f * 1000.f); // micro-seconds -> seconds
    fps = 1.f / dtSeconds;
}

/**********************************************************/
void DataplayerEngine::goToPercentage(int value)
{
    for (int i=0; i < numPart; i++){
        int maxFrame = utilities->partDetails[i].maxFrame;
        int currFrame = (value * maxFrame) / 100;
        utilities->partDetails[i].currFrame = currFrame;
    }
    virtualTime = utilities->partDetails[0].timestamp[ utilities->partDetails[0].currFrame ];
}

/**********************************************************/
void DataplayerEngine::forward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++){
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) / 100 );
        if ( utilities->partDetails[i].currFrame < utilities->partDetails[i].maxFrame - selectedFrame){
            utilities->partDetails[i].currFrame += selectedFrame;
            if (i == 0){
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
            }
        } else {
            yError() << "cannot go any forward, out of range" ;
        }
    }
}

/**********************************************************/
void DataplayerEngine::backward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++){
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) /100 );
        if ( utilities->partDetails[i].currFrame > selectedFrame){
            utilities->partDetails[i].currFrame -= selectedFrame;
            if (i == 0){
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
            }
        } else {
            yError() << "cannot go any backwards, out of range.. " ;
        }
    }
}

/**********************************************************/
void DataplayerEngine::pause()
{
    if (utilities->verbose){
        yInfo()<<"pausing thread";
    }
    pauseStart = yarp::os::Time::now();
    dataplayer_updater->suspend();
}

/**********************************************************/
void DataplayerEngine::resume()
{
    if (utilities->verbose){
        yInfo()<<"resuming thread";
    }
    allPartsStatus = false;
    pauseEnd = yarp::os::Time::now();
    virtualTime -= pauseEnd - pauseStart;
    //PeriodicThread::resume();
    dataplayer_updater->resume();
}
