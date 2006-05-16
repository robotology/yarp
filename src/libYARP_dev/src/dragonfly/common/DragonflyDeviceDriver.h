/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: DragonflyDeviceDriver.h,v 1.1 2006-05-16 09:57:57 natta Exp $
///
///

#ifndef __DragonflyDeviceDriverh__
#define __DragonflyDeviceDriverh__

//=============================================================================
// YARP Includes
//=============================================================================

// May 06, readapted for YARP2 by nat

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/FrameGrabber.h>

namespace yarp {
    namespace dev {
    class DragonflyOpenParameters;
    class DragonflyDeviceDriver;
    }
}

/**
 * \file DragonflyDeviceDriver.h device driver for managing the 
 * Dragonfly IEEE-1394 Camera (WINNT version)
 */

/**
 * Structure for defining the open() parameters of the camera.
 */
class yarp::dev::DragonflyOpenParameters
{
public:
	/**
	 * Constructor. Add here the parameters for the open().
	 */
	DragonflyOpenParameters()
	{
		// parameters initialization
		_unit_number = 0;
		_size_x = 0;
		_size_y = 0;
		_video_type = 0;
		_offset_y = 0;
		_offset_x = 0;
		_alfa = 0;
	}

	// Parameters
	unsigned int _unit_number;
	unsigned int _size_x;
	unsigned int _size_y;
	unsigned int _video_type;
	int _offset_y;				/** NOT USED */
	int _offset_x;				/** NOT USED */
	float _alfa;				/** NOT USED */
};

class yarp::dev::DragonflyDeviceDriver : 
	public FrameGrabber
{
private:
	DragonflyDeviceDriver(const DragonflyDeviceDriver&);
	void operator=(const DragonflyDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	DragonflyDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~DragonflyDeviceDriver();

    /**
	 * Open the device driver.
     * @par parameters for the device driver
	 * @return returns true on success, false on failure.
	 */
    bool open(const DragonflyOpenParameters& par);

	/**
	 * Closes the device driver.
	 * @return returns true/false on success/failure.
	 */
	virtual bool close(void);

    /**
    * Implements FrameGrabber basic interface.
    * @return returns true/false on success/failure.
    */
    virtual bool getBuffer(unsigned char *buffer);

    /**
    * Implements FrameGrabber basic interface.
    */
    virtual int getHeight();
    
    /**
    * Implements FrameGrabber basic interface.
    */
    virtual int getWidth();

protected:
	/**
	 * Locks the current image buffer.
	 * @param buffer is a pointer to the buffer address (i.e. a double pointer).
	 * @return true if successful, false otherwise.
	 */
	bool acquireBuffer(void *buffer);

	/**
	 * Releases the current image buffer.
	 * @return true if successful, false otherwise.
	 */
	bool releaseBuffer();

	/**
	 * Waits on a new frame. An event is signaled when a new frame is acquired,
	 * the calling thread waits efficiently on this event.
	 * @return true on success, false otherwise (God forbid)
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