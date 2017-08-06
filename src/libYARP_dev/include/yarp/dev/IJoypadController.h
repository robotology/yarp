/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IJOYPADCONTROLLER_H
#define YARP_DEV_IJOYPADCONTROLLER_H

#include <yarp/sig/Vector.h>
#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/RateThread.h>
#include <map>
#include <vector>

#define HAT_ACTIONS_ID_SHIFT 100

namespace yarp
{
    namespace dev
    {
        class IJoypadController;
        class IJoypadEvent;
        class IJoypadEventDriven;
    }
}

class YARP_dev_API yarp::dev::IJoypadController
{
public:
    enum JoypadCtrl_coordinateMode {JypCtrlcoord_POLAR  =  0, JypCtrlcoord_CARTESIAN = 1};

protected:
    std::map<int, std::string> m_actions;

    virtual bool parseActions(const yarp::os::Searchable& cfg, int *count = YARP_NULLPTR);
    virtual bool executeAction(int action_id);

public:

    /**
    Destructor
    * @brief ~IJoypadController
    * @return
    */
    virtual ~IJoypadController(){}

    /**
      Activate event Driven mode
     * @brief eventDriven
     * @param enable a bool to turn on or off the eventDriven mode
     * * @param event a pointer to a valid yarp::dev::IJoypadEvent object whom action() method will be called on event detection
     * @return true if succeded. false otherwise
     */
    virtual bool eventDriven(bool enable, yarp::dev::IJoypadEvent* event = YARP_NULLPTR){return false;}
    virtual bool isEventDriven(){return false;}

    /**
      Get number of Axes
     * @brief getAxisCount
     * @param axes_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getAxisCount(unsigned int& axis_count) = 0;

    /**
      Get number of Buttons
     * @brief getButtonCount
     * @param button_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getButtonCount(unsigned int& button_count) = 0;

    /**
      Get number of trackballs
     * @brief getTrackballCount
     * @param Trackball_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getTrackballCount(unsigned int& Trackball_count) = 0;

    /**
      Get number of Hats
     * @brief getHatsCount
     * @param Hat_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getHatCount(unsigned int& Hat_count) = 0;

    /**
      get the number of touch surface. multiple touch surface can rappresent either multiple physical monotouch surface or a multitouch surface
      or a combination of the two
     * @brief hasTouchSurface
     * @param touch_count unsigned int reference that will contain the result
     * @return true if succeded. false otherwise
     */
    virtual bool getTouchSurfaceCount(unsigned int& touch_count) = 0;

    /**
      get the number of the sticks
     * @brief getStickCount
     * @param stick_count unsigned int reference that will contain the result
     * @return bool if succeded, false otherwise
     */
    virtual bool getStickCount(unsigned int& stick_count) = 0;

    /**
     Get the Degree Of Freedom count for desired stick
    * @brief getStickDoF
    * @param stick_id Id of the stick. must be > -1 && < getStickCount(), return false otherwise
    * @param DoF an unsigned int reference that will contain the value.
    * @return true if succeded, false otherwise
    */
    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF) = 0;

    /**
     Get the value of a button. from 0-unpressed to 1-fullpressed and values in the middle in the analog-face-button case
    * @brief getButton
    * @param button_id Id of the button to get. Must be > -1 && < getButtonCount(), return false otherwise
    * @param value reference to be valued. The value will be from 0.0 (not even touched) to 1.0 (fully pressed)
    * @return true if succeded, false otherwise
    */
    virtual bool getButton(unsigned int button_id, float& value) = 0;

    /**
     Get the axes change of a Trackball.
    * @brief getTrackball
    * @param trackball_id Id of the Trackball to get. Must be > -1 && < getTrackballCount(), return false otherwise
    * @param value reference to be valued. the value will be from 0.0 (not moving) to 1.0 (full velocity)
    * @return true if succeded, false otherwise
    */
    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) = 0;

    /**
     Get the value of an Hat.
    * @brief getHat
    * @param hat_id Id of the POV hat to get. Must be > -1 && < getHatCount(), return false otherwise
    * @param value reference to be valued. use the YRPJOY_HAT_ macro series to get the currently pressed directions
    * @return true if succeded, false otherwise
    */
    virtual bool getHat(unsigned int hat_id, unsigned char& value) = 0;

    /**
     Get the value of an axis if present, return false otherwise
    * @brief getAxis
    * @param axis_id Id of the axis to get. must be > -1 && < getAxisCount(), return false otherwise
    * @param value reference to be valued. the absolute boundaries for the values should be -1.0 and 1.0 However
    * the actual range depends on the physical device (example: analog trigger does not have a central position thus can
    * give a value from 0.0 to 0.1)
    * @return true if succeded, false otherwise
    */
    virtual bool getAxis(unsigned int axis_id, double& value) = 0;

    /**
     Get the value of a stick if present, return false otherwise.
    * @brief getStick
    * @param stick_id Id of the stick to get. must be > -1 && < getStickCount(), return false otherwise
    * @param value a vector that will contain the joystick position. the size of the vector will give you the Number of degrees of freedom
    * and the value will be from -1.0 to 1.0
    * @param coordinate_mode to get data in cartesian mode or polar (spheric in 3 dof position cases) mode
    * @return true if succeded, false otherwise
    */
    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) = 0;

    /**
     Get the value of a touch if present, return false otherwise
    * @brief getTouch
    * @param value a vector that will contain the touch values normalized from 0.0 to 1.0.
    * @return true if succeded, false otherwise
    */
    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) = 0;
};

class YARP_dev_API yarp::dev::IJoypadEvent
{
public:
    virtual ~IJoypadEvent();

    template <typename T> struct joyData
    {
        unsigned int m_id;
        T            m_datum;

        joyData(unsigned int id, const T& datum)
        {
            m_id    = id;
            m_datum = datum;
        }
    };

    virtual void action(std::vector<joyData<float> >             buttons,
                        std::vector<joyData<double> >            axes,
                        std::vector<joyData<unsigned char> >     hats,
                        std::vector<joyData<yarp::sig::Vector> > trackBalls,
                        std::vector<joyData<yarp::sig::Vector> > sticks,
                        std::vector<joyData<yarp::sig::Vector> > Touch) = 0;
};



class YARP_dev_API yarp::dev::IJoypadEventDriven : yarp::os::RateThread,
                                                   public yarp::dev::IJoypadController
{
private:
    yarp::dev::IJoypadEvent*       m_event;
    bool                           EventDrivenEnabled;
    std::vector<float>             old_buttons;
    std::vector<double>            old_axes;
    std::vector<unsigned char>     old_hats;
    std::vector<yarp::sig::Vector> old_trackballs;
    std::vector<yarp::sig::Vector> old_sticks;
    std::vector<yarp::sig::Vector> old_touches;
protected:
    virtual bool getRawAxisCount(unsigned int& axis_count) = 0;
    virtual bool getRawButtonCount(unsigned int& button_count) = 0;
    virtual bool getRawTrackballCount(unsigned int& Trackball_count) = 0;
    virtual bool getRawHatCount(unsigned int& Hat_count) = 0;
    virtual bool getRawTouchSurfaceCount(unsigned int& touch_count) = 0;
    virtual bool getRawStickCount(unsigned int& stick_count) = 0;
    virtual bool getRawStickDoF(unsigned int stick_id, unsigned int& DoF) = 0;
    virtual bool getRawButton(unsigned int button_id, float& value) = 0;
    virtual bool getRawTrackball(unsigned int trackball_id, yarp::sig::Vector& value) = 0;
    virtual bool getRawHat(unsigned int hat_id, unsigned char& value) = 0;
    virtual bool getRawAxis(unsigned int axis_id, double& value) = 0;
    virtual bool getRawStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) = 0;
    virtual bool getRawTouch(unsigned int touch_id, yarp::sig::Vector& value) = 0;
    using IJoypadController::m_actions;
    using IJoypadController::executeAction;
    using IJoypadController::parseActions;

public:

    virtual bool getAxisCount(unsigned int& axis_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getButtonCount(unsigned int& button_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getTrackballCount(unsigned int& Trackball_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getHatCount(unsigned int& Hat_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getTouchSurfaceCount(unsigned int& touch_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getStickCount(unsigned int& stick_count) YARP_OVERRIDE YARP_FINAL;
    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF) YARP_OVERRIDE YARP_FINAL;
    virtual bool getButton(unsigned int button_id, float& value) YARP_OVERRIDE YARP_FINAL;
    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) YARP_OVERRIDE YARP_FINAL;
    virtual bool getHat(unsigned int hat_id, unsigned char& value) YARP_OVERRIDE YARP_FINAL;
    virtual bool getAxis(unsigned int axis_id, double& value) YARP_OVERRIDE YARP_FINAL;
    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) YARP_OVERRIDE YARP_FINAL;
    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) YARP_OVERRIDE YARP_FINAL;
    using IJoypadController::JoypadCtrl_coordinateMode;
    using IJoypadController::JypCtrlcoord_CARTESIAN;
    using IJoypadController::JypCtrlcoord_POLAR;



    IJoypadEventDriven();

    IJoypadEventDriven(int rate);

    virtual bool threadInit() YARP_OVERRIDE;
    virtual void run() YARP_OVERRIDE;

    virtual bool eventDriven(bool enable, yarp::dev::IJoypadEvent* event = YARP_NULLPTR) YARP_OVERRIDE;
    virtual bool isEventDriven() YARP_OVERRIDE { return EventDrivenEnabled;}
};


#define YRPJOY_HAT_CENTERED	 0x00
#define YRPJOY_HAT_UP		 0x01
#define YRPJOY_HAT_RIGHT	 0x02
#define YRPJOY_HAT_DOWN		 0x04
#define YRPJOY_HAT_LEFT		 0x08
#define YRPJOY_HAT_RIGHTUP	 (YRPJOY_HAT_RIGHT|YRPJOY_HAT_UP)
#define YRPJOY_HAT_RIGHTDOWN     (YRPJOY_HAT_RIGHT|YRPJOY_HAT_DOWN)
#define YRPJOY_HAT_LEFTUP	 (YRPJOY_HAT_LEFT |YRPJOY_HAT_UP)
#define YRPJOY_HAT_LEFTDOWN	 (YRPJOY_HAT_LEFT |YRPJOY_HAT_DOWN)

#define VOCAB_OK          VOCAB2('o','k')
#define VOCAB_FAILED      VOCAB4('f','a','i','l')
#define VOCAB_IJOYPADCTRL VOCAB4('i','j','p','c')
#ifndef VOCAB_GET
#define VOCAB_GET         VOCAB3('g','e','t')
#endif
#define VOCAB_BUTTON      VOCAB4('b','u','t','n')
#define VOCAB_TRACKBALL   VOCAB4('t','r','b','l')
#define VOCAB_HAT         VOCAB3('h','a','t')
#define VOCAB_AXIS        VOCAB4('a','x','i','s')
#define VOCAB_STICK       VOCAB4('s','t','c','k')
#define VOCAB_STICKDOF    VOCAB4('s','d','o','f')
#define VOCAB_TOUCH       VOCAB4('t','u','c','h')
#define VOCAB_COUNT       VOCAB3('c','n','t')
#define VOCAB_VALUE       VOCAB3('v','a','l')
#define VOCAB_POLAR       VOCAB4('p','o','l','r')
#define VOCAB_CARTESIAN   VOCAB4('c','a','r','t')

#endif //#define YARP_DEV_IJOYPADCONTROLLER_H

//todo list 23/03/2017..
//1. complete the single port functionality between server and client and test
//2. complete the rpc_only functionality in the client (add the parameter in the open() and test it)
//3. sdl hat #define to vocab traduction (just in case they change something)
//4. SDLJoypad implement stick functionality
//5. SDLJoypad implement deadband
//6. SDLJoypad implement button/axis remapping
