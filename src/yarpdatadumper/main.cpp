/**
@ingroup icub_tools

\defgroup dataDumper dataDumper
 
Acquires and stores Vectors or Images and Videos from a YARP 
port. 

Copyright (C) 2008 RobotCub Consortium
 
Author: Ugo Pattacini 
 
Date: first release 16/05/2008 

CopyPolicy: Released under the terms of the GNU GPL v2.0.

\section intro_sec Description

When launched, the service monitors the presence of incoming 
data (vectors or images) and stores it within a folder called 
with the same name as the service port (or with a proper suffix 
appended if other data is present in the current path). In this 
folder the file 'data.log' contains the information (taken from 
the envelope field of the port) line by line as follows: 
 
\code 
[pck id] [time stamp] [vector (or image_file_name)] 
   0       1.234         0 1 2 3 ...
   1       1.235         4 5 6 7 ...
   ...     ...           ...
\endcode 
 
Note that if the envelope is not valid, then the Time Stamp is 
the reference time of the machine where the service is running. 
Anyway, a selection between these two Time Stamps is available 
for the user through --rxTime option. 
 
\section lib_sec Libraries 
YARP libraries and OpenCV (if found)

\section parameters_sec Parameters
--name \e portname 
- The parameter \e portname identifies the name of the listening
  port open by the service; if not specified \e /dump is
  assumed.
 
--type \e datatype 
- The parameter \e datatype selects the type of items to be 
  stored. It can be \e vector, \e image or \e video; if not
  specified \e vector is assumed. Note that images are stored as
  ppm files. If \e video is choosen then only the file
  'video.avi' is produced neither 'data.log' nor the single
  images. The data type \e video is available iff OpenCV is
  found and the codec MPEG-4 is installed.
 
--addVideo
- In case images are acquired with this option enabled, a video 
  called 'video.avi' is also produced at the same time (the
  codec used is MPEG-4). This option is available iff OpenCV is
  found.
 
--downsample \e n 
- With this option it is possible to reduce the storing rate by 
  a factor \e n, i.e. the parameter \e n specifies how many
  items (vectors or images) shall be skipped after one
  acquisition.

--rxTime
- With this option it is possible to select which Time Stamp to
  assign to the dumped data: normally the sender time is the
  reference, but if this option is enabled, the time of the
  receiving machine on which the service is running will be the
  reference. However, even if --rxTime is not selected, the
  receiver time will be taken as reference in case of invalid
  message envelope.
 
\section portsa_sec Ports Accessed
The port the service is listening to.

\section portsc_sec Ports Created
 
- \e <portname> (e.g. /dump) 
 
- \e <portname>/rpc which is a remote procedure call port useful
  to shut down the service remotely by sending to this port the
  'quit' command.

\section in_files_sec Input Data Files
None.

\section out_data_sec Output Data Files
Within the directory \e ./<portname> the file \e data.log is 
created containing the acquisitions report. Beside this, if \e 
image type has been selected, all the acquired images are also 
stored. 
 
Note that in case an acquisition with the same \e <portname> was
previously carried out, an increasing suffix will be appended 
to the name of the directory.
 
\section conf_file_sec Configuration Files
None. 
 
\section tested_os_sec Tested OS
Linux and Windows. 
On Linux platforms, in order to have the video acquisition 
properly working you should check that OpenCV finds the ffmpeg
package correctly installed: the quickest way to achieve that is 
to install the ffmepg prior to the OpenCV installation. 

\section example_sec Example
By launching the following command: 
 
\code 
dataDumper --name /example --type image --downsample 2 
\endcode 
 
the service will create the listening port /example capable of 
storing images in ppm format within the subdirectory ./example 
at a halved rate with respect to the rate of the sender. 
 
Then, by connecting the port to the sender with the usual yarp 
command
 
\code 
yarp connect /grabber /example 
\endcode 
 
the acquisition will start. 
 
By pressing CTRL+C the acquisition is terminated.
 
So, now, have a look inside the directory ./example 

\author Ugo Pattacini

This file can be edited at \in src/dataDumper/main.cpp.
*/ 

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/os/Os.h>

#ifdef ADD_VIDEO
    #include <cv.h>
    #include <highgui.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <deque>

using namespace std;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;


typedef enum { vect, image } DumpType;

// Abstract object definition for queueing
class DumpObj
{
public:
    virtual ~DumpObj() { }
    virtual const string toFile(const char*, unsigned int) = 0;
    virtual void *getPtr() = 0;
};


// Specialization for Vector object
class DumpVect : public DumpObj
{
private:
    Vector *p;

public:
    DumpVect() { p=new Vector(); }
    DumpVect(const DumpVect &obj) { p=new Vector(*(obj.p)); }
    DumpVect(const Vector &v) { p=new Vector(v); }
    const DumpVect &operator=(const DumpVect &obj) { *p=*(obj.p); return *this; }
    ~DumpVect() { delete p; }

    virtual const string toFile(const char *dirName, unsigned int cnt)
    {
        string ret;
        ret=p->toString();

        return ret;
    }

    virtual void *getPtr() { return NULL; }
};

// Object creator of type Vector - overloaded
DumpObj *factory(Vector& obj)
{
    DumpVect *p=new DumpVect(obj);
    return p;
}


// Specialization for Image object
class DumpImage : public DumpObj
{
private:
    ImageOf<PixelBgr> *p;

public:
    DumpImage() { p=new ImageOf<PixelBgr>(); }
    DumpImage(const DumpImage &obj) { p=new ImageOf<PixelBgr>(*(obj.p)); }
    DumpImage(const ImageOf<PixelBgr> &img) { p=new ImageOf<PixelBgr>(img); }
    const DumpImage &operator=(const DumpImage &obj) { *p=*(obj.p); return *this; }
    ~DumpImage() { delete p; }

    virtual const string toFile(const char *dirName, unsigned int cnt)
    {
        string ret;
        char extfName[255];
        char fName[255];

        sprintf(fName,"%.8d.ppm",cnt);
        ret=fName;

        sprintf(extfName,"%s/%s",dirName,fName);
        file::write(*p,extfName);

        return ret;
    }

    virtual void *getPtr() { return p->getIplImage(); }
};

// Object creator of type Image - overloaded
DumpObj *factory(ImageOf<PixelBgr> &obj)
{
    DumpImage *p=new DumpImage(obj);
    return p;
}


// Definition of item to be put in the queue
typedef struct
{
    int     seqNumber;
    double  timeStamp;
    DumpObj *obj;
} DumpItem;

// Definition of the queue
// Two services act on this resource:
// 1) the port, which listens to incoming data
// 2) the thread, which stores the data to disk
class DumpQueue : public deque<DumpItem>
{
private:
    Semaphore *mutex;

public:
    DumpQueue()   { mutex=new Semaphore; }
    ~DumpQueue()  { delete mutex;        }
    void lock()   { mutex->wait();       }
    void unlock() { mutex->post();       }
};


template <class T>
class DumpPort : public BufferedPort<T>
{
public:
    DumpPort(DumpQueue &Q, unsigned int _dwnsample=1, bool _rxTime=true) : buf(Q)
    {
        rxTime=_rxTime;
        dwnsample=_dwnsample>0?_dwnsample:1;
        cnt=0;

        firstIncomingData=true;
    }

private:
    DumpQueue &buf;
    unsigned int dwnsample;
    unsigned int cnt;
    bool firstIncomingData;
    bool rxTime;

    virtual void onRead(T &obj)
    {
        if (++cnt==dwnsample)
        {
            if (firstIncomingData)
            {
                cout << "Incoming data detected" << endl;
                firstIncomingData=false;
            }

            DumpItem item;
            Stamp info;
    
            BufferedPort<T>::getEnvelope(info);
    
            item.seqNumber=info.getCount();
            if (!info.isValid() || rxTime)
                item.timeStamp=Time::now();
            else
                item.timeStamp=info.getTime();

            item.obj=factory(obj);
    
            buf.lock();
            buf.push_back(item);
            buf.unlock();

            cnt=0;
        }
    }
};



class DumpThread : public Thread
{
private:
    DumpQueue     &buf;
    ofstream       fout;
    char           dirName[255];
    char           logFile[255];
    unsigned int   blockSize;
    unsigned int   counter;
    double         oldTime;
    
    bool           saveData;
    bool           videoOn;        

#ifdef ADD_VIDEO
    char           videoFile[255];    
    bool           doImgParamsExtraction;
    bool           doSaveFrame;
    CvVideoWriter *videoWriter;
#endif

public:
    DumpThread(DumpQueue &Q, const char *_dirName, int szToWrite, bool _saveData, bool _videoOn) :
               buf(Q), blockSize(szToWrite), saveData(_saveData), videoOn(_videoOn)
    {
        strcpy(dirName,_dirName);
        strcpy(logFile,dirName);        
        strcat(logFile,"/data.log");        

    #ifdef ADD_VIDEO
        strcpy(videoFile,dirName);
        strcat(videoFile,"/video.avi");

        doImgParamsExtraction=videoOn;
        doSaveFrame=false;
    #endif
    }

    virtual bool threadInit()
    {
        oldTime=Time::now();
        counter=0;

        if (saveData)
        {
            fout.open(logFile);
            bool ret=fout.is_open();

            if (!ret)
                cout << "unable to open file" << endl;

            return ret;
        }
        else
            return true;
    }

    virtual void run()
    {
        while (!isStopping())
        {
            bool writeToDisk=false;

            buf.lock();
            unsigned int sz=buf.size(); //!!! access to size must be proteceted: problem spotted with Linux stl
            buf.unlock();

            // each 10 seconds it issues a writeToDisk command straightaway
            double curTime=Time::now();
            if (curTime-oldTime>10.0)
            {
				writeToDisk=sz>0;
                oldTime=curTime;
            }

            // it performs the writeToDisk on command or as soon as
            // the queue size is greater than the given threshold
            if (sz>blockSize || writeToDisk)
            {
                static unsigned int cumulSize=0;

            #ifdef ADD_VIDEO
                // extract images parameters just once
                if (doImgParamsExtraction && sz>1)
                {
                    buf.lock();
                    DumpItem itemFront=buf.front();
                    DumpItem itemEnd=buf.back();
                    buf.unlock();

                    int fps;
                    int frameW=((IplImage*)itemEnd.obj->getPtr())->width;
                    int frameH=((IplImage*)itemEnd.obj->getPtr())->height;                    

                    double dt=itemEnd.timeStamp-itemFront.timeStamp;
                    if (dt<=0.0)
                        fps=25; // default
                    else
                    {
                        double _fps=sz/dt;
                        fps=(int)_fps;

                        if (_fps-fps>=0.5)
                            fps++;
                    }

                    videoWriter=cvCreateVideoWriter(videoFile,CV_FOURCC('D','I','V','X'),
                                                    fps,cvSize(frameW,frameH),true);

                    doImgParamsExtraction=false;
                    doSaveFrame=true;
                }
            #endif

                // save to disk
                for (unsigned int i=0; i<sz; i++)
                {
                    buf.lock();
                    DumpItem item=buf.front();
                    buf.unlock();

                    if (saveData)
                    {
                        fout << item.seqNumber << '\t' << fixed << item.timeStamp << '\t';
                        fout << item.obj->toFile(dirName,counter++) << endl;
                    }

                #ifdef ADD_VIDEO
                    if (doSaveFrame)
                        cvWriteFrame(videoWriter,(IplImage*)item.obj->getPtr());
                #endif

                    delete item.obj;

                    buf.lock();
                    buf.pop_front();
                    buf.unlock();
                }

                cumulSize+=sz;
                cout << sz << " items stored [cumul #: " << cumulSize << "]" << endl;
            }

            Time::delay(0.05);
        }
    }

    virtual void threadRelease()
    {
        if (saveData)
            fout.close();

    #ifdef ADD_VIDEO
        if (doSaveFrame)
            cvReleaseVideoWriter(&videoWriter);
    #endif
    }
};



void createFullPath(const char*);

class DumpModule: public RFModule
{
private:
    DumpQueue                    *q;
    DumpPort<Vector>             *p_vect;
    DumpPort<ImageOf<PixelBgr> > *p_image;
    DumpThread                   *t;
    Port                         rpcPort;
    DumpType                     T;
    bool                         saveData;
    bool                         videoOn;
    bool                         rxTime;
    unsigned int                 dwnsample;
    char                         portName[255];

public:
    DumpModule() { }

    virtual bool configure(ResourceFinder &rf)
    {
        Time::turboBoost();

        if (rf.check("name"))
        {
            strcpy(portName,rf.find("name").asString().c_str());
            if (portName[0]!='/')
            {
                string slashedPortName='/'+string(portName);
                strcpy(portName,slashedPortName.c_str());
            }
        }
        else
            strcpy(portName,"/dump");

        saveData=true;
        videoOn=false;

        if (rf.check("type"))
        {
            string optTypeName;
            optTypeName=rf.find("type").asString().c_str();

            if (optTypeName=="vector")
                T=vect;
            else if (optTypeName=="image")
            {    
                T=image;
            #ifdef ADD_VIDEO
                if (rf.check("addVideo"))
                    videoOn=true;
            #endif
            }
        #ifdef ADD_VIDEO
            else if (optTypeName=="video")
            {    
                T=image;
                videoOn=true;
                saveData=false;
            }
        #endif
            else
            {
                cout << "Error: invalid type" << endl;
                return false;
            }
        }
        else
            T=vect;

        if (rf.check("downsample"))
            dwnsample=rf.find("downsample").asInt();
        else
            dwnsample=1;

        if (rf.check("rxTime"))
            rxTime=true;
        else
            rxTime=false;

        char dirName[255];
        bool proceed=true;

        // look for a proper directory
        for (int i=0; proceed; i++)
        {		
            if (i)
                sprintf(dirName,"./%s_%.5d",portName,i);
            else
                sprintf(dirName,"./%s",portName);

			proceed=!yarp::os::stat(dirName);
        }

        createFullPath(dirName);

        q=new DumpQueue();
        t=new DumpThread(*q,dirName,100,saveData,videoOn);

        if (!t->start())
        {
            delete t;
            delete q;

            return false;
        }

        if (T==vect)
        {
            p_vect=new DumpPort<Vector>(*q,dwnsample,rxTime);
            p_vect->useCallback();
            p_vect->open(portName);
        }
        else
        {
            p_image=new DumpPort<ImageOf<PixelBgr> >(*q,dwnsample,rxTime);
            p_image->useCallback();
            p_image->open(portName);
        }

        char rpcPortName[255];
        strcpy(rpcPortName,portName);
        strcat(rpcPortName,"/rpc");

        rpcPort.open(rpcPortName);
        attach(rpcPort);

        cout << "Service launched with the port name: " << portName << endl;

        return true;
    }

    virtual bool close()
    {
        t->stop();

        if (T==vect)
        {
            p_vect->interrupt();
            p_vect->close();
            delete p_vect;
        }
        else
        {
            p_image->interrupt();
            p_image->close();
            delete p_image;
        }

        rpcPort.interrupt();
        rpcPort.close();

        delete t;
        delete q;

        return true;
    }

    virtual double getPeriod()    { return 1.0;  }
    virtual bool   updateModule() { return true; }
};



void createFullPath(const char* path)
{
	if (yarp::os::stat(path))
    {
        string strPath=string(path);
        size_t found=strPath.find_last_of("/");
    
        while (strPath[found]=='/')
            found--;

        createFullPath(strPath.substr(0,found+1).c_str());
		yarp::os::mkdir(strPath.c_str());
    }
}



int main(int argc, char *argv[])
{
    ResourceFinder rf;
    rf.setVerbose(true);
    rf.configure("ICUB_ROOT",argc,argv);

    if (rf.check("help"))
    {
        cout << "Options:" << endl << endl;
        cout << "\t--name    port:\tservice port name (default: /dump)"                            << endl;        
    #ifdef ADD_VIDEO
        cout << "\t--type     typ:\ttype of the data to be dumped [vector(default), image, video]" << endl;
        cout << "\t--addVideo    :\tproduce video as well (if image is selected)"                  << endl;
    #else
        cout << "\t--type     typ:\ttype of the data to be dumped [vector(default), image]"        << endl;
    #endif
        cout << "\t--downsample n:\tdownsample rate (default: 1 => downsample disabled)"           << endl;
        cout << "\t--rxTime      :\tdumps the receiver time instead of the sender time"            << endl;

        return 0;
    }

    Network yarp;

    if (!yarp.checkNetwork())
        return -1;

    DumpModule mod;

    return mod.runModule(rf);
}


