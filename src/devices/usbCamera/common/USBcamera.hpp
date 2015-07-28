// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
    public IFrameGrabberRgb
//     public IFrameGrabberControlsDC1394
{
protected:
    USBCameraDriver(const USBCameraDriver&);
    void operator=(const USBCameraDriver&);
    IFrameGrabberRgb *deviceRgb;
    IFrameGrabber    *deviceRaw;
    DeviceDriver     *os_device;

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

//     /**
//     * Set Brightness.
//     * @param v normalized image brightness [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setBrightness(double v);
//     /**
//     * Set Exposure.
//     * @param v normalized image exposure [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setExposure(double v);
//     /**
//     * Set Sharpness.
//     * @param v normalized image sharpness [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setSharpness(double v);
//     /**
//     * Set White Balance.
//     * @param blue normalized image blue balance [0.0 : 1.0].
//     * @param red normalized image red balance [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setWhiteBalance(double blue, double red);
//     /**
//     * Set Hue.
//     * @param v normalized image hue [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setHue(double v);
//     /**
//     * Set Saturation.
//     * @param v normalized image saturation [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setSaturation(double v);
//     /**
//     * Set Gamma.
//     * @param v normalized image gamma [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setGamma(double v);
//     /**
//     * Set Shutter.
//     * @param v normalized camera shutter time [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setShutter(double v);
//     /**
//     * Set Gain.
//     * @param v normalized camera gain [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setGain(double v);
//     /**
//     * Set Iris.
//     * @param v normalized camera iris [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool setIris(double v);
//     //virtual bool setTemperature(double v);
//     //virtual bool setWhiteShading(double r,double g,double b);
//     //virtual bool setOpticalFilter(double v);
//     //virtual bool setCaptureQuality(double v);
//
//     /**
//     * Get Brightness.
//     * @return normalized image brightness [0.0 : 1.0].
//     */
//     virtual double getBrightness();
//     /**
//     * Get Exposure.
//     * @return normalized image exposure [0.0 : 1.0].
//     */
//     virtual double getExposure();
//     /**
//     * Get Sharpness.
//     * @return normalized image sharpness [0.0 : 1.0].
//     */
//     virtual double getSharpness();
//     /**
//     * Get White Balance.
//     * @param blue normalized blue balance [0.0 : 1.0].
//     * @param red normalized red balance [0.0 : 1.0].
//     * @return true/false upon success/failure
//     */
//     virtual bool getWhiteBalance(double &blue, double &red);
//     /**
//     * Get Hue.
//     * @return normalized hue [0.0 : 1.0].
//     */
//     virtual double getHue();
//     /**
//     * Get Saturation.
//     * @return normalized saturation [0.0 : 1.0].
//     */
//     virtual double getSaturation();
//     /**
//     * Get Gamma.
//     * @return normalized gamma [0.0 : 1.0].
//     */
//     virtual double getGamma();
//     /**
//     * Get Shutter.
//     * @return normalized shutter time [0.0 : 1.0].
//     */
//     virtual double getShutter();
//     /**
//     * Get Gain.
//     * @return normalized gain [0.0 : 1.0].
//     */
//     virtual double getGain();
//     /**
//     * Get Iris.
//     * @return normalized iris [0.0 : 1.0].
//     */
//     virtual double getIris();


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
