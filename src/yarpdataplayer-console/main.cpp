/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/WireLink.h>
#include <yarp/dataplayer/YarpDataplayer.h>
#include "yarpdataplayer_console_IDL.h"

#include <string>
#include <cstdio>
#include <iostream>

#define LOGO_MESSAGE "\
__ _____ ________  ___/ /__ _/ /____ ____  / /__ ___ _____ ____ \n\
/ // / _ `/ __/ _ \\/ _  / _ `/ __/ _ `/ _ \\/ / _ `/ // / -_) __/ \n\
\\_, /\\_,_/_/ / .__/\\_,_/\\_,_/\\__/\\_,_/ .__/_/\\_,_/\\_, /\\__/_/ \n\
/___/        /_/                     /_/          /___/ "


using namespace std;
using namespace yarp::os;
using namespace yarp::yarpDataplayer;

class dataplayer_module : public yarp::os::RFModule, public yarpdataplayer_console_IDL
{
    DataplayerUtilities* utilities = nullptr;
    std::vector<yarp::yarpDataplayer::RowInfo> rowInfoVec;
    int subDirCnt;
    yarp::os::RpcServer rpcPort;
    bool verbose;
    string dataset;
    string status;

    float progress;

    /**********************************************************/
    bool configure(yarp::os::ResourceFinder &rf) override
    {
        string moduleName = rf.check("module_name", Value("yarpdataplayer")).asString();
        bool add_prefix = rf.check("add_prefix", Value(false)).asBool();
        verbose = rf.check("verbose",Value(false)).asBool();
        dataset = rf.check("dataset",Value("")).asString();

        utilities = new DataplayerUtilities(moduleName, add_prefix, verbose);
        if (rf.check("withExtraTimeCol"))
        {
            utilities->withExtraColumn = true;
            utilities->column = rf.find("withExtraTimeCol").asInt32();
        }

        utilities->dataplayerEngine->stepfromCmd = false;
        subDirCnt = 0;

        rpcPort.open("/yarpdataplayer/rpc:i");
        attach(rpcPort);

        progress = 0.0;
        status = "";

        std::cout<<std::endl<<std::endl<<LOGO_MESSAGE<<std::endl<<std::endl;

        if (!dataset.empty())
        {
            load(dataset);
            std::cout << std::endl << std::endl;
        }
        return true;
    }

    /**********************************************************/
    bool attach(Port& source) override
    {
        return this->yarp().attachAsServer(source);
    }

    /**********************************************************/
    bool attach(RpcServer &source) override
    {
        return this->yarp().attachAsServer(source);
    }

    /**********************************************************/
    bool interruptModule() override
    {
        rpcPort.interrupt();
        return true;
    }

    /**********************************************************/
    bool close() override
    {
        rpcPort.interrupt();
        rpcPort.close();

        if (utilities)
        {
            delete utilities;
            utilities = nullptr;
        }

        return true;
    }

    /**********************************************************/
    double getPeriod() override
    {
        return 1.0;
    }

    /**********************************************************/
    bool updateModule() override
    {
        return true;
    }

    /**********************************************************/
    bool load(const string &filename) override
    {
        utilities->resetMaxTimeStamp();
        subDirCnt = utilities->getRecSubDirList(filename, rowInfoVec, 1);
        if (verbose)
        {
            yInfo() << "the size of subDirs is: " << subDirCnt;
        }
        //reset totalSent to 0
        utilities->totalSent = 0;
        utilities->totalThreads = subDirCnt;
        if (subDirCnt > 0)
        {
            utilities->partDetails = new yarp::yarpDataplayer::PartsData [subDirCnt];
        }

        //fill in parts with all data
        for (int x=0; x < subDirCnt; x++)
        {
            utilities->partDetails[x].name = rowInfoVec[x].name;
            utilities->partDetails[x].infoFile = rowInfoVec[x].info;
            utilities->partDetails[x].logFile = rowInfoVec[x].log;
            utilities->partDetails[x].path = rowInfoVec[x].path;

            utilities->setupDataFromParts(utilities->partDetails[x]);

            utilities->partDetails[x].worker = new yarp::yarpDataplayer::DataplayerWorker(x, subDirCnt);
            utilities->partDetails[x].worker->setManager(utilities);
        }

        //get the max timestamp of all the parts for synchronization
        if (subDirCnt > 0)
        {
            utilities->getMaxTimeStamp();
        }

        if (subDirCnt > 0){
            utilities->getMinTimeStamp();
        }


        //set initial frames for all parts depending on first timestamps
        for (int x=0; x < subDirCnt; x++)
        {
            utilities->initialFrame.push_back(utilities->partDetails[x].currFrame);

            double totalTime = 0.0;
            double final = utilities->partDetails[x].timestamp[utilities->partDetails[x].timestamp.length()-1];
            double initial = utilities->partDetails[x].timestamp[utilities->partDetails[x].currFrame];

            totalTime = final - initial;
            if (verbose)
            {
                yInfo() << "The part " << utilities->partDetails[x].name << " should last for: " << totalTime
                        << " with " << utilities->partDetails[x].maxFrame << " frames";
            }
            utilities->configurePorts(utilities->partDetails[x]);
        }

        utilities->dataplayerEngine->setNumPart(subDirCnt);


        if (subDirCnt > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**********************************************************/
    bool play() override
    {
        if (subDirCnt > 0)
        {
            bool allPartsStatus = utilities->dataplayerEngine->getAllPartsStatus();
            if (allPartsStatus)
            {
                if (verbose)
                {
                    yInfo() << "asking the threads to stop...";
                }
                if (utilities->dataplayerEngine->isSuspended())
                {
                    utilities->dataplayerEngine->resume();
                }

                utilities->dataplayerEngine->stop();
                if (verbose)
                {
                    yInfo() << "done stopping!";
                }
                for (int i=0; i < subDirCnt; i++)
                    utilities->partDetails[i].currFrame = 1;

                if (verbose)
                {
                    yInfo() << "done stopping the thread...";
                }
                utilities->dataplayerEngine->setAllPartsStatus(false);
            }

            if ( utilities->dataplayerEngine->isSuspended() )
            {
                if (verbose)
                {
                    yInfo() << "asking the thread to resume";
                }

                for (int i=0; i < subDirCnt; i++)
                    utilities->partDetails[i].worker->resetTime();

                utilities->dataplayerEngine->resume();
            }
            else if (!utilities->dataplayerEngine->isRunning())
            {
                if (verbose)
                {
                    yInfo() << "asking the thread to start";
                    yInfo() <<"initializing the workers...";
                }

                for (int i=0; i < subDirCnt; i++)
                    utilities->partDetails[i].worker->init();

                if (verbose)
                {
                    yInfo() << "starting the master thread...";
                }
                utilities->dataplayerEngine->start();
            }
            status = "playing";
            return true;
        }
        else
        {
            if (verbose)
            {
                yError() << "No dataset loaded";
            }
            return false;
        }
    }

    /**********************************************************/
    bool pause() override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "asking the threads to pause...";
            }
            utilities->dataplayerEngine->pause();
            status = "paused";
            return true;
        }
        else
        {
            if (verbose)
            {
                yError() << "No dataset loaded";
            }
            return false;
        }
    }

    /**********************************************************/
    bool resume() override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "asking the threads to resume...";
            }
            utilities->dataplayerEngine->resume();
            return true;
        }
        else
        {
            if (verbose)
            {
                yError() << "No dataset loaded";
            }
            return false;
        }
    }

    /**********************************************************/
    bool stop() override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "asking the threads to stop...";
            }
            if (utilities->dataplayerEngine->isSuspended()){
                utilities->dataplayerEngine->resume();
            }

            utilities->dataplayerEngine->stop();
            if (verbose)
            {
                yInfo() << "done stopping!";
            }
            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].currFrame = 1;

            if (verbose)
            {
                yInfo() << "done stopping the thread...";
            }
            status = "stopped";
            return true;
        }
        else
        {
            if (verbose)
            {
                yError() << "No dataset loaded";
            }
            return false;
        }
    }

    /**********************************************************/
    bool enable(const string &part) override
    {
        for (int i=0; i < subDirCnt; i++)
        {
            string partname = utilities->partDetails[i].name;
            if (strcmp( partname.c_str() , part.c_str()) == 0)
            {
                if (verbose)
                {
                    yInfo() << "Enabling" << partname;
                }
                utilities->dataplayerEngine->setPart(i, true);
                return true;
                break;
            }
        }
        return false;
    }


    /**********************************************************/
    bool disable(const string &part) override
    {
        for (int i=0; i < subDirCnt; i++)
        {
            string partname = utilities->partDetails[i].name;
            if (strcmp( partname.c_str() , part.c_str()) == 0)
            {
                if (verbose)
                {
                    yInfo() << "Disabling" << partname;
                }
                utilities->dataplayerEngine->setPart(i, false);
                return true;
                break;
            }
        }
        return false;
    }

    /**********************************************************/
    vector<string> getAllParts() override
    {
        vector<string> parts(subDirCnt, "");
        if (subDirCnt > 0)
        {
            for (int i=0; i < subDirCnt; i++)
            {
                string partname = utilities->partDetails[i].name;
                parts[i] = partname;
            }
        }
        return parts;
    }

    /**********************************************************/
    string getPortName(const string &part) override
    {
        string portname = "";
        if (subDirCnt > 0)
        {
            for (int i=0; i < subDirCnt; i++)
            {
                if (utilities->partDetails[i].name == part)
                {
                    portname = utilities->partDetails[i].portName;
                }
            }
        }
        return portname;
    }

    /**********************************************************/
    bool setPortName(const string &part, const string &new_name) override
    {
        if (subDirCnt > 0)
        {
            for (int i=0; i < subDirCnt; i++)
            {
                string partname = utilities->partDetails[i].name;
                if (partname == part)
                {
                    utilities->partDetails[i].portName = new_name;
                    utilities->configurePorts(utilities->partDetails[i]);
                    return true;
                }
            }
        }
        return false;
    }

    /**********************************************************/
    int getFrame(const string &partname) override
    {
        if (subDirCnt > 0)
        {
            for (int i=0; i < subDirCnt; i++)
            {
                if (strcmp (partname.c_str(), utilities->partDetails[i].name.c_str()) == 0)
                {
                    return utilities->partDetails[i].currFrame;
                }
            }
        }
        return -1;
    }

    /**********************************************************/
    bool setFrame(const int frameNum) override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "setting initial frame to " << frameNum;
            }
            for (int i=0; i < subDirCnt; i++)
            {
                utilities->dataplayerEngine->virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
                utilities->partDetails[i].currFrame = frameNum;
            }
            utilities->dataplayerEngine->virtualTime = utilities->partDetails[0].timestamp[utilities->partDetails[0].currFrame];
            return true;
        }

        return false;
    }

    /**********************************************************/
    bool setSpeed(const double speed) override
    {
        if (verbose)
        {
            yInfo() << "Setting speed to" << speed;
        }
        utilities->speed = speed;
        return true;
    }

    /**********************************************************/
    double getSpeed() override
    {
        return utilities->speed;
    }

    /**********************************************************/
    bool step() override
    {
        if (subDirCnt > 0)
        {
            utilities->stepThread();
            return true;
        }
        return false;
    }

    /**********************************************************/
    bool repeat(const bool val=false) override
    {
        if (verbose)
        {
            yInfo() << "Setting repeat mode to" << val;
        }
        utilities->repeat = val;
        return true;
    }

    /**********************************************************/
    bool setStrict(const bool val=false) override
    {
        if (verbose)
        {
            yInfo() << "Setting strict mode to" << val;
        }
        utilities->sendStrict = val;
        return true;
    }

    /**********************************************************/
    bool forward(const int steps=5) override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "Going forward of" << steps << "steps";
            }
            utilities->dataplayerEngine->forward(steps);
            return true;
        }
        return false;
    }

    /**********************************************************/
    bool backward(const int steps=5) override
    {
        if (subDirCnt > 0)
        {
            if (verbose)
            {
                yInfo() << "Going backward of" << steps << "steps";
            }
            utilities->dataplayerEngine->backward(steps);
            return true;
        }
        return false;
    }

    /**********************************************************/
    double getProgress() override
    {
        if (subDirCnt > 0)
        {
            double prog = 0.0;
            int nactivParts = 0;
            for (int x=0; x < subDirCnt; x++)
            {
                if (utilities->dataplayerEngine->isPartActive[x])
                {
                    prog += ((utilities->partDetails[x].currFrame * 100) / utilities->partDetails[x].maxFrame);
                    nactivParts++;
                }
            }

            prog /= nactivParts;
            return prog;
        }

        if (verbose)
        {
            yError() << "Dataset not loaded";
        }
        return -1.0;
    }

    /**********************************************************/
    string getStatus() override
    {
        return status;
    }

    /**********************************************************/
    bool quit() override
    {
        if(utilities)
        {
            if (verbose)
            {
                yInfo() << "asking the threads to stop...";
            }
            if (utilities->dataplayerEngine->isSuspended()){
                utilities->dataplayerEngine->resume();

            }
            utilities->dataplayerEngine->stop();
            if (verbose)
            {
                yInfo() << "done stopping!";
            }
            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].currFrame = 1;

            if (verbose)
            {
                yInfo() << "Module closing...";
                yInfo() << "Cleaning up...";
            }
            for (int x=0; x < subDirCnt; x++)
            {
                utilities->partDetails[x].worker->release();
            }
            if (verbose)
            {
                yInfo() <<  "Attempt to interrupt ports";
            }
            for (int x=0; x < subDirCnt; x++)
            {
                utilities->interruptPorts(utilities->partDetails[x]);
            }
            if (verbose)
            {
                yInfo() << "Attempt to close ports";
            }
            for (int x=0; x < subDirCnt; x++)
            {
                utilities->closePorts(utilities->partDetails[x]);
            }

            if(utilities)
            {
                delete utilities;
                utilities = nullptr;
            }
            if (verbose)
            {
                yInfo() <<  "Done!...";
            }
        }
        return true;
    }

};

int main(int argc, char *argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"ERROR: check YARP network.\n");
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.configure(argc,argv);

    dataplayer_module module;
    return module.runModule(rf);
}
