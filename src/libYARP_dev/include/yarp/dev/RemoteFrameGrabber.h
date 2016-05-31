/*
 * Copyright (C) 2006 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Paul Fitzpatrick, Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP2_REMOTEFRAMEGRABBER
#define YARP2_REMOTEFRAMEGRABBER

#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/LogStream.h>


namespace yarp{
    namespace dev {
        class RemoteFrameGrabber;
        class RemoteFrameGrabberDC1394;
        class ImplementDC1394;
    }
}


class YARP_dev_API yarp::dev::ImplementDC1394 : public IFrameGrabberControlsDC1394
{
private:
    yarp::os::Port *_port;

public:
    ImplementDC1394()  { _port = NULL;};
    ~ImplementDC1394() { _port = NULL;};

    void init(yarp::os::Port *__port) { _port = __port;}

private:
    bool setCommand(int code, double v) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        _port->write(cmd,response);
        return true;
    }

    bool setCommand(int code, double b, double r) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(b);
        cmd.addDouble(r);
        _port->write(cmd,response);
        return true;
    }

    double getCommand(int code) const {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        _port->write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asDouble();
    }

    bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        _port->write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asDouble();
        r=response.get(3).asDouble();
        return true;
    }

public:
    // 00
    virtual bool hasFeatureDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASFEA);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 01
    virtual bool setFeatureDC1394(int feature,double value)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETVAL);
        cmd.addInt(feature);
        cmd.addDouble(value);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 02
    virtual double getFeatureDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETVAL);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asDouble();
    }

    // 03
    virtual bool hasOnOffDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASACT);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 04
    virtual bool setActiveDC1394(int feature, bool onoff)
    {
        yarp::os::Bottle cmd ,response;
        cmd.addVocab(VOCAB_DRSETACT);
        cmd.addInt(feature);
        cmd.addInt(int(onoff));
        _port->write(cmd, response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 05
    virtual bool getActiveDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETACT);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 06
    virtual bool hasManualDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASMAN);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 07
    virtual bool hasAutoDC1394(int feature)
    {
        yTrace();
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASAUT);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 08
    virtual bool hasOnePushDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASONP);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 09
    virtual bool setModeDC1394(int feature, bool auto_onoff)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETMOD);
        cmd.addInt(feature);
        cmd.addInt(int(auto_onoff));
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 10
    virtual bool getModeDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETMOD);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 11
    virtual bool setOnePushDC1394(int feature)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETONP);
        cmd.addInt(feature);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 12
    virtual unsigned int getVideoModeMaskDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETMSK);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 13
    virtual unsigned int getVideoModeDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETVMD);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 14
    virtual bool setVideoModeDC1394(int video_mode)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETVMD);
        cmd.addInt(video_mode);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 15
    virtual unsigned int getFPSMaskDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETFPM);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 16
    virtual unsigned int getFPSDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETFPS);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 17
    virtual bool setFPSDC1394(int fps)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETFPS);
        cmd.addInt(fps);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 18
    virtual unsigned int getISOSpeedDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETISO);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 19
    virtual bool setISOSpeedDC1394(int speed)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETISO);
        cmd.addInt(speed);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 20
    virtual unsigned int getColorCodingMaskDC1394(unsigned int video_mode)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETCCM);
        cmd.addInt(video_mode);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 21
    virtual unsigned int getColorCodingDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETCOD);
        _port->write(cmd,response);

        // I'll bite your sweet little fingers ^__^
        return (unsigned)response.get(0).asInt();
        //return response.get(0).asInt()!=0? true:false;
    }
    // 22
    virtual bool setColorCodingDC1394(int coding)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETCOD);
        cmd.addInt(coding);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 23
    virtual bool setWhiteBalanceDC1394(double b, double r) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETWHB);
        cmd.addDouble(b);
        cmd.addDouble(r);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 24
    virtual bool getWhiteBalanceDC1394(double &b, double &r)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETWHB);
        _port->write(cmd,response);
        b=response.get(0).asDouble();
        r=response.get(1).asDouble();
        return response.get(0).asInt()!=0? true:false;
    }

    virtual bool getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETF7M);
        _port->write(cmd,response);

        xdim=response.get(0).asInt();
        ydim=response.get(1).asInt();
        xstep=response.get(2).asInt();
        ystep=response.get(3).asInt();
        xoffstep=response.get(4).asInt();
        yoffstep=response.get(5).asInt();
        return response.get(0).asInt()!=0? true:false;
    }
    // 26
    virtual bool getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETWF7);
        _port->write(cmd,response);
        xdim=response.get(0).asInt();
        ydim=response.get(1).asInt();
        x0=response.get(2).asInt();
        y0=response.get(3).asInt();
        return response.get(0).asInt()!=0? true:false;
    }
    // 27
    virtual bool setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETWF7);
        cmd.addInt(xdim);
        cmd.addInt(ydim);
        cmd.addInt(x0);
        cmd.addInt(y0);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 28
    virtual bool setOperationModeDC1394(bool b1394b)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETOPM);
        cmd.addInt(int(b1394b));
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 29
    virtual bool getOperationModeDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETOPM);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 30
    virtual bool setTransmissionDC1394(bool bTxON)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETTXM);
        cmd.addInt(int(bTxON));
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 31
    virtual bool getTransmissionDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETTXM);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    /*
     *    // 32
     *    virtual bool setBayerDC1394(bool bON)
     *    {
     *        yarp::os::Bottle cmd, response;
     *        cmd.addVocab(VOCAB_DRSETBAY);
     *        cmd.addInt(int(bON));
     *        _port->write(cmd,response);
     *        return response.get(0).asInt()!=0? true:false;
}
// 33
virtual bool getBayerDC1394()
{
yarp::os::Bottle cmd, response;
cmd.addVocab(VOCAB_DRGETBAY);
_port->write(cmd,response);
return response.get(0).asInt()!=0? true:false;
}
*/
    // 34
    virtual bool setBroadcastDC1394(bool onoff)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETBCS);
        cmd.addInt((int)onoff);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 35
    virtual bool setDefaultsDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETDEF);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 36
    virtual bool setResetDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETRST);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }
    // 37
    virtual bool setPowerDC1394(bool onoff)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETPWR);
        cmd.addInt((int)onoff);
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 38
    virtual bool setCaptureDC1394(bool bON)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETCAP);
        cmd.addInt(int(bON));
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 39
    virtual bool setBytesPerPacketDC1394(unsigned int bpp)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRSETBPP);
        cmd.addInt(int(bpp));
        _port->write(cmd,response);
        return response.get(0).asInt()!=0? true:false;
    }

    // 40
    virtual unsigned int getBytesPerPacketDC1394()
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETBPP);
        _port->write(cmd,response);
        return (unsigned)response.get(0).asInt();
    }


    /////////////////////////////////////////////////


    bool setBrightness(double v) {
        return setCommand(VOCAB_BRIGHTNESS,v);
    }
    double getBrightness() {
        return getCommand(VOCAB_BRIGHTNESS);
    }
    bool setExposure(double v) {
        return setCommand(VOCAB_EXPOSURE,v);
    }
    double getExposure() {
        return getCommand(VOCAB_EXPOSURE);
    }

    bool setSharpness(double v) {
        return setCommand(VOCAB_SHARPNESS,v);
    }
    double getSharpness() {
        return getCommand(VOCAB_SHARPNESS);
    }

    bool setWhiteBalance(double blue, double red)
    {
        return setCommand(VOCAB_WHITE, blue, red);
    }
    bool getWhiteBalance(double &blue, double &red)
    {
        return getCommand(VOCAB_WHITE, blue, red);
    }

    bool setHue(double v) {
        return setCommand(VOCAB_HUE,v);
    }
    double getHue() {
        return getCommand(VOCAB_HUE);
    }

    bool setSaturation(double v) {
        return setCommand(VOCAB_SATURATION,v);
    }
    double getSaturation() {
        return getCommand(VOCAB_SATURATION);
    }

    bool setGamma(double v) {
        return setCommand(VOCAB_GAMMA,v);
    }
    double getGamma() {
        return getCommand(VOCAB_GAMMA);
    }

    bool setShutter(double v) {
        return setCommand(VOCAB_SHUTTER,v);
    }
    double getShutter() {
        return getCommand(VOCAB_SHUTTER);
    }

    bool setGain(double v) {
        return setCommand(VOCAB_GAIN,v);
    }
    double getGain() {
        return getCommand(VOCAB_GAIN);
    }

    bool setIris(double v) {
        return setCommand(VOCAB_IRIS,v);
    }
    double getIris() {
        return getCommand(VOCAB_IRIS);
    }
};


/**
 * @ingroup dev_impl_wrapper
 *
 * \section remoteFrameGrabber
 * Connect to a ServerFrameGrabber.  See ServerFrameGrabber for
 * the network protocol used.
 *
 */
class YARP_dev_API yarp::dev::RemoteFrameGrabber :  public IFrameGrabberImage,
                                                    public IFrameGrabberControls2,
                                                    public ImplementDC1394,
                                                    public DeviceDriver
{
public:
    /**
     * Constructor.
     */
    RemoteFrameGrabber() : mutex(1) {
        lastHeight = 0;
        lastWidth = 0;
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        mutex.wait();
        if (reader.read(true)!=NULL) {
            image = *(reader.lastRead());
            lastHeight = image.height();
            lastWidth = image.width();
            mutex.post();
            return true;
        }
        mutex.post();
        return false;
    }

    // this is bad!
    virtual int height() const {
        return lastHeight;
    }

    virtual int width() const {
        return lastWidth;
    }

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> local </TD><TD> Port name of this client. </TD></TR>
     * <TR><TD> remote </TD><TD> Port name of server to connect to. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config){
        yTrace();
        std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        yDebug() << "config is " << config.toString();

        remote = config.check("remote",yarp::os::Value(""),
                              "port name of real grabber").asString();
        local = config.check("local",yarp::os::Value("..."),
                             "port name to use locally").asString();
        yarp::os::ConstString carrier = 
            config.check("stream",yarp::os::Value("tcp"),
                         "carrier to use for streaming").asString();
        port.open(local);
        if (remote!="") {
            std::cout << "\nconnecting "  << local << " to " << remote << std::endl;

            if(!yarp::os::Network::connect(remote,local,carrier))
                yError() << "cannot connect "  << local << " to " << remote;

            // reverse connection for RPC
            // could choose to do this only on need

            yarp::os::Network::connect(local,remote);
        }
        reader.attach(port);
        ImplementDC1394::init(&port);
        return true;
    }

    virtual bool close() {
        port.close();
//        mutex.wait();   // why does it need this?
        return true;
    }

    virtual bool setBrightness(double v) {
        return setCommand(VOCAB_BRIGHTNESS, v);
    }
    virtual double getBrightness() {
        return getCommand(VOCAB_BRIGHTNESS);
    }
    virtual bool setExposure(double v) {
        return setCommand(VOCAB_EXPOSURE, v);
    }
    virtual double getExposure() {
        return getCommand(VOCAB_EXPOSURE);
    }

    virtual bool setSharpness(double v) {
        return setCommand(VOCAB_SHARPNESS, v);
    }
    virtual double getSharpness() {
        return getCommand(VOCAB_SHARPNESS);
    }

    virtual bool setWhiteBalance(double blue, double red)
    {
        return setCommand(VOCAB_WHITE, blue, red);
    }
    virtual bool getWhiteBalance(double &blue, double &red)
    {
        return getCommand(VOCAB_WHITE, blue, red);
    }

    virtual bool setHue(double v) {
        return setCommand(VOCAB_HUE,v);
    }
    virtual double getHue() {
        return getCommand(VOCAB_HUE);
    }

    virtual bool setSaturation(double v) {
        return setCommand(VOCAB_SATURATION,v);
    }
    virtual double getSaturation() {
        return getCommand(VOCAB_SATURATION);
    }

    virtual bool setGamma(double v) {
        return setCommand(VOCAB_GAMMA,v);
    }
    virtual double getGamma() {
        return getCommand(VOCAB_GAMMA);
    }

    virtual bool setShutter(double v) {
        return setCommand(VOCAB_SHUTTER,v);
    }
    virtual double getShutter() {
        return getCommand(VOCAB_SHUTTER);
    }

    virtual bool setGain(double v) {
        return setCommand(VOCAB_GAIN,v);
    }
    virtual double getGain() {
        return getCommand(VOCAB_GAIN);
    }

    virtual bool setIris(double v) {
        return setCommand(VOCAB_IRIS,v);
    }
    virtual double getIris() {
        return getCommand(VOCAB_IRIS);
    }

    /* Implementation of IFrameGrabberControls2 interface */
    virtual bool getCameraDescription(CameraDescriptor *camera);
    virtual bool hasFeature(int feature, bool *hasFeature);
    virtual bool setFeature(int feature, double value);
    virtual bool getFeature(int feature, double *value);
    virtual bool setFeature(int feature, double  value1, double  value2);
    virtual bool getFeature(int feature, double *value1, double *value2);
    virtual bool hasOnOff(int feature, bool *HasOnOff);
    virtual bool setActive(int feature, bool onoff);
    virtual bool getActive(int feature, bool *isActive);
    virtual bool hasAuto(int feature, bool *hasAuto);
    virtual bool hasManual(int feature, bool *hasManual);
    virtual bool hasOnePush(int feature, bool *hasOnePush);
    virtual bool setMode(int feature, FeatureMode mode);
    virtual bool getMode(int feature, FeatureMode *mode);
    virtual bool setOnePush(int feature);

protected:
    yarp::os::PortReaderBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> > reader;
    yarp::os::Port port;
    yarp::os::ConstString remote;
    yarp::os::ConstString local;
    yarp::os::Semaphore mutex;
    int lastHeight;
    int lastWidth;

    IFrameGrabberControlsDC1394 *Ifirewire;
    
    bool setCommand(int code, double v) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        port.write(cmd,response);
        return true;
    }

    bool setCommand(int code, double b, double r) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(b);
        cmd.addDouble(r);
        port.write(cmd,response);
        return true;
    }

    double getCommand(int code) const {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asDouble();
    }

    bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asDouble();
        r=response.get(3).asDouble();
        return true;
    }
};

class yarp::dev::RemoteFrameGrabberDC1394 : public yarp::dev::RemoteFrameGrabber
{

};

#endif


