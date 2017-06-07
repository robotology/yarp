/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "JoypadControlServer.h"
#include <map>
#include <vector>
#include <yarp/os/LogStream.h>

#define DEFAULT_THREAD_PERIOD   10 //ms

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
JoypadCtrlParser::JoypadCtrlParser() : device(YARP_NULLPTR){}

inline void cat(Vector& a, const Vector& b)
{
    for (size_t i = 0; i < b.size(); i++)
    {
        a.push_back(b[i]);
    }
}

bool JoypadCtrlParser::configure(yarp::dev::IJoypadController* interface)
{
    bool ret;
    ret = false;
    if(interface)
    {
        device = interface;
        ret = true;
    }
    else
    {
        device = YARP_NULLPTR;
        ret = false;
    }

    countGetters.insert(make_pair(VOCAB_BUTTON,    &IJoypadController::getButtonCount));
    countGetters.insert(make_pair(VOCAB_HAT,       &IJoypadController::getHatCount));
    countGetters.insert(make_pair(VOCAB_TRACKBALL, &IJoypadController::getTrackballCount));
    countGetters.insert(make_pair(VOCAB_AXIS,      &IJoypadController::getAxisCount));
    countGetters.insert(make_pair(VOCAB_STICK,     &IJoypadController::getStickCount));
    countGetters.insert(make_pair(VOCAB_TOUCH,     &IJoypadController::getTouchSurfaceCount));

    return ret;
}

bool JoypadCtrlParser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret;

    ret = false;
    if(cmd.get(0).asVocab() != VOCAB_IJOYPADCTRL || !cmd.get(1).isVocab() || !cmd.get(2).isVocab() || !cmd.get(3).isVocab())
    {
        response.addVocab(VOCAB_FAILED);
        return ret;
    }

    if(cmd.get(1).asVocab() == VOCAB_GET)
    {
        int toGet;

        toGet = cmd.get(2).asVocab();

        if(cmd.get(3).asVocab() == VOCAB_COUNT)
        {
            if(countGetters.find(toGet) != countGetters.end())
            {
                unsigned int   count;
                getcountmethod getter;
                getter = countGetters[toGet];
                if((device->*getter)(count))
                {
                    response.addVocab(VOCAB_OK);
                    response.addInt(count);
                    ret = true;
                }
            }
            else if (toGet == VOCAB_STICKDOF && cmd.get(4).isInt())
            {
                unsigned int count;
                if (device->getStickDoF(cmd.get(4).asInt(), count))
                {
                    response.addVocab(VOCAB_OK);
                    response.addInt(count);
                    ret = true;
                }
                else
                {
                    response.addVocab(VOCAB_FAILED);
                    ret = false;
                }
            }
            else
            {
                response.addVocab(VOCAB_FAILED);
                ret = false;
            }
        }
        else if(cmd.get(3).asVocab() == VOCAB_VALUE)
        {
            switch (cmd.get(2).asVocab()) {
            case VOCAB_BUTTON:
            {
                float value;
                if(cmd.get(4).isInt() && device->getButton(cmd.get(4).asInt(), value))
                {
                    response.addVocab(VOCAB_OK);
                    response.addDouble(value);
                    ret = true;
                }
                break;
            }
            case VOCAB_AXIS:
            {
                double value;
                if(cmd.get(4).isInt() && device->getAxis(cmd.get(4).asInt(), value))
                {
                    response.addVocab(VOCAB_OK);
                    response.addDouble(value);
                    ret = true;
                }
                break;
            }
            case VOCAB_STICK:
            {
                if(cmd.get(4).isVocab())
                {
                    yarp::sig::Vector frame;

                    auto mode = cmd.get(4).asVocab() == VOCAB_CARTESIAN ? yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN : yarp::dev::IJoypadController::JypCtrlcoord_POLAR;
                    if(cmd.get(5).isInt() && device->getStick(cmd.get(5).asInt(), frame, mode))
                    {
                        response.addVocab(VOCAB_OK);
                        for(size_t i = 0; i < frame.size(); ++i)
                        {
                            response.addDouble(frame[i]);
                        }

                        ret = true;
                    }
                }
                break;
            }
            case VOCAB_STICKDOF:
            {

                unsigned int dofCount;

                if(cmd.get(5).isInt() && device->getStickDoF(cmd.get(5).asInt(), dofCount))
                {
                    response.addVocab(VOCAB_OK);
                    response.addInt(dofCount);
                    ret = true;
                }

                break;
            }
            case VOCAB_TOUCH:
            {
                yarp::sig::Vector pos;
                unsigned int      id;

                id = cmd.get(4).asInt();
                if(cmd.get(4).isInt() && device->getTouch(id, pos))
                {
                    response.addVocab(VOCAB_OK);
                    for(size_t i = 0; i < pos.size(); ++i)
                    {
                        response.addDouble(pos[i]);
                    }
                    ret = true;
                }
                break;
            }
            case VOCAB_TRACKBALL:
            {
                yarp::sig::Vector axes;
                unsigned int      id;

                id = cmd.get(4).asInt();
                if(cmd.get(4).isInt() && device->getTrackball(id, axes))
                {
                    response.addVocab(VOCAB_OK);
                    for(size_t i = 0; i < axes.size(); ++i)
                    {
                        response.addDouble(axes[i]);
                    }
                    ret = true;
                }
                break;
            }
            case VOCAB_HAT:
            {
                unsigned char value;
                if(cmd.get(4).isInt() && device->getHat(cmd.get(4).asInt(), value))
                {
                    response.addVocab(VOCAB_OK);
                    response.addInt(value);
                    ret = true;
                }
                break;
            }
            default:
                break;
            }
        }
    }
    return ret;

}


JoypadControlServer::JoypadControlServer() : RateThread(DEFAULT_THREAD_PERIOD),
                                             m_rate(DEFAULT_THREAD_PERIOD),
                                             m_device(YARP_NULLPTR),
                                             m_subDeviceOwned(YARP_NULLPTR),
                                             m_separatePorts(false)
{

}

JoypadControlServer::~JoypadControlServer()
{
    if(m_subDeviceOwned)
    {
        delete m_subDeviceOwned;
    }
    m_subDeviceOwned = YARP_NULLPTR;
    m_device         = YARP_NULLPTR;
}

bool JoypadControlServer::open(yarp::os::Searchable& params)
{
    if(params.check("help"))
    {
        yInfo() << "parameters:\n\n" <<
                   "period             - refresh period of the broadcasted values in ms.. default" << DEFAULT_THREAD_PERIOD << "\n"
                   "use_separate_ports - set it to 1 to use separate ports (buttons, axes, trackballs, hats) and 0 to stream all in one single port\n" <<
                   "name               - Prefix name of the ports opened by the JoypadControlServer, e.g. /robotName/joypad\n" <<
                   "subdevice          - name of the subdevice to open\n" <<
                   "profile            - print the joypad data for debugging purpose";
        return false;
    }
    yarp::os::ConstString rootName;
    if (!params.check("period", "refresh period of the broadcasted values in ms"))
    {
        yInfo() << "JoypadControlServer: using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "ms";
    }
    else
    {
        m_rate = params.find("period").asInt();
    }

    m_profile = params.check("profile");

    if(params.check("use_separate_ports"))
    {
        m_separatePorts = params.find("use_separate_ports").asBool();
        if(!m_separatePorts)
        {
            yError() << "single port mode not supported at the moment";
            return false;
        }
    }
    else
    {
        yError() << "JoypadControlServer: missing use_separate_ports in configuration";
        return false;
    }
    m_coordsMode = yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN;
    rootName = params.check("name",Value("/"), "starting '/' if needed.").asString().c_str();

    if (!params.check("name", "Prefix name of the ports opened by the JoypadControlServer."))
    {
        yError() << "JoypadControlServer: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   name:         Prefix name of the ports opened by the JoypadControlServer, e.g. /robotName/joypad";
        return false;
    }

        rootName             = params.find("name").asString().c_str();
        m_rpcPortName        = rootName + "/rpc:i";
        m_portButtons.name   = rootName + "/buttons:o";
        m_portAxis.name      = rootName + "/axis:o";
        m_portStick.name     = rootName + "/stick:o";
        m_portTouch.name     = rootName + "/touch:o";
        m_portTrackball.name = rootName + "/trackball:o";
        m_portHats.name      = rootName + "/hat:o";


    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(params.check("subdevice"))
    {
        m_isSubdeviceOwned=true;
        if(!openAndAttachSubDevice(params))
        {
            yError("JoypadControlServer: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        m_isSubdeviceOwned=false;
    }
    return true;
}

bool JoypadControlServer::openAndAttachSubDevice(Searchable& prop)
{
    Property p;

    m_subDeviceOwned = new PolyDriver;

    p.fromString(prop.toString().c_str());
    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if error occour during open, quit here.
    m_subDeviceOwned->open(p);

    if (!m_subDeviceOwned->isValid())
    {
        yError("JoypadControlServer: opening subdevice... FAILED\n");
        return false;
    }
    m_isSubdeviceOwned = true;
    if(!attach(m_subDeviceOwned))
        return false;

    if(!m_parser.configure(m_device) )
    {
        yError() << "JoypadControlServer: error configuring interfaces for parsers";
        return false;
    }

    openPorts();
    RateThread::setRate(m_rate);
    RateThread::start();
    return true;
}

bool JoypadControlServer::attach(PolyDriver* poly)
{
    if(poly)
        poly->view(m_device);

    if(m_device == YARP_NULLPTR)
    {
        yError() << "JoypadControlServer: attached device has no valid IJoypadController interface.";
        return false;
    }
    return true;
}

bool JoypadControlServer::attach(yarp::dev::IJoypadController *s)
{
    if(s == YARP_NULLPTR)
    {
        yError() << "JoypadControlServer: attached device has no valid IJoystickController interface.";
        return false;
    }
    m_device = s;
    return true;
}

bool JoypadControlServer::detach()
{
    m_device = YARP_NULLPTR;
    return true;
}

bool JoypadControlServer::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void JoypadControlServer::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

bool JoypadControlServer::openPorts()
{
    if(!m_device)
    {
        return false;
    }

    if(!m_rpcPort.open(m_rpcPortName))
    {
        yError() << "JoypadControlServer: unable to open rpc Port" << m_rpcPortName.c_str();
        return false;
    }
    m_rpcPort.setReader(m_parser);
//    dumb(or K.I.S.S.) version of the method:
//    unsigned int count;
//    if(m_device->getAxisCount(count))
//    {
//        if(count == 0)
//        {
//            m_portAxis.valid = false;
//        }
//        else
//        {
//            m_portAxis.open();
//            m_portAxis.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//
//    if(m_device->getButtonCount(count))
//    {
//        if(count == 0)
//        {
//            m_portButton.valid = false;
//        }
//        else
//        {
//            m_portButton.open();
//            m_portButton.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//
//    if(m_device->getStickCount(count))
//    {
//        if(count == 0)
//        {
//            m_portStick.valid = false;
//        }
//        else
//        {
//            m_portStick.open();
//            m_portStick.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//
//    if(m_device->getTouchSurfaceCount(count))
//    {
//        if(count == 0)
//        {
//            m_portTouch.valid = false;
//        }
//        else
//        {
//            m_portTouch.open();
//            m_portTouch.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//    if(m_device->getTrackballCount(count))
//    {
//        if(count == 0)
//        {
//            m_portTrackball.valid = false;
//        }
//        else
//        {
//            m_portTrackball.open();
//            m_portTrackball.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//
//    if(m_device->getHatCount(count))
//    {
//        if(count == 0)
//        {
//            m_portHats.valid = false;
//        }
//        else
//        {
//            m_portHats.open();
//            m_portHats.valid = true;
//        }
//    }
//    else
//    {
//        return false;
//    }
//    return true;
    if(m_separatePorts)
    {
        typedef bool (IJoypadController::*countGet)(unsigned int&);

        struct solver
        {
            countGet                     getter;
            JoypadControl::LoopablePort* port;

            solver(countGet a, JoypadControl::LoopablePort* b) : getter(a), port(b)
            {}
        };

        vector<solver> getters;

        getters.push_back(solver(&IJoypadController::getAxisCount,         &m_portAxis     ));
        getters.push_back(solver(&IJoypadController::getButtonCount,       &m_portButtons  ));
        getters.push_back(solver(&IJoypadController::getStickCount,        &m_portStick    ));
        getters.push_back(solver(&IJoypadController::getTouchSurfaceCount, &m_portTouch    ));
        getters.push_back(solver(&IJoypadController::getTrackballCount,    &m_portTrackball));
        getters.push_back(solver(&IJoypadController::getHatCount,          &m_portHats     ));

        for(size_t i = 0; i < getters.size(); ++i)
        {
            if((m_device->*(getters[i].getter))(getters[i].port->count))
            {
                if(getters[i].port->count == 0)
                {
                    getters[i].port->valid = false;
                }
                else
                {
                    getters[i].port->contactable->open(getters[i].port->name);
                    getters[i].port->valid = true;
                }
            }
            else
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        return false;
        //m_godPort.open(m_name + "/joydata:o");
    }
}

void JoypadControlServer::profile()
{
    string       message;
    unsigned int count;

    message = "Axes: ";
    m_device->getAxisCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        double data;
        m_device->getAxis(i, data);
        message += to_string(data) + " ";
    }
    yInfo() << message;

    message = "Hats: ";
    m_device->getHatCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        unsigned char data;
        m_device->getHat(i, data);
        message += to_string(data) + " ";
    }
    yInfo() << message;

    message = "Buttons: ";
    m_device->getButtonCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        float data;
        m_device->getButton(i, data);
        message += to_string(data) + " ";
    }
    yInfo() << message;

    message = "Stick: ";
    m_device->getStickCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_device->getStick(i, data, yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN);
        message += "n_" + to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += to_string(data[j]) + " ";
        }
        message += "\n";

    }
    yInfo() << message;

    message = "trackball: ";
    m_device->getTrackballCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_device->getTrackball(i, data);
        message += "n_" + to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += to_string(data[j]) + " ";
        }
        message += "\n";
    }

    message = "touch Surface: ";
    m_device->getTouchSurfaceCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_device->getTouch(i, data);
        message += "n_" + to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += to_string(data[j]) + " ";
        }
        message += "\n";
    }
    yInfo() << message;
}

void JoypadControlServer::run()
{
    if(m_separatePorts)
    {
        if (m_portButtons.valid)
        {
            bool write;
            write = true;
            Vector& b = m_portButtons.prepare();
            b.clear();
            for(size_t i = 0; i < m_portButtons.count; ++i)
            {
                float v;
                if(!m_device->getButton(i, v))
                {
                    write = false;
                    break;
                }
                b.push_back(v);
            }
            if(write)m_portButtons.write();
        }

        if (m_portHats.valid)
        {
            bool write;

            write = true;
            VecOfChar& b = m_portHats.prepare();
            b.clear();
            for(size_t i = 0; i < m_portHats.count; ++i)
            {
                unsigned char v;
                if(!m_device->getHat(i, v))
                {
                    write = false;
                    break;
                }
                b.push_back(v);
            }
            if(write)m_portHats.write();
        }

        if (m_portAxis.valid)
        {
            bool write;

            write = true;
            Vector& b = m_portAxis.prepare();
            b.clear();
            for(size_t i = 0; i < m_portAxis.count; ++i)
            {
                double v;
                if(!m_device->getAxis(i, v))
                {
                    yError() << "cannot get axis with id" << i;
                    write = false;
                    break;
                }
                b.push_back(v);
            }
            if(write)m_portAxis.write();
        }

        if (m_portTrackball.valid)
        {
            bool write;

            write     = true;
            Vector& b = m_portTrackball.prepare();
            b.clear();
            for(size_t i = 0; i < m_portTrackball.count; ++i)
            {
                Vector v;
                if(!m_device->getTrackball(i, v))
                {
                    yError() << "cannot get axis with id" << i;
                    write = false;
                    break;
                }
                cat(b, v);
            }
            if(write)m_portTrackball.write();
        }

        if (m_portStick.valid)
        {
            bool write;
            write = true;
            Vector& b = m_portStick.prepare();
            b.clear();
            for(size_t i = 0; i < m_portStick.count; ++i)
            {
                Vector       v;
                unsigned int dofCount;
                if(!m_device->getStick(i, v, m_coordsMode) || !m_device->getStickDoF(i, dofCount) || v.size() != dofCount)
                {
                    write = false;
                    break;
                }
                cat(b, v);
            }
            if(write)m_portStick.write();
        }

        if (m_portTouch.valid)
        {
            bool write;
            write = true;
            Vector& b = m_portTouch.prepare();
            b.clear();
            for(unsigned int i = 0; i < m_portTouch.count; ++i)
            {
                Vector v;
                if(!m_device->getTouch(i, v))
                {
                    write = false;
                    break;
                }
                cat(b, v);
            }

            if(write)m_portTouch.write();
        }
    }
    else
    {
        return;
        //JoyData& message = m_godPort.prepare();
        //for(size_t i = 0; i < m_device->getAxisCount();)
        //message.Axes
    }

    if(m_profile)
    {
        profile();
    }
}

bool JoypadControlServer::attachAll(const PolyDriverList& p)
{
    if (p.size() != 1)
    {
        yError("JoypadControlServer: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver* Idevice2attach = p[0]->poly;
    if(p[0]->key == "IJoypadController")
    {
        yInfo() << "JoypadControlServer: Good name Dude!";
    }
    else
    {
        yInfo() << "JoypadControlServer: Bad name Dude!!";
    }

    if (!Idevice2attach->isValid())
    {
        yError() << "JoypadControlServer: Device " << p[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(m_device);
    if(!attach(m_device))
        return false;

    RateThread::setRate(m_rate);
    RateThread::start();

    openPorts();
    return true;
}

bool JoypadControlServer::detachAll()
{
    if (yarp::os::RateThread::isRunning())
        yarp::os::RateThread::stop();

    //check if we already instantiated a subdevice previously
    if (m_isSubdeviceOwned)
        return false;

    m_device = YARP_NULLPTR;
    return true;
}

bool JoypadControlServer::close()
{
    detachAll();

    // close subdevice if it was created inside the open (--subdevice option)
    if(m_isSubdeviceOwned)
    {
        if(m_subDeviceOwned)m_subDeviceOwned->close();

        m_subDeviceOwned   = YARP_NULLPTR;
        m_device           = YARP_NULLPTR;
        m_isSubdeviceOwned = false;
    }

    // Closing port
    vector<JoypadControl::LoopablePort*> portv;
    portv.push_back(&m_portButtons);
    portv.push_back(&m_portAxis);
    portv.push_back(&m_portStick);
    portv.push_back(&m_portTouch);
    portv.push_back(&m_portTrackball);
    portv.push_back(&m_portHats);

    for(auto p : portv)
    {
        //p->contactable->interrupt();
        p->contactable->close();
    }

    m_rpcPort.close();
    return true;
}

// needed for the driver factory.
yarp::dev::DriverCreator* createJoypadControlServer()
{
    return new DriverCreatorOf<yarp::dev::JoypadControlServer>("JoypadControlServer", "JoypadControlServer", "yarp::dev::JoypadControlServer");
}
