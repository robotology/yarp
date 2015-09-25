// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef __USBCameraDriverh__
#define __USBCameraDriverh__


#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PreciselyTimed.h>

namespace yarp {
    namespace dev {
        class USBCameraDriver;
        class USBCameraDriverRaw;
        class USBCameraDriverRgb;
    }
}


/**
*
\author Alberto Cardellino
*/

/**
*  Yarp device driver implementation for acquiring images from USB cameras
*/
class yarp::dev::USBCameraDriver :
    public DeviceDriver,
    public IPreciselyTimed,
    public IFrameGrabber,
    public IFrameGrabberRgb,
    public IFrameGrabberControls,
    public IFrameGrabberControls2
{
protected:
    USBCameraDriver(const USBCameraDriver&);
    void operator=(const USBCameraDriver&);
    IFrameGrabberRgb        *deviceRgb;
    IFrameGrabber           *deviceRaw;
    DeviceDriver            *os_device;
    IFrameGrabberControls   *deviceControls;
    IFrameGrabberControls2  *deviceControls2;

    int _width;
    int _height;
    int pixelType;

public:
    /**
    * Constructor.
    */
    USBCameraDriver();

    /**
    * Destructor.
    */
    virtual ~USBCameraDriver();

    /**
    * Open the device driver.
    * @param config configuration for the device driver
    * @return returns true on success, false on failure.
    */
    virtual bool open(yarp::os::Searchable& config);

    /**
    * Closes the device driver.
    * @return returns true/false on success/failure.
    */
    virtual bool close(void);

    /**
     * Implements FrameGrabber basic interface.
     */
    virtual int height() const;

    /**
     * Implements FrameGrabber basic interface.
     */
    virtual int width() const;

    /**
    * Implements FrameGrabber basic interface.
    * @param buffer the pointer to the array to store the last frame.
    * @return returns true/false on success/failure.
    */
    virtual bool getRawBuffer(unsigned char *buffer);

    /**
    * Implements the Frame grabber basic interface.
    * @return the size of the raw buffer (for the Dragonfly
    * camera this is 1x640x480).
    */
    virtual int getRawBufferSize();

    /**
    * FrameGrabber bgr interface, returns the last acquired frame as
    * a buffer of bgr triplets. A demosaicking method is applied to
    * reconstuct the color from the Bayer pattern of the sensor.
    * @param buffer pointer to the array that will contain the last frame.
    * @return true/false upon success/failure
    */
    virtual bool getRgbBuffer(unsigned char *buffer);

    /**
    * Implements the IPreciselyTimed interface.
    * @return the yarp::os::Stamp of the last image acquired
    */
    virtual yarp::os::Stamp getLastInputStamp();

    /**
    * Set Brightness.
    * @param v normalized image brightness [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setBrightness(double v);
    /**
    * Set Exposure.
    * @param v normalized image exposure [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setExposure(double v);
    /**
    * Set Sharpness.
    * @param v normalized image sharpness [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setSharpness(double v);
    /**
    * Set White Balance.
    * @param blue normalized image blue balance [0.0 : 1.0].
    * @param red normalized image red balance [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setWhiteBalance(double blue, double red);
    /**
    * Set Hue.
    * @param v normalized image hue [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setHue(double v);
    /**
    * Set Saturation.
    * @param v normalized image saturation [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setSaturation(double v);
    /**
    * Set Gamma.
    * @param v normalized image gamma [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setGamma(double v);
    /**
    * Set Shutter.
    * @param v normalized camera shutter time [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setShutter(double v);
    /**
    * Set Gain.
    * @param v normalized camera gain [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setGain(double v);
    /**
    * Set Iris.
    * @param v normalized camera iris [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool setIris(double v);
    //virtual bool setTemperature(double v);
    //virtual bool setWhiteShading(double r,double g,double b);
    //virtual bool setOpticalFilter(double v);
    //virtual bool setCaptureQuality(double v);

    /**
    * Get Brightness.
    * @return normalized image brightness [0.0 : 1.0].
    */
    virtual double getBrightness();
    /**
    * Get Exposure.
    * @return normalized image exposure [0.0 : 1.0].
    */
    virtual double getExposure();
    /**
    * Get Sharpness.
    * @return normalized image sharpness [0.0 : 1.0].
    */
    virtual double getSharpness();
    /**
    * Get White Balance.
    * @param blue normalized blue balance [0.0 : 1.0].
    * @param red normalized red balance [0.0 : 1.0].
    * @return true/false upon success/failure
    */
    virtual bool getWhiteBalance(double &blue, double &red);
    /**
    * Get Hue.
    * @return normalized hue [0.0 : 1.0].
    */
    virtual double getHue();
    /**
    * Get Saturation.
    * @return normalized saturation [0.0 : 1.0].
    */
    virtual double getSaturation();
    /**
    * Get Gamma.
    * @return normalized gamma [0.0 : 1.0].
    */
    virtual double getGamma();
    /**
    * Get Shutter.
    * @return normalized shutter time [0.0 : 1.0].
    */
    virtual double getShutter();
    /**
    * Get Gain.
    * @return normalized gain [0.0 : 1.0].
    */
    virtual double getGain();
    /**
    * Get Iris.
    * @return normalized iris [0.0 : 1.0].
    */
    virtual double getIris();


    //virtual double getTemperature() const;
    //virtual double getWhiteShading() const;
    //virtual double getOpticalFilter() const;
    //virtual double getCaptureQuality() const;

    //virtual bool setAutoBrightness(bool bAuto=true);

    //virtual bool setAutoGain(bool bAuto=true);

    //virtual bool setAutoShutter(bool bAuto=true);

    //virtual bool setAutoWhiteBalance(bool bAuto=true);

    //virtual bool setAuto(bool bAuto=true);

    //virtual void PrintSettings();

    /*  Implementation of IFrameGrabberControls2 interface
     *
     * Actual function will be implemented by OS specific devices
     */
    virtual bool getCameraDescription(CameraDescriptor *camera);
    virtual bool hasFeature(int feature, bool *hasFeature);
    virtual bool setFeature(int feature, double  value);
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

    /////////
    // DC1394
    /////////

    /**
     * Is feature present?
     * @param feature feature ID.
     * @return true=present, false=not present.
     */
    virtual bool hasFeatureDC1394(int feature);
    /**
     * Set feature value.
     * @param feature feature ID
     * @param value normalized feature value [0.0 : 1.0].
     * @return true/false upon success/failure.
     */
    virtual bool setFeatureDC1394(int feature, double value);
    /**
     * Get feature value.
     * @param feature feature ID
     * @return normalized feature value [0.0 : 1.0].
     */
    virtual double getFeatureDC1394(int feature);

    /**
     * Can be feature turned on/off?
     * @param feature feature ID.
     * @return true=YES, false=NO.
     */
    virtual bool hasOnOffDC1394(int feature);
    /**
     * Switch feature on/off.
     * @param feature feature ID.
     * @param onoff true=ON false=OFF.
     * @return true/false upon success/failure.
     */
    virtual bool setActiveDC1394(int feature, bool onoff);
    /**
     * Is feature ON or OFF?
     * @param feature feature ID.
     * @return true=ON false=OFF.
     */
    virtual bool getActiveDC1394(int feature);

    /**
     * Has feature Auto mode?
     * @param feature feature ID.
     * @return true=YES, false=NO.
     */
    virtual bool hasAutoDC1394(int feature);
    /**
     * Has feature Manual mode?
     * @param feature feature ID.
     * @return true=YES, false=NO.
     */
    virtual bool hasManualDC1394(int feature);
    /**
     * Has feature Manual mode?
     * @param feature feature ID.
     * @return true=YES, false=NO.
     */
    virtual bool hasOnePushDC1394(int feature);
    /**
     * Switch feature Auto/Manual.
     * @param feature feature ID.
     * @param onoff true=Auto false=Manual.
     * @return true/false upon success/failure.
     */
    virtual bool setModeDC1394(int feature, bool auto_onoff);
    /**
     * Is feature mode Auto or Manual?
     * @param feature feature ID.
     * @return true=Auto false=Manual.
     */
    virtual bool getModeDC1394(int feature);
    /**
     * Trigger feature One Push adjust.
     * @param feature feature ID.
     * @return true/false upon success/failure.
     */
    virtual bool setOnePushDC1394(int feature);

    /**
     * Get supported video mode mask.
     * The video mode bitmask is obtained as follows:
     *
     *   unsigned int mask=0;
     *
     *   for (unsigned int m=0; m<modes.num; ++m)
     *
     *           mask|=1<<(modes.modes[m]-DC1394_VIDEO_MODE_MIN);
     *
     * 0=160x120 YUV444, 1=320x240 YUV422, 2=640x480 YUV411, 3=640x480 YUV422, 4=640x480 RGB8, 5=640x480 MONO8,
     * 6=640x480 MONO16,7=800x600 YUV422, 8=800x600 RGB8, 9=800x600_MONO8, 10=1024x768 YUV422, 11=1024x768 RGB8, 12=1024x768 MONO8,
     * 13=800x600 MONO16, 14=1024x768 MONO16, 15=1280x960 YUV422, 16=1280x960 RGB8, 17=1280x960_MONO8, 18=1600x1200 YUV422, 19=1600x1200 RGB8,
     * 20=1600x1200 MONO8, 21=1280x960 MONO16, 22=1600x1200_MONO16, 23=EXIF, 24=FORMAT7 0, 25=FORMAT7 1, 26=FORMAT7 2, 27=FORMAT7 3,
     * 28=FORMAT7 4, 29=FORMAT7 5, 30=FORMAT7 6, 31=FORMAT7 7
     * @return video mode bitmask.
     */
    virtual unsigned int getVideoModeMaskDC1394();
    /**
     * Get camera acquisition mode.
     * @return video mode ID: 0=160x120 YUV444, 1=320x240 YUV422, 2=640x480 YUV411, 3=640x480 YUV422, 4=640x480 RGB8, 5=640x480 MONO8,
     * 6=640x480 MONO16,7=800x600 YUV422, 8=800x600 RGB8, 9=800x600_MONO8, 10=1024x768 YUV422, 11=1024x768 RGB8, 12=1024x768 MONO8,
     * 13=800x600 MONO16, 14=1024x768 MONO16, 15=1280x960 YUV422, 16=1280x960 RGB8, 17=1280x960_MONO8, 18=1600x1200 YUV422, 19=1600x1200 RGB8,
     * 20=1600x1200 MONO8, 21=1280x960 MONO16, 22=1600x1200_MONO16, 23=EXIF, 24=FORMAT7 0, 25=FORMAT7 1, 26=FORMAT7 2, 27=FORMAT7 3,
     * 28=FORMAT7 4, 29=FORMAT7 5, 30=FORMAT7 6, 31=FORMAT7 7
     */
    virtual unsigned int getVideoModeDC1394();
    /**
     * Set camera acquisition mode.
     * @param video_mode ID: 0=160x120 YUV444, 1=320x240 YUV422, 2=640x480 YUV411, 3=640x480 YUV422, 4=640x480 RGB8, 5=640x480 MONO8,
     * 6=640x480 MONO16,7=800x600 YUV422, 8=800x600 RGB8, 9=800x600_MONO8, 10=1024x768 YUV422, 11=1024x768 RGB8, 12=1024x768 MONO8,
     * 13=800x600 MONO16, 14=1024x768 MONO16, 15=1280x960 YUV422, 16=1280x960 RGB8, 17=1280x960_MONO8, 18=1600x1200 YUV422, 19=1600x1200 RGB8,
     * 20=1600x1200 MONO8, 21=1280x960 MONO16, 22=1600x1200_MONO16, 23=EXIF, 24=FORMAT7 0, 25=FORMAT7 1, 26=FORMAT7 2, 27=FORMAT7 3,
     * 28=FORMAT7 4, 29=FORMAT7 5, 30=FORMAT7 6, 31=FORMAT7 7
     * @return true/false upon success/failure.
     */
    virtual bool setVideoModeDC1394(int video_mode);

    /**
     * Get supported framerates mask.
     * The framerates bitmask is obtained as follows:
     *
     *   unsigned int mask=0;
     *
     *   for (unsigned int f=0; f<fps.num; ++f)
     *
     *       mask|=1<<(fps.framerates[f]-DC1394_FRAMERATE_MIN);
     *
     * 0=1.875 fps, 1=3.75 fps, 2=7.5 fps, 3=15 fps, 4=30 fps, 5=60 fps, 6=120 fps, 7=240 fps.
     * @return framerates bitmask.
     */
    virtual unsigned int getFPSMaskDC1394();

    /**
     * Get camera framerate.
     * @return framerate mode ID: 0=1.875 fps, 1=3.75 fps, 2=7.5 fps, 3=15 fps, 4=30 fps, 5=60 fps, 6=120 fps, 7=240 fps.
     */
    virtual unsigned int getFPSDC1394();
    /**
     * Set camera framerate.
     * @param fps framerate ID: 0=1.875 fps, 1=3.75 fps, 2=7.5 fps, 3=15 fps, 4=30 fps, 5=60 fps, 6=120 fps, 7=240 fps.
     * @return true/false upon success/failure.
     */
    virtual bool setFPSDC1394(int fps);

    /**
     * Get camera Firewire ISO speed.
     * @return ISO speed ID: 0=100 Mbps, 1=200 Mbps, 2=400 Mbps, 3=800 Mbps, 4=1600 Mbps, 5=3200 Mbps.
     */
    virtual unsigned int getISOSpeedDC1394();
    /**
     * Set camera Firewire ISO speed.
     * @param speed ISO speed ID: 0=100 Mbps, 1=200 Mbps, 2=400 Mbps, 3=800 Mbps, 4=1600 Mbps, 5=3200 Mbps.
     * @return true/false upon success/failure.
     */
    virtual bool setISOSpeedDC1394(int speed);

    /**
     * Get supported color coding mask.
     * The framerates bitmask is obtained as follows:
     *
     *   unsigned int mask=0;
     *
     *   for (unsigned int m=0; m<codings.num; ++m)
     *
     *           mask|=1<<(codings.codings[m]-DC1394_COLOR_CODING_MIN);
     *
     * 0=MONO8, 1=YUV411, 2=YUV422, 3=YUV444, 4=RGB8, 5=MONO16, 6=RGB16, 7=MONO16S, 8=RGB16S, 9=RAW8, 10=RAW16.
     * @return framerates bitmask.
     */
    virtual unsigned int getColorCodingMaskDC1394(unsigned int video_mode);
    /**
     * Get image color coding.
     * @return image color coding ID: 0=MONO8, 1=YUV411, 2=YUV422, 3=YUV444, 4=RGB8, 5=MONO16, 6=RGB16, 7=MONO16S, 8=RGB16S, 9=RAW8, 10=RAW16.
     */
    virtual unsigned int getColorCodingDC1394();
    /**
     * Set image color coding.
     * @param coding image color coding ID: 0=MONO8, 1=YUV411, 2=YUV422, 3=YUV444, 4=RGB8, 5=MONO16, 6=RGB16, 7=MONO16S, 8=RGB16S, 9=RAW8, 10=RAW16.
     * @return true/false upon success/failure.
     */
    virtual bool setColorCodingDC1394(int coding);

    /**
     * Set White Balance.
     * @param blue normalized image blue balance [0.0 : 1.0].
     * @param red normalized image red balance [0.0 : 1.0].
     * @return true/false upon success/failure
     */
    virtual bool setWhiteBalanceDC1394(double b, double r);
    /**
     * Get White Balance.
     * @param blue normalized blue balance [0.0 : 1.0].
     * @param red normalized red balance [0.0 : 1.0].
     * @return true/false upon success/failure
     */
    virtual bool getWhiteBalanceDC1394(double &b, double &r);

    /**
     * Get maximum image dimensions in Format 7 mode.
     * @param xdim maximum width
     * @param ydim maximum height
     * @param xstep width granularity
     * @param ystep height granularity
     * @param xoffstep horizontal offset granularity
     * @param yoffstep vertical offset granularity
     * @return true/false upon success/failure
     */
    virtual bool getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep);

    /**
     * Get image dimensions in Format 7 mode.
     * @param xdim image width
     * @param ydim image height
     * @param x0 horizontal image offset
     * @param y0 vertical image offset
     * @return true/false upon success/failure
     */
    virtual bool getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0);
    /**
     * Set image dimensions in Format 7 mode.
     * @param xdim image width
     * @param ydim image height
     * @param x0 horizontal image offset
     * @param y0 vertical image offset
     * @return true/false upon success/failure
     */
    virtual bool setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0);

    /**
     * Set Operation Mode.
     * @param b1394b true=1394b false=LEGACY
     * @return true/false upon success/failure
     */
    virtual bool setOperationModeDC1394(bool b1394b);
    /**
     * Get Operation Mode.
     * @return true=1394b false=LEGACY
     */
    virtual bool getOperationModeDC1394();

    /**
     * Set image transmission ON/OFF.
     * @param bTxON true=ON false=OFF
     * @return true/false upon success/failure
     */
    virtual bool setTransmissionDC1394(bool bTxON);
    /**
     * Is image transmission ON or OFF?
     * @return true=ON false=OFF
     */
    virtual bool getTransmissionDC1394();
    //virtual bool setBayerDC1394(bool bON);
    //virtual bool getBayerDC1394();

    /**
     * Set feature commands propagation ON/OFF.
     * All the cameras on the same Firewire bus can be adjusted at once setting broadcast ON. In this way, they will share the feature settings.
     * @param onoff true=ON false=OFF
     * @return true/false upon success/failure
     */
    virtual bool setBroadcastDC1394(bool onoff);
    /**
     * Set camera features to default.
     * @return true/false upon success/failure
     */
    virtual bool setDefaultsDC1394();
    /*
     * Reset camera.
     * @return true/false upon success/failure
     */
    virtual bool setResetDC1394();
    /**
     * Switch camera power ON/OFF.
     * @param onoff true=ON false=OFF
     * @return true/false upon success/failure
     */
    virtual bool setPowerDC1394(bool onoff);
    /**
     * Switch image capture ON/OFF.
     * @param onoff true=ON false=OFF
     * @return true/false upon success/failure
     */
    virtual bool setCaptureDC1394(bool bON);

    /**
     * Get Firewire communication packet size.
     * In Format7 mode the framerate depends from packet size.
     * @return bytes per packet
     */
    virtual unsigned int getBytesPerPacketDC1394();
    /**
     * Set Firewire communication packet size.
     * In Format7 mode the framerate depends from packet size.
     * @param bpp bytes per packet
     * @return true/false upon success/failure
     */
    virtual bool setBytesPerPacketDC1394(unsigned int bpp);
};


class yarp::dev::USBCameraDriverRgb :   public yarp::dev::USBCameraDriver,
                                        public IFrameGrabberImage
{
private:
    USBCameraDriverRgb(const USBCameraDriverRgb&);
    void operator=(const USBCameraDriverRgb&);

public:

    USBCameraDriverRgb();
    ~USBCameraDriverRgb();

    /**
        * FrameGrabber image interface, returns the last acquired frame as
        * an rgb image. A demosaicking method is applied to
        * reconstuct the color from the Bayer pattern of the sensor.
        * @param image that will store the last frame.
        * @return true/false upon success/failure
        */
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    /**
        * Return the height of each frame.
        * @return image height
        */
    int height() const;

    /**
        * Return the width of each frame.
        * @return image width
        */
    int width() const;
};

class yarp::dev::USBCameraDriverRaw :   public yarp::dev::USBCameraDriver,
                                        public IFrameGrabberImageRaw
{
private:
    USBCameraDriverRaw(const USBCameraDriverRaw&);
    void operator=(const USBCameraDriverRaw&);

public:

    USBCameraDriverRaw();
    ~USBCameraDriverRaw();

    /**
        * FrameGrabber image interface, returns the last acquired frame as
        * an rgb image. A demosaicking method is applied to
        * reconstuct the color from the Bayer pattern of the sensor.
        * @param image that will store the last frame.
        * @return true/false upon success/failure
        */
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);

    /**
        * Return the height of each frame.
        * @return image height
        */
    int height() const;

    /**
        * Return the width of each frame.
        * @return image width
        */
    int width() const;
};

#endif // __USBCameraDriverh__
