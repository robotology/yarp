/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/all.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <deque>
#include <utility>
#include <mutex>
#include <algorithm>

#ifdef ADD_VIDEO
#    include <opencv2/opencv.hpp>
#    include <yarp/cv/Cv.h>
#    include <opencv2/core/core_c.h>
#    include <opencv2/videoio.hpp>
#endif // ADD_VIDEO


using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

#ifdef ADD_VIDEO
    using namespace yarp::cv;
#endif

/**************************************************************************/
enum class DumpFormat { bottle, image, image_jpg, image_png, depth, depth_compressed };

// Abstract object definition for queuing
/**************************************************************************/
class DumpObj
{
protected:
    DumpFormat m_dump_format{DumpFormat::bottle};

public:
    virtual ~DumpObj() = default;
    virtual const string toFile(const string&, unsigned int) = 0;
    virtual void attachFormat(const DumpFormat &format) { m_dump_format=format; }
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

    const string toFile(const string &dirName, unsigned int cnt) override
    {
        string ret=p->toString();
        return ret;
    }
};


// Object creator of type Bottle - overloaded
/**************************************************************************/
DumpObj *factory(Bottle& obj)
{
    auto* p=new DumpBottle(obj);
    return p;
}


// Specialization for Image object
/**************************************************************************/
class DumpImage : public DumpObj
{
private:
    Image *p;
#ifdef ADD_VIDEO
    cv::Mat img;
#endif

public:
    DumpImage() { p=new Image(); }
    DumpImage(const DumpImage &obj) { p=new Image(*(obj.p)); }
    DumpImage(const Image &img) { p=new Image(img); }
    const DumpImage &operator=(const DumpImage &obj) { *p=*(obj.p); return *this; }
    ~DumpImage() { delete p; }

    const string toFile(const string &dirName, unsigned int cnt) override
    {
        file::image_fileformat fileformat = file::FORMAT_NULL;
        string ext;

        int code=p->getPixelCode();
        switch (code)
        {
            //depth images
            case VOCAB_PIXEL_MONO_FLOAT:
                if (m_dump_format == DumpFormat::depth)
                {
                    fileformat = file::FORMAT_NUMERIC;
                    ext = ".float";
                }
                else if (m_dump_format == DumpFormat::depth_compressed)
                {
                    fileformat = file::FORMAT_NUMERIC_COMPRESSED;
                    ext = ".floatzip";
                }
            break;

            //grayscale images
            case VOCAB_PIXEL_MONO:
                if (m_dump_format == DumpFormat::image_png)
                {
                    fileformat = file::FORMAT_PNG;
                    ext = ".png";
                }
                else
                {
                    fileformat = file::FORMAT_PGM;
                    ext = ".pgm";
                }
            break;

            //rgb, bgr and other type of color images
            default:
                if (m_dump_format == DumpFormat::image_jpg)
                {
                    fileformat = file::FORMAT_JPG;
                    ext = ".jpg";
                }
                else if (m_dump_format == DumpFormat::image_png)
                {
                    fileformat = file::FORMAT_PNG;
                    ext = ".png";
                }
                else
                {
                    fileformat = file::FORMAT_PPM;
                    ext = ".ppm";
                }
            break;
        }

        ostringstream fName;
        fName << setw(8) << setfill('0') << cnt << ext;
        file::write(*p,dirName+"/"+fName.str(), fileformat);

        return (fName.str()+" ["+Vocab32::decode(code)+"]");
    }

#ifdef ADD_VIDEO
    const cv::Mat &getImage()
    {
        int code=p->getPixelCode();
        if (code==VOCAB_PIXEL_MONO_FLOAT)
        {
            img=toCvMat(*static_cast<ImageOf<PixelFloat>*>(p));
        }
        else if (code==VOCAB_PIXEL_MONO)
        {
            img=toCvMat(*static_cast<ImageOf<PixelMono>*>(p));
        }
        else
        {
            img=toCvMat(*static_cast<ImageOf<PixelRgb>*>(p));
        }
        return img;
    }
#endif
};


// Object creator of type Image - overloaded
/**************************************************************************/
DumpObj *factory(Image &obj)
{
    auto* p=new DumpImage(obj);
    return p;
}


// Class to manage tx and rx time stamps
/**************************************************************************/
class DumpTimeStamp
{
    double rxStamp{0.0};
    double txStamp{0.0};
    bool   rxOk{false};
    bool   txOk{false};

public:
    DumpTimeStamp() = default;

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
struct DumpItem
{
    int            seqNumber;
    DumpTimeStamp  timeStamp;
    DumpObj       *obj;
};


// Definition of the queue
// Two services act on this resource:
// 1) the port, which listens to incoming data
// 2) the thread, which stores the data to disk
/**************************************************************************/
class DumpQueue : public deque<DumpItem>
{
private:
    std::mutex mutex;

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
             bool _rxTime=true, bool _txTime=false, DumpFormat _dataformat= DumpFormat::bottle) : buf(Q)
    {
        rxTime=_rxTime;
        txTime=_txTime;
        dwnsample=_dwnsample>0?_dwnsample:1;
        cnt=0;
        itemformat = _dataformat;
        firstIncomingData=true;
    }

private:
    DumpQueue &buf;
    unsigned int dwnsample;
    unsigned int cnt;
    bool firstIncomingData;
    bool rxTime;
    bool txTime;
    DumpFormat itemformat;

    using BufferedPort<T>::onRead;
    void onRead(T &obj) override
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
            item.obj->attachFormat(itemformat);

            buf.lock();
            buf.push_back(item);
            buf.unlock();

            cnt=0;
        }
    }
};


/**************************************************************************/
class DumpThread : public PeriodicThread
{
private:
    DumpQueue      &buf;
    DumpFormat      type;
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
    bool            rxTime;
    bool            txTime;
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
    DumpThread(DumpFormat _type, DumpQueue &Q, const string &_dirName, const int szToWrite,
               const bool _saveData, const bool _videoOn, const string &_videoType,
               const bool _rxTime, const bool _txTime) :
        PeriodicThread(0.05),
        buf(Q),
        type(_type),
        dirName(std::move(_dirName)),
        blockSize(szToWrite),
        cumulSize(0),
        counter(0),
        oldTime(0.0),
        saveData(_saveData),
        videoOn(_videoOn),
        videoType(std::move(_videoType)),
        rxTime(_rxTime),
        txTime(_txTime),
        closing(false)
    {
        infoFile=dirName;
        infoFile+="/info.log";

        dataFile=dirName;
        dataFile+="/data.log";

    #ifdef ADD_VIDEO
        t0 = 0.0;
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

    bool threadInit() override
    {
        oldTime=Time::now();

        finfo.open(infoFile.c_str());
        if (!finfo.is_open())
        {
            yError() << "unable to open file: " << infoFile;
            return false;
        }

        finfo<<"Type: ";
        if (type== DumpFormat::bottle)
            finfo<<"Bottle;";
        else if (type== DumpFormat::image)
        {
            ///this is ppm/pgm image format
            finfo<<"Image;";
            if (videoOn) finfo<<" Video:"<<videoType<<"(huffyuv);";
        }
        else if (type == DumpFormat::depth)
        {
            finfo << "Depth;";
        }
        else if (type == DumpFormat::depth_compressed)
        {
            finfo << "DepthCompressed;";
        }
        else if (type == DumpFormat::image_jpg)
        {
            finfo << "Image:jpg;";
        }
        else if (type == DumpFormat::image_png)
        {
            finfo << "Image:png;";
        }
        else
        {
            yError() << "I should not reach this line! Unknown data type" << (int)type;
        }
        finfo<<endl;

        finfo<<"Stamp: ";
        if (txTime && rxTime)
            finfo<<"tx+rx;";
        else if (txTime)
            finfo<<"tx;";
        else
            finfo<<"rx;";
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

    void run() override
    {
        //!!! access to size must be protected: problem spotted with Linux stl
        buf.lock();
        unsigned int sz=(unsigned int)buf.size();
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
                auto& img=static_cast<DumpImage*>(itemEnd.obj)->getImage();
                int frameW=img.size().width;
                int frameH=img.size().height;

                t0=itemFront.timeStamp.getStamp();
                double dt=itemEnd.timeStamp.getStamp()-t0;
                fps=(dt<=0.0)?25:int(double(sz-1)/dt);

                videoWriter.open(videoFile.c_str(),cv::VideoWriter::fourcc('H','F','Y','U'),
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
                    videoWriter << static_cast<DumpImage*>(item.obj)->getImage();

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

    void threadRelease() override
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
    DumpThread *thread{nullptr};

public:
    DumpReporter() = default;
    void setThread(DumpThread *thread) { this->thread=thread; }
    void report(const PortInfo &info) override
    {
        if ((thread!=nullptr) && info.incoming)
            thread->writeSource(info.sourceName,info.created);
    }
};


/**************************************************************************/
class DumpModule: public RFModule
{
private:
    DumpQueue        *q{nullptr};
    DumpPort<Bottle> *p_bottle{nullptr};
    DumpPort<Image>  *p_image{nullptr};
    DumpThread       *t{nullptr};
    DumpReporter      reporter;
    Port              rpcPort;
    DumpFormat        dumptype{ DumpFormat::bottle};
    bool              rxTime{false};
    bool              txTime{false};
    unsigned int      dwnsample{0};
    string            portName;

    void polish_filename(string &fname)
    {
        array<char,6> notallowed={':','*','?','|','>','<'};
        for (const auto& c : notallowed)
        {
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            replace(fname.begin(),fname.end(),c,'_');
#else
            auto it = fname.begin();
            for (; it != fname.end(); ++it) {
                if (*it == c) {
                    *it = '_';
                }
            }
#endif
        }
    }

public:
    DumpModule() = default;

    bool configure(ResourceFinder &rf) override
    {
        portName=rf.check("name",Value("/dump")).asString();
        if (portName[0]!='/')
            portName="/"+portName;

        bool saveData=true;
        bool videoOn=false;
        string videoType=rf.check("videoType",Value("mkv")).asString();

        if (rf.check("type"))
        {
            string optTypeName=rf.find("type").asString();
            if (optTypeName=="bottle")
            {
                dumptype = DumpFormat::bottle;
            }
            else if (optTypeName == "depth")
            {
                dumptype = DumpFormat::depth;
            }
            else if (optTypeName == "depth_compressed")
            {
                dumptype = DumpFormat::depth_compressed;
            }
            else if ((optTypeName == "image"))
            {
                dumptype = DumpFormat::image;
                #ifdef ADD_VIDEO
                if (rf.check("addVideo"))   videoOn = true;
                #endif
            }
            else if ((optTypeName == "image_jpg"))
            {
                dumptype = DumpFormat::image_jpg;
                #ifdef ADD_VIDEO
                if (rf.check("addVideo"))   videoOn = true;
                #endif
            }
            else if ((optTypeName == "image_png"))
            {
                dumptype = DumpFormat::image_png;
                #ifdef ADD_VIDEO
                if (rf.check("addVideo"))   videoOn = true;
                #endif
            }
        #ifdef ADD_VIDEO
            else if (optTypeName=="video")
            {
                dumptype = DumpFormat::image;
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
        {
            dumptype = DumpFormat::bottle;
        }

        dwnsample=rf.check("downsample",Value(1)).asInt32();
        rxTime=rf.check("rxTime");
        txTime=rf.check("txTime");
        string templateDirName=rf.check("dir")?rf.find("dir").asString():portName;
        polish_filename(templateDirName);
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
        t=new DumpThread(dumptype,*q,dirName,100,saveData,videoOn,videoType,rxTime,txTime);

        if (!t->start())
        {
            delete t;
            delete q;

            return false;
        }

        reporter.setThread(t);

        if (dumptype == DumpFormat::bottle)
        {
            p_bottle=new DumpPort<Bottle>(*q,dwnsample,rxTime,txTime, DumpFormat::bottle);
            p_bottle->useCallback();
            p_bottle->open(portName);
            p_bottle->setStrict();
            p_bottle->setReporter(reporter);
        }
        else
        {
            p_image=new DumpPort<Image>(*q,dwnsample,rxTime,txTime, dumptype);
            p_image->useCallback();
            p_image->open(portName);
            p_image->setStrict();
            p_image->setReporter(reporter);
        }

        if (rf.check("connect"))
        {
            string srcPort=rf.find("connect").asString();
            bool ok=Network::connect(srcPort.c_str(),
                                    (dumptype == DumpFormat::bottle)?  p_bottle->getName().c_str() : p_image->getName().c_str(),
                                    "tcp");

            ostringstream msg;
            msg << "Connection to " << srcPort << " " << (ok?"successful":"failed");

            if (ok)
                yInfo() << msg.str();
            else
                yWarning() << msg.str();
        }

        // this port serves to handle the "quit" rpc command
        rpcPort.open(portName+"/rpc");
        attach(rpcPort);

        yInfo() << "Service yarp port: " << portName;
        yInfo() << "Data stored in   : " << dirName;

        return true;
    }

    bool close() override
    {
        t->stop();

        if (dumptype == DumpFormat::bottle)
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

    double getPeriod() override { return 1.0;  }
    bool   updateModule() override { return true; }
};


/**************************************************************************/
int main(int argc, char *argv[])
{
    Network yarp;

    ResourceFinder rf;
    rf.configure(argc,argv);

    if (rf.check("help"))
    {
        yInfo() << "Options:";
        yInfo() << "\t--name       port: service port name (default: /dump)";
        yInfo() << "\t--connect    port: name of the port to connect the dumper to at launch time";
        yInfo() << "\t--dir        name: provide explicit name of storage directory";
        yInfo() << "\t--overwrite      : overwrite pre-existing storage directory";
    #ifdef ADD_VIDEO
        yInfo() << "\t--type       type: type of the data to be dumped [bottle(default), image, image_jpg, image_png, video, depth, depth_compressed]";
        yInfo() << "\t--addVideo       : produce video as well (if image* is selected)";
        yInfo() << "\t--videoType   ext: produce video of specified container type [mkv(default), avi]";
    #else
        yInfo() << "\t--type       type: type of the data to be dumped [bottle(default), image, image_jpg, image_png, depth, depth_compressed]";
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
