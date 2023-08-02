/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBERCONTROLS_H
#define YARP_DEV_IFRAMEGRABBERCONTROLS_H

#include <yarp/dev/api.h>

#include <string>


typedef enum {
    BUS_UNKNOWN = 0,
    BUS_FIREWIRE,
    BUS_USB
} BusType;

typedef enum {
    MODE_UNKNOWN = 0,
    MODE_MANUAL,
    MODE_AUTO
} FeatureMode;

typedef struct {
    BusType busType;
    std::string deviceDescription;
} CameraDescriptor;

typedef enum {
    YARP_FEATURE_INVALID=-1,
    YARP_FEATURE_BRIGHTNESS=0,
    YARP_FEATURE_EXPOSURE,
    YARP_FEATURE_SHARPNESS,
    YARP_FEATURE_WHITE_BALANCE,
    YARP_FEATURE_HUE,
    YARP_FEATURE_SATURATION,
    YARP_FEATURE_GAMMA,
    YARP_FEATURE_SHUTTER,
    YARP_FEATURE_GAIN,
    YARP_FEATURE_IRIS,
    YARP_FEATURE_FOCUS,
    YARP_FEATURE_TEMPERATURE,
    YARP_FEATURE_TRIGGER,
    YARP_FEATURE_TRIGGER_DELAY,
    YARP_FEATURE_WHITE_SHADING,
    YARP_FEATURE_FRAME_RATE,
    YARP_FEATURE_ZOOM,
    YARP_FEATURE_PAN,
    YARP_FEATURE_TILT,
    YARP_FEATURE_OPTICAL_FILTER,
    YARP_FEATURE_CAPTURE_SIZE,
    YARP_FEATURE_CAPTURE_QUALITY,
    YARP_FEATURE_MIRROR,
    YARP_FEATURE_NUMBER_OF          // this has to be the last one
} cameraFeature_id_t;

namespace yarp::dev {

/**
 * @ingroup dev_iface_media
 *
 * Control interface for frame grabber devices.
 */
class YARP_dev_API IFrameGrabberControls
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberControls();

    std::string busType2String(BusType type)
    {
        switch (type) {
            case BUS_FIREWIRE:
                return "FireWire";
            break;

            case BUS_USB:
                return "USB";
            break;

            default:
                return "bus type undefined";
            break;
        }
    }

    inline FeatureMode toFeatureMode(bool _auto)
    {
        return _auto ? MODE_AUTO : MODE_MANUAL;
    }
    /**
     * Get a basic description of the camera hw. This is mainly used to determine the
     * HW bus type in order to choose the corresponding interface for advanced controls.
     * @param device returns an identifier for the bus
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getCameraDescription(CameraDescriptor *camera)=0;

    /**
     * Check if camera has the requested feature (saturation, brightness ... )
     * @param feature the identifier of the feature to check
     * @param hasFeature flag value: true if the feature is present, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasFeature(int feature, bool *hasFeature)=0;

    /**
     * Set the requested feature to a value (saturation, brightness ... )
     * @param feature the identifier of the feature to change
     * @param value new value of the feature, range from 0 to 1 expressed as a percentage
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool setFeature(int feature, double value)=0;

    /**
     * Get the current value for the requested feature.
     * @param feature the identifier of the feature to read
     * @param value  pointer to current value of the feature, from 0 to 1 expressed as a percentage
     * @return returns true on success, false on failure.
     */
    virtual bool getFeature(int feature, double *value)=0;

    /**
     * Set the requested feature to a value using 2 params (like white balance)
     * @param feature the identifier of the feature to change
     * @param value1  first param,  from 0 to 1 expressed as a percentage
     * @param value2  second param, from 0 to 1 expressed as a percentage
     *
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool setFeature(int feature, double value1, double value2)=0;

    /**
     * Get the current value for the requested feature.
     * @param feature the identifier of the feaature to read
     * @param value1  returns the current value of the feature, from 0 to 1 expressed as a percentage
     * @param value2  returns the current value of the feature, from 0 to 1 expressed as a percentage
     * @return returns true on success, false on failure.
     */
    virtual bool getFeature(int feature, double *value1, double *value2)=0;

    /**
     * Check if the camera has the ability to turn on/off the requested feature
     * @param feature the identifier of the feature to change
     * @param hasOnOff flag true if this feature can be turned on/off, false otherwise.
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasOnOff(int feature, bool *HasOnOff)=0;

    /**
     * Set the requested feature on or off
     * @param feature the identifier of the feature to change
     * @param onoff true to activate, off to deactivate the feature
     * @return returns true on success, false on failure.
     */
    virtual bool setActive(int feature, bool onoff)=0;

    /**
     * Get the current status of the feature, on or off
     * @param feature the identifier of the feature to check
     * @param isActive flag true if the feature is active, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getActive(int feature, bool *isActive)=0;

    /**
     * Check if the requested feature has the 'auto' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'auto' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasAuto(int feature, bool *hasAuto)=0;

    /**
     * Check if the requested feature has the 'manual' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'manual' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasManual(int feature, bool *hasManual)=0;

    /**
     * Check if the requested feature has the 'onePush' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'onePush' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasOnePush(int feature, bool *hasOnePush)=0;

    /**
     * Set the requested mode for the feature
     * @param feature the identifier of the feature to change
     * @param auto_onoff true to activate 'auto' mode, false to activate 'manual' mode
     * @return returns true on success, false on failure.
     */
    virtual bool setMode(int feature, FeatureMode mode)=0;

    /**
     * Get the current mode for the feature
     * @param feature the identifier of the feature to change
     * @param hasAuto flag true if the feature is has 'auto' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getMode(int feature, FeatureMode *mode)=0;

    /**
     * Set the requested feature to a value (saturation, brightness ... )
     * @param feature the identifier of the feature to change
     * @param value new value of the feature, from 0 to 1 as a percentage of param range
     * @return returns true on success, false on failure.
     */
    virtual bool setOnePush(int feature)=0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IFRAMEGRABBERCONTROLS_H
