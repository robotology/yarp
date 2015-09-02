/*
 * Copyright (C) 2010 RobotCub Consortium
 * Author: Ugo Pattacini <ugo.pattacini@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <deque>

#ifdef ADD_VIDEO
    #include <opencv2/opencv.hpp>
#endif

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;


/**************************************************************************/
typedef enum { bottle, image } DumpType;


// Abstract object definition for queueing
/**************************************************************************/
class DumpObj
{
public:
    virtual ~DumpObj() { }
    virtual const string toFile(const string&, unsigned int) = 0;
    virtual void *getPtr() = 0;
};


// Specialization for Bottle object
/**************************************************************************/
class DumpBottle : public DumpObj
{
private:
    Bottle *p;

public:
    DumpBottle() { p=new Bottle; }
    DumpBottle(const DumpBottle &obj) { p=new Bottle(*(obj.p)); }
    DumpBottle(const Bottle &b) { p=new Bottle(b); }
    const DumpBottle &operator=(const DumpBottle &obj) { *p=*(obj.p); return *this; }
    ~DumpBottle() { delete p; }

    const string toFile(const string &dirName, unsigned int cnt)
    {
        string ret=p->toString().c_str();
        return ret;
    }

    void *getPtr() { return NULL; }
};


// Object creator of type Bottle - overloaded
/**************************************************************************/
DumpObj *factory(Bottle& obj)
{
    DumpBottle *p=new DumpBottle(obj);
    return p;
}


// Specialization for Image object
/**************************************************************************/
class DumpImage : public DumpObj
{
private:
    Image *p;

public:
    DumpImage() { p=new Image(); }
    DumpImage(const DumpImage &obj) { p=new Image(*(obj.p)); }
    DumpImage(const Image &img) { p=new Image(img); }
    const DumpImage &operator=(const DumpImage &obj) { *p=*(obj.p); return *this; }
    ~DumpImage() { delete p; }

    const string toFile(const string &dirName, unsigned int cnt)
    {
        int code=p->getPixelCode();
        string ext;

        if (code==VOCAB_PIXEL_MONO_FLOAT)
            ext=".float";
        else if (code==VOCAB_PIXEL_MONO)
            ext=".pgm";
        else
            ext=".ppm";

        ostringstream fName;
        fName << setw(8) << setfill('0') << cnt << ext;
        
        string extfName=dirName;
        extfName+="/";
        extfName+=fName.str();
        file::write(*p,extfName.c_str());

        string ret=fName.str();
        ret+=" [";
        ret+=Vocab::decode(code).c_str();
        ret+="]";

        return ret;
    }

    void *getPtr() { return p->getIplImage(); }
};


// Object creator of type Image - overloaded
/**************************************************************************/
DumpObj *factory(Image &obj)
{
    DumpImage *p=new DumpImage(obj);
    return p;
}


// Class to manage tx and rx time stamps
/**************************************************************************/
class DumpTimeStamp
{
    double rxStamp;
    double txStamp;
    bool   rxOk;
    bool   txOk;

public:
    DumpTimeStamp() : rxOk(false), txOk(false) { }
    void setRxStamp(const double stamp) { rxStamp=stamp; rxOk=true; }
    void setTxStamp(const double stamp) { txStamp=stamp; txOk=true; }
    double getStamp() const
    {
        if (txOk)
            return txStamp;
        else if (rxOk)
            return rxStamp;
        else
            return -1.0;
    }
    string getString() const
    {
        ostringstream ret;
        ret<<fixed;

        if (txOk)
            ret<<txStamp;
        if (rxOk)
        {
            if (!ret.str().empty())
                ret<<' ';
            ret<<rxStamp;
        }
        return ret.str();
    }
};


// Definition of item to be put in the queue
/**************************************************************************/
typedef struct
{
    int            seqNumber;
    DumpTimeStamp  timeStamp;
    DumpObj       *obj;
} DumpItem;


// Definition of the queue
// Two services act on this resource:
// 1) the port, which listens to incoming data
// 2) the thread, which stores the data to disk
/**************************************************************************/
class DumpQueue : public deque<DumpItem>
{
private:
    Mutex mutex;

public:
    void lock()   { mutex.lock();   }
    void unlock() { mutex.unlock(); }
};


/**************************************************************************/
template <class T>
class DumpPort : public BufferedPort<T>
{
public:
    DumpPort(DumpQueue &Q, unsigned int _dwnsample=1,
             bool _rxTime=true, bool _txTime=false) : buf(Q)
    {
        rxTime=_rxTime;
        txTime=_txTime;
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
    bool txTime;

    void onRead(T &obj)
    {
        if (++cnt==dwnsample)
        {
            if (firstIncomingData)
            {
                yInfo() << "Incoming data detected";
                firstIncomingData=false;
            }

            DumpItem item;
            Stamp info;

            BufferedPort<T>::getEnvelope(info);
            item.seqNumber=info.getCount();

            if (txTime || (info.isValid() && !rxTime))
                item.timeStamp.setTxStamp(info.getTime());

            if (rxTime || !info.isValid())
                item.timeStamp.setRxStamp(Time::now());

            item.obj=factory(obj);

            buf.lock();
            buf.push_back(item);
            buf.unlock();

            cnt=0;
        }
    }
};


/**************************************************************************/
class DumpThread : public RateThread
{
private:
    DumpQueue      &buf;
    DumpType        type;
    ofstream        finfo;
    ofstream        fdata;
    string          dirName;
    string          infoFile;
    string          dataFile;
    unsigned int    blockSize;
    unsigned int    cumulSize;
    unsigned int    counter;
    double          oldTime;

    bool            saveData;
    bool            videoOn;
    string          videoType;
    bool            closing;

#ifdef ADD_VIDEO
    ofstream        ftimecodes;
    string          videoFile;
    string          timecodesFile;
    double          t0;
    bool            doImgParamsExtraction;
    bool            doSaveFrame;
    cv::VideoWriter videoWriter;
#endif

public:
    DumpThread(DumpType _type, DumpQueue &Q, const string &_dirName, int szToWrite,
               bool _saveData, bool _videoOn, const string &_videoType) :
               RateThread(50), buf(Q), type(_type), dirName(_dirName),
               blockSize(szToWrite), saveData(_saveData),
               videoOn(_videoOn), videoType(_videoType)
    {
        infoFile=dirName;
        infoFile+="/info.log";

        dataFile=dirName;
        dataFile+="/data.log";

    #ifdef ADD_VIDEO
        transform(videoType.begin(),videoType.end(),videoType.begin(),::tolower);
        if ((videoType!="mkv") && (videoType!="avi"))
        {
            yWarning() << "unknown video type '" << videoType << "' specified; "
                       << "'mkv' type will be used.";
            videoType="mkv";
        }

        videoFile=dirName;
        videoFile+="/video.";
        videoFile+=videoType;

        timecodesFile=dirName;
        timecodesFile+="/timecodes.log";

        doImgParamsExtraction=videoOn;
        doSaveFrame=false;
    #endif
    }

    void writeSource(const string &sourceName, const bool connected)
    {
        finfo << "[" << fixed << Time::now() << "] ";
        finfo << sourceName << " ";
        finfo << (connected?"[connected]":"[disconnected]") << endl;
    }

    bool threadInit()
    {
        oldTime=Time::now();
        cumulSize=0;
        counter=0;
        closing=false;

        finfo.open(infoFile.c_str());
        if (!finfo.is_open())
        {
            yError() << "unable to open file: " << infoFile;
            return false;
        }

        finfo<<"Type: ";
        if (type==bottle)
            finfo<<"Bottle;";
        else if (type==image)
        {
            finfo<<"Image;";
            if (videoOn)
                finfo<<" Video:"<<videoType<<"(huffyuv);";
        }
        finfo<<endl;

        fdata.open(dataFile.c_str());
        if (!fdata.is_open())
        {
            yError() << "unable to open file: " << dataFile;
            return false;
        }

    #ifdef ADD_VIDEO
        if (videoOn)
        {
            ftimecodes.open(timecodesFile.c_str());
            if (!ftimecodes.is_open())
            {
                yError() << "unable to open file: " << timecodesFile;
                return false;
            }
            ftimecodes<<"# timecode format v2"<<endl;
        }
    #endif

        return true;
    }

    void run()
    {
        buf.lock();
        unsigned int sz=buf.size(); //!!! access to size must be protected: problem spotted with Linux stl
        buf.unlock();

        // each 10 seconds it issues a writeToDisk command straightaway
        bool writeToDisk=false;
        double curTime=Time::now();
        if ((curTime-oldTime>10.0) || closing)
        {
            writeToDisk=sz>0;
            oldTime=curTime;
        }

        // it performs the writeToDisk on command or as soon as
        // the queue size is greater than the given threshold
        if ((sz>blockSize) || writeToDisk)
        {
        #ifdef ADD_VIDEO
            // extract images parameters just once
            if (doImgParamsExtraction && (sz>1))
            {
                buf.lock();
                DumpItem itemFront=buf.front();
                DumpItem itemEnd=buf.back();
                buf.unlock();

                int fps;
                int frameW=((IplImage*)itemEnd.obj->getPtr())->width;
                int frameH=((IplImage*)itemEnd.obj->getPtr())->height;

                t0=itemFront.timeStamp.getStamp();
                double dt=itemEnd.timeStamp.getStamp()-t0;
                if (dt<=0.0)
                    fps=25; // default
                else
                    fps=int(double(sz-1)/dt);

                videoWriter.open(videoFile.c_str(),CV_FOURCC('H','F','Y','U'),
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
                buf.pop_front();
                buf.unlock();

                fdata << item.seqNumber << ' ' << item.timeStamp.getString() << ' ';
                if (saveData)
                    fdata << item.obj->toFile(dirName,counter++) << endl;
                else
                {
                    ostringstream frame;
                    frame << "frame_" << setw(8) << setfill('0') << counter++;
                    fdata << frame.str() << endl;
                }

            #ifdef ADD_VIDEO
                if (doSaveFrame)
                {
                    cv::Mat img=cv::cvarrToMat((IplImage*)item.obj->getPtr());
                    videoWriter<<img;

                    // write the timecode of the frame
                    int dt=(int)(1000.0*(item.timeStamp.getStamp()-t0));
                    ftimecodes << dt << endl;
                }
            #endif

                delete item.obj;
            }

            cumulSize+=sz;
            yInfo() << sz << " items stored [cumul #: " << cumulSize << "]";
        }
    }

    void threadRelease()
    {
        // call run() for the last time to flush the queue
        closing=true;
        run();

        finfo.close();
        fdata.close();

    #ifdef ADD_VIDEO
        if (videoOn)
            ftimecodes.close();
    #endif
    }
};


/**************************************************************************/
class DumpReporter : public PortReport
{
private:
    DumpThread *thread;

public:
    DumpReporter() : thread(NULL) { }
    void setThread(DumpThread *thread) { this->thread=thread; }
    void report(const PortInfo &info)
    {
        if ((thread!=NULL) && info.incoming)
            thread->writeSource(info.sourceName.c_str(),info.created);
    }
};


/**************************************************************************/
class DumpModule: public RFModule
{
private:
    DumpQueue        *q;
    DumpPort<Bottle> *p_bottle;
    DumpPort<Image>  *p_image;
    DumpThread       *t;
    DumpReporter      reporter;
    Port              rpcPort;
    DumpType          type;
    bool              rxTime;
    bool              txTime;
    unsigned int      dwnsample;
    string            portName;

public:
    DumpModule() { }

    bool configure(ResourceFinder &rf)
    {
        Time::turboBoost();

        portName=rf.check("name",Value("/dump")).asString().c_str();
        if (portName[0]!='/')
            portName="/"+portName;

        bool saveData=true;
        bool videoOn=false;
        string videoType=rf.check("videoType",Value("mkv")).asString().c_str();

        if (rf.check("type"))
        {
            string optTypeName=rf.find("type").asString().c_str();
            if (optTypeName=="bottle")
                type=bottle;
            else if (optTypeName=="image")
            {
                type=image;
            #ifdef ADD_VIDEO
                if (rf.check("addVideo"))
                    videoOn=true;
            #endif
            }
        #ifdef ADD_VIDEO
            else if (optTypeName=="video")
            {
                type=image;
                videoOn=true;
                saveData=false;
            }
        #endif
            else
            {
                yError() << "Error: invalid type";
                return false;
            }
        }
        else
            type=bottle;

        dwnsample=rf.check("downsample",Value(1)).asInt();
        rxTime=rf.check("rxTime");
        txTime=rf.check("txTime");
        string templateDirName=rf.check("dir")?rf.find("dir").asString().c_str():portName;
        if (templateDirName[0]!='/')
            templateDirName="/"+templateDirName;

        string dirName;
        if (rf.check("overwrite"))
            dirName="."+templateDirName;
        else
        {
            // look for a proper directory
            int i=0;
            do
            {
                ostringstream checkDirName;
                if (i>0)
                    checkDirName << "." << templateDirName << "_" << setw(5) << setfill('0') << i;
                else
                    checkDirName << "." << templateDirName;

                dirName=checkDirName.str();
                i++;
            }
            while (!yarp::os::stat(dirName.c_str()));
        }
        yarp::os::mkdir_p(dirName.c_str());

        q=new DumpQueue();
        t=new DumpThread(type,*q,dirName.c_str(),100,saveData,videoOn,videoType);

        if (!t->start())
        {
            delete t;
            delete q;

            return false;
        }

        reporter.setThread(t);

        if (type==bottle)
        {
            p_bottle=new DumpPort<Bottle>(*q,dwnsample,rxTime,txTime);
            p_bottle->useCallback();
            p_bottle->open(portName.c_str());
            p_bottle->setStrict();
            p_bottle->setReporter(reporter);
        }
        else
        {
            p_image=new DumpPort<Image>(*q,dwnsample,rxTime,txTime);
            p_image->useCallback();
            p_image->open(portName.c_str());
            p_image->setStrict();
            p_image->setReporter(reporter);
        }

        if (rf.check("connect"))
        {
            string srcPort=rf.find("connect").asString().c_str();
            bool ok=Network::connect(srcPort.c_str(),
                                     (type==bottle)?p_bottle->getName().c_str():
                                     p_image->getName().c_str(),"tcp");

            ostringstream msg;
            msg << "Connection to " << srcPort << " " << (ok?"successful":"failed");

            if (ok)
                yInfo() << msg.str();
            else
                yWarning() << msg.str();
        }

        // this port serves to handle the "quit" rpc command
        rpcPort.open((portName+"/rpc").c_str());
        attach(rpcPort);

        yInfo() << "Service yarp port: " << portName;
        yInfo() << "Data stored in   : " << dirName;

        return true;
    }

    bool close()
    {
        t->stop();

        if (type==bottle)
        {
            p_bottle->interrupt();
            p_bottle->close();
            delete p_bottle;
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

    double getPeriod()    { return 1.0;  }
    bool   updateModule() { return true; }
};


/**************************************************************************/
int main(int argc, char *argv[])
{
    Network yarp;

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.configure(argc,argv);

    if (rf.check("help"))
    {
        yInfo() << "Options:";
        yInfo() << "\t--name       port: service port name (default: /dump)";
        yInfo() << "\t--connect    port: name of the port to connect the dumper to at launch time";
        yInfo() << "\t--dir        name: provide explicit name of storage directory";
        yInfo() << "\t--overwrite      : overwrite pre-existing storage directory";
    #ifdef ADD_VIDEO
        yInfo() << "\t--type       type: type of the data to be dumped [bottle(default), image, video]";
        yInfo() << "\t--addVideo       : produce video as well (if image is selected)";
        yInfo() << "\t--videoType   ext: produce video of specified container type [mkv(default), avi]";
    #else
        yInfo() << "\t--type       type: type of the data to be dumped [bottle(default), image]";
    #endif
        yInfo() << "\t--downsample    n: downsample rate (default: 1 => downsample disabled)";
        yInfo() << "\t--rxTime         : dump the receiver time instead of the sender time";
        yInfo() << "\t--txTime         : dump the sender time straightaway";
        yInfo();

        return 0;
    }

    if (!yarp.checkNetwork())
    {
        yError()<<"YARP server not available!";
        return 1;
    }

    DumpModule mod;
    return mod.runModule(rf);
}
