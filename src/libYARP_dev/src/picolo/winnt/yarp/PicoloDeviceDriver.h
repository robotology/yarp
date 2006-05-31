// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2PicoloDeviceDriverh__
#define __YARP2PicoloDeviceDriverh__

#include <yarp/dev/FrameGrabberInterfaces.h>

#include <stdlib.h>
#include <string.h>

/**
 * \file PicoloDeviceDriver.h device driver for managing the Picolo frame grabber.
 * This is a bt848 based frame grabber.
 */

namespace yarp {
    namespace dev{
        class PicoloDeviceDriver;
        class PicoloOpenParameters;
    }
}

/**
 * Class for defining the open() parameters of the frame grabber.
 */
class yarp::dev::PicoloOpenParameters
{
public:
	/**
	 * Constructor. Add here the parameters for the open().
	 */
    PicoloOpenParameters()
	{
		_unit_number = 0;
		_video_type = 0;
		_size_x = 256;
		_size_y = 256;
		_offset_y = 0;
		_offset_x = 0;
		_alfa = 1.055f;
		// _alfa = 1.125f;
	}

	int _unit_number;		/** board number 0, 1, 2, etc. */
	int _video_type;		/** 0 composite, 1 svideo. */
	int _size_x;			/** requested size x. */
	int _size_y;			/** requested size y. */
	int _offset_y;			/** y offset, with respect to the center. */
	int _offset_x;			/** x offset, with respect to the center. */
	float _alfa;			/** to have the possibility of shifting the ROI vertically, the requested size is actually _alfa*_size_y */
};

/**
 * The Picolo frame grabber device.
 * The device driver handles triple buffering by having a thread waiting on new frame events and
 * manually generating the correct pointer for the user to read. The class is not itself 
 * protected by a mutex since there's an internal mutex already. This is hidden in system_resources.
 */
class yarp::dev::PicoloDeviceDriver : public IFrameGrabber, public IFrameGrabberRgb
{
private:
	PicoloDeviceDriver(const PicoloDeviceDriver&);
	void operator=(const PicoloDeviceDriver&);
public:
	/**
	 * Constructor.
	 */
	PicoloDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~PicoloDeviceDriver();

	/**
	 * Opens the device driver. For the meaning of parameters see PicoloOpenParameters.
	 * @param p is a pointer to the open arguments.
	 * @return returns true on success.
	 */
	bool open(const PicoloOpenParameters &par);

	/**
	 * Closes the device driver.
	 */
    virtual bool close();

	/**
	 * Implements the FrameGrabber interface.
	 */
	virtual bool getRgbBuffer(unsigned char *buff);

    /**
	 * Implements the FrameGrabber interface, for the
     * Picolo device the raw buffer is a triplet of bgr
     * values.
	 */
	virtual bool getRawBuffer(unsigned char *buff);

    /**
	 * Implements the FrameGrabber interface, for the 
     * Picolo device, this is 3*height()*width().
	 */
	virtual int getRawBufferSize();

	/**
	 * Implements the FrameGrabber interface.
	 */
	virtual int height() const;

	/**
	 * Implements the FrameGrabber interface.
	 */
	virtual int width() const;

protected:
	/**
	 * Locks the current image buffer.
	 * @param buffer is a pointer to the buffer address (i.e. a double pointer).
	 * @return true if successful.
	 */
	bool acquireBuffer(void *buffer);

	/**
	 * Releases the current image buffer.
	 * @return true always
	 */
	bool releaseBuffer();

	/**
	 * Waits on a new frame. An event is signaled when a new frame is acquired,
	 * the calling thread waits efficiently on this event.
	 * @return true always.
	 */
	bool waitOnNewFrame ();

#if 0

	/**
	 * Sets the average image brightness. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setBrightness (void *cmd);

	/**
	 * Sets the acquisition hue. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setHue (void *cmd);

	/**
	 * Sets the acquisition contrast. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setContrast (void *cmd);

	/**
	 * Sets the gain of the amplifier on the U chroma channel. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setSatU (void *cmd);

	/**
	 * Sets the gain of the amplifier on the V chroma channel. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setSatV (void *cmd);

	/**
	 * Enables the notch filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setLNotch (void *cmd);

	/**
	 * Enables the decimation filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setLDec (void *cmd);

	/**
	 * Enables the crush filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setCrush (void *cmd);
#endif

protected:
	void *system_resources;
};


#endif
