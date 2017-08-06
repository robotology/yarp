#include <yarp/dev/IJoypadController.h>
#include <yarp/os/LogStream.h>
#include <cmath>
using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
#define MESSAGE_PREFIX "IJoypadController:"
#define myInfo() yInfo() << MESSAGE_PREFIX
#define myError() yError() << MESSAGE_PREFIX
#define myDebug() yDebug() << MESSAGE_PREFIX
#define myWarning() yWarning() << MESSAGE_PREFIX
#define buttActionGroupName "BUTTON_EXECUTE"

#define JoyData yarp::dev::IJoypadEvent::joyData

yarp::dev::IJoypadEventDriven::IJoypadEventDriven() : IJoypadEventDriven(10){}

yarp::dev::IJoypadEventDriven::IJoypadEventDriven(int rate) : RateThread(rate)
{
    EventDrivenEnabled = false;
    m_event = YARP_NULLPTR;
}

bool isEqual(const float& a, const float& b, const float& tollerance)
{
    return fabs(a - b) < tollerance;
}

bool isEqual(const double& a, const double& b, const double& tollerance)
{
    return fabs(a - b) < tollerance;
}

bool isEqual(const yarp::sig::Vector& a, const yarp::sig::Vector& b, const double& tollerance)
{
    if (a.size() != b.size()) return false;

    bool ret = true;

    for (size_t i = 0; i < a.size(); i++)
    {
        if (ret &= fabs(a[i] - b[i]) < tollerance);
    }
    return ret;
}

bool yarp::dev::IJoypadEventDriven::threadInit()
{
    unsigned int count;
    if(getRawButtonCount(count) && count)
    {
        float value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawButton(i, value);
            old_buttons.push_back(value);
        }
    }

    if(getRawAxisCount(count) && count)
    {
        double value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawAxis(i, value);
            old_axes.push_back(value);
        }
    }

    if(getRawHatCount(count) && count)
    {
        unsigned char value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawHat(i, value);
            old_hats.push_back(value);
        }
    }

    if(getRawTrackballCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawTrackball(i, value);
            old_trackballs.push_back(value);
        }
    }

    if(getRawStickCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawStick(i, value, IJoypadController::JypCtrlcoord_CARTESIAN);
            old_sticks.push_back(value);

        }
    }

    if(getRawTouchSurfaceCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawTouch(i, value);
            old_touches.push_back(value);
        }
    }
    return true;
}

void yarp::dev::IJoypadEventDriven::run()
{
    bool perform = false;
    if(!m_event)
    {
        return;
    }

    unsigned int                             count;
    std::vector<JoyData<float> >             buttons;
    std::vector<JoyData<double> >            axes;
    std::vector<JoyData<unsigned char> >     hats;
    std::vector<JoyData<yarp::sig::Vector> > trackBalls;
    std::vector<JoyData<yarp::sig::Vector> > sticks;
    std::vector<JoyData<yarp::sig::Vector> > Touch;

    if(getRawButtonCount(count) && count)
    {
        float value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawButton(i, value);
            if(!isEqual(value, old_buttons[i], 0.00001f))
            {
                perform = true;
                buttons.push_back(JoyData<float>(i, value));
                old_buttons[i] = value;
            }
        }
    }

    if(getRawAxisCount(count) && count)
    {
        double value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawAxis(i, value);
            if(!isEqual(value, old_axes[i], 0.00001))
            {
                perform = true;
                axes.push_back(JoyData<double>(i, value));
                old_axes[i] = value;
            }
        }
    }

    if(getRawHatCount(count) && count)
    {
        unsigned char value = 0;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawHat(i, value);
            if(value != old_hats[i])
            {
                perform = true;
                hats.push_back(JoyData<unsigned char>(i, value));
                old_hats[i] = value;
            }
        }
    }

    if(getRawTrackballCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawTrackball(i, value);
            if(!isEqual(value, old_trackballs[i], 0.00001))
            {
                perform = true;
                trackBalls.push_back(JoyData<yarp::sig::Vector>(i, value));
                old_trackballs[i] = value;
            }
        }
    }

    if(getRawStickCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawStick(i, value, IJoypadController::JypCtrlcoord_CARTESIAN);
            if(!isEqual(value, old_sticks[i], 0.00001))
            {
                perform = true;
                sticks.push_back(JoyData<yarp::sig::Vector>(i, value));
                old_sticks[i] = value;
            }
        }
    }

    if(getRawTouchSurfaceCount(count) && count)
    {
        yarp::sig::Vector value;
        for(unsigned int i = 0; i < count; i++)
        {
            getRawTouch(i, value);
            if(!isEqual(value, old_touches[i], 0.00001))
            {
                perform = true;
                Touch.push_back(JoyData<yarp::sig::Vector>(i, value));
                old_touches[i] = value;
            }
        }
    }

    if(perform)
    {
        m_event->action(buttons, axes, hats, trackBalls, sticks, Touch);
    }
}

bool yarp::dev::IJoypadEventDriven::eventDriven(bool enable, IJoypadEvent *event)
{
    if (enable)
    {
        if (event)
        {
            if(isRunning())
            {
                stop();
            }

            m_event = event;
            EventDrivenEnabled = true;
            start();

            return true;
        }
        else
        {
            if(isRunning())
            {
                yError() << "IJoypadController: event thread is already running";
                return false;
            }

            if (m_event)
            {
                EventDrivenEnabled = true;
                start();
                return true;
            }
            else
            {
                yError() << "IJoypadController: you must provide a valid event to start the event thread";
                return false;
            }
        }
    }

    if(isRunning())
    {
        stop();
        EventDrivenEnabled = false;
        return true;
    }

    return false;
}

bool yarp::dev::IJoypadEventDriven::getAxisCount(unsigned int& axis_count)
{
    return getRawAxisCount(axis_count);
}

bool yarp::dev::IJoypadEventDriven::getButtonCount(unsigned int& button_count)
{
    return getRawButtonCount(button_count);
}

bool yarp::dev::IJoypadEventDriven::getTrackballCount(unsigned int& Trackball_count)
{
    return getRawTrackballCount(Trackball_count);
}

bool yarp::dev::IJoypadEventDriven::getHatCount(unsigned int& Hat_count)
{
    return getRawHatCount(Hat_count);
}

bool yarp::dev::IJoypadEventDriven::getTouchSurfaceCount(unsigned int& touch_count)
{
    return getRawTouchSurfaceCount(touch_count);
}

bool yarp::dev::IJoypadEventDriven::getStickCount(unsigned int& stick_count)
{
    return getRawStickCount(stick_count);
}

bool yarp::dev::IJoypadEventDriven::getStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }
    return getStickDoF(stick_id, DoF);
}

bool yarp::dev::IJoypadEventDriven::getButton(unsigned int button_id, float& value)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }
    return getRawButton(button_id, value);
}

bool yarp::dev::IJoypadEventDriven::getTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }

    return getRawTrackball(trackball_id, value);
}

bool yarp::dev::IJoypadEventDriven::getHat(unsigned int hat_id, unsigned char& value)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }

    return getRawHat(hat_id, value);
}

bool yarp::dev::IJoypadEventDriven::getAxis(unsigned int axis_id, double& value)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }

    return getRawAxis(axis_id, value);
}

bool yarp::dev::IJoypadEventDriven::getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }

    return getRawStick(stick_id, value, coordinate_mode);
}

bool yarp::dev::IJoypadEventDriven::getTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    if (EventDrivenEnabled)
    {
        yError() << "EventDriven is enable.. you can't poll the joypad state";
        return false;
    }

    return getRawTouch(touch_id, value);
}

bool yarp::dev::IJoypadController::executeAction(int action_id)
{
    if (m_actions.find(action_id) != m_actions.end())
    {
        myInfo() << "executing script" << action_id << ":" << m_actions[action_id];
        system(m_actions[action_id].c_str());
    }
    else
    {
        myWarning() << "no scripts associated to button" << action_id;
        return false;
    }
    return true;
}

bool yarp::dev::IJoypadController::parseActions(const yarp::os::Searchable& cfg, int* count)
{
    int  dummy;
    int  i;
    int& actCount = count ? *count : dummy;
    if(!cfg.check(buttActionGroupName))
    {
        myInfo() << "no actions found in the configuration file (no" << buttActionGroupName << "group found)";
        actCount = 0;
        return true;
    }
    Bottle& actionsGroup = cfg.findGroup(buttActionGroupName);

    if(!actionsGroup.size())
    {
        myError() << "no action found under" << buttActionGroupName << "group";
        actCount = 0;
        return false;
    }

    for(i = 1; i < actionsGroup.size(); i++)
    {
        if(!actionsGroup.get(i).isList())
        {
            yDebug() << "error parsing cfg";
            return false;
        }

        Bottle& keyvalue         = *actionsGroup.get(i).asList();
        yDebug() << keyvalue.toString();
        unsigned int buttonCount;
        if(!this->getButtonCount(buttonCount))
        {
            myError() << "unable to get button count while parsing the actions";
            actCount = 0;
            return false;
        }
        if(!keyvalue.get(0).isInt()                 ||
            keyvalue.get(0).asInt() < 0             ||
            keyvalue.get(0).asInt() > buttonCount-1 ||
           !keyvalue.get(1).isString())
        {
            myError() << "Button's actions parameters must be in the format 'unsigned int string' and the button id must be in range";
            actCount = 0;
            return false;
        }
        myInfo() << "assigning actions" << keyvalue.get(1).asString() << "to button" << keyvalue.get(0).asInt();
        m_actions[keyvalue.get(0).asInt()] = keyvalue.get(1).asString();
    }

    actCount = i;
    myInfo() << actCount << "action parsed succesfully";
    return true;
}

yarp::dev::IJoypadEvent::~IJoypadEvent(){};
