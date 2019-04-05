/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#ifndef __USBCameraDriverh__
#define __USBCameraDriverh__


#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IVisualParams.h>

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
    public IRgbVisualParams
{
protected:
    USBCameraDriver(const USBCameraDriver&);
    void operator=(const USBCameraDriver&);
    IFrameGrabberRgb        *deviceRgb;
    IPreciselyTimed         *deviceTimed;
    IFrameGrabber           *deviceRaw;
    DeviceDriver            *os_device;
    IFrameGrabberControls   *deviceControls;
    IFrameGrabberControls2  *deviceControls2;
    IRgbVisualParams        *deviceRgbVisualParam;

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
    /**
     * Return the height of each frame.
     * @return rgb image height
     */
    virtual int getRgbHeight();

    /**
     * Return the width of each frame.
     * @return rgb image width
     */
    virtual int getRgbWidth();

    /**
     * Get the possible configurations of the camera
     * @param configurations  list of camera supported configurations as CameraConfig type
     * @return true on success
     */

    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations);
    /**
     * Get the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */

    virtual bool getRgbResolution(int &width, int &height);

    /**
     * Set the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */

    virtual bool setRgbResolution(int width, int height);

    /**
     * Get the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true on success
     */
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov);

    /**
     * Set the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    virtual bool setRgbFOV(double horizontalFov, double verticalFov);

    /**
     * Get the intrinsic parameters of the rgb camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     *
     * Look at IVisualParams.h for more details
     */
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic);

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual bool getRgbMirroring(bool &mirror);

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual bool setRgbMirroring(bool mirror);
};


class yarp::dev::USBCameraDriverRgb :   public yarp::dev::USBCameraDriver,
                                        public IFrameGrabberImage,
                                        public IFrameGrabberImageRaw
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
        * FrameGrabber image interface, returns the last acquired frame as
        * a raw image.
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
