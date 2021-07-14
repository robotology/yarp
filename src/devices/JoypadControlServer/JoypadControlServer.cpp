/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "JoypadControlServer.h"
#include <map>
#include <vector>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#define DEFAULT_THREAD_PERIOD   0.010 //s

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(JOYPADCONTROLSERVER, "yarp.device.JoypadControlServer")
}


JoypadCtrlParser::JoypadCtrlParser() : device(nullptr){}

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
    if(interface)
    {
        device = interface;
        ret = true;
    }
    else
    {
        device = nullptr;
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
    if(cmd.get(0).asVocab32() != VOCAB_IJOYPADCTRL || !cmd.get(1).isVocab32() || !cmd.get(2).isVocab32() || !cmd.get(3).isVocab32())
    {
        response.addVocab32(VOCAB_FAILED);
        return ret;
    }

    if(cmd.get(1).asVocab32() == VOCAB_GET)
    {
        int toGet;

        toGet = cmd.get(2).asVocab32();

        if(cmd.get(3).asVocab32() == VOCAB_COUNT)
        {
            if(countGetters.find(toGet) != countGetters.end())
            {
                unsigned int   count;
                getcountmethod getter;
                getter = countGetters[toGet];
                if((device->*getter)(count))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addInt32(count);
                    ret = true;
                }
            }
            else if (toGet == VOCAB_STICKDOF && cmd.get(4).isInt32())
            {
                unsigned int count;
                if (device->getStickDoF(cmd.get(4).asInt32(), count))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addInt32(count);
                    ret = true;
                }
                else
                {
                    response.addVocab32(VOCAB_FAILED);
                    ret = false;
                }
            }
            else
            {
                response.addVocab32(VOCAB_FAILED);
                ret = false;
            }
        }
        else if(cmd.get(3).asVocab32() == VOCAB_VALUE)
        {
            switch (cmd.get(2).asVocab32()) {
            case VOCAB_BUTTON:
            {
                float value;
                if(cmd.get(4).isInt32() && device->getButton(cmd.get(4).asInt32(), value))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addFloat64(value);
                    ret = true;
                }
                break;
            }
            case VOCAB_AXIS:
            {
                double value;
                if(cmd.get(4).isInt32() && device->getAxis(cmd.get(4).asInt32(), value))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addFloat64(value);
                    ret = true;
                }
                break;
            }
            case VOCAB_STICK:
            {
                if(cmd.get(4).isVocab32())
                {
                    yarp::sig::Vector frame;

                    auto mode = cmd.get(4).asVocab32() == VOCAB_CARTESIAN ? yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN : yarp::dev::IJoypadController::JypCtrlcoord_POLAR;
                    if(cmd.get(5).isInt32() && device->getStick(cmd.get(5).asInt32(), frame, mode))
                    {
                        response.addVocab32(VOCAB_OK);
                        for(size_t i = 0; i < frame.size(); ++i)
                        {
                            response.addFloat64(frame[i]);
                        }

                        ret = true;
                    }
                }
                break;
            }
            case VOCAB_STICKDOF:
            {

                unsigned int dofCount;

                if(cmd.get(5).isInt32() && device->getStickDoF(cmd.get(5).asInt32(), dofCount))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addInt32(dofCount);
                    ret = true;
                }

                break;
            }
            case VOCAB_TOUCH:
            {
                yarp::sig::Vector pos;
                unsigned int      id;

                id = cmd.get(4).asInt32();
                if(cmd.get(4).isInt32() && device->getTouch(id, pos))
                {
                    response.addVocab32(VOCAB_OK);
                    for(size_t i = 0; i < pos.size(); ++i)
                    {
                        response.addFloat64(pos[i]);
                    }
                    ret = true;
                }
                break;
            }
            case VOCAB_TRACKBALL:
            {
                yarp::sig::Vector axes;
                unsigned int      id;

                id = cmd.get(4).asInt32();
                if(cmd.get(4).isInt32() && device->getTrackball(id, axes))
                {
                    response.addVocab32(VOCAB_OK);
                    for(size_t i = 0; i < axes.size(); ++i)
                    {
                        response.addFloat64(axes[i]);
                    }
                    ret = true;
                }
                break;
            }
            case VOCAB_HAT:
            {
                unsigned char value;
                if(cmd.get(4).isInt32() && device->getHat(cmd.get(4).asInt32(), value))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addInt32(value);
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


JoypadControlServer::JoypadControlServer() : PeriodicThread(DEFAULT_THREAD_PERIOD),
                                             m_period(DEFAULT_THREAD_PERIOD),
                                             m_device(nullptr),
                                             m_subDeviceOwned(nullptr),
                                             m_isSubdeviceOwned(false),
                                             m_separatePorts(false),
                                             m_profile(false),
                                             m_coordsMode(yarp::dev::IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_POLAR)
{

}

JoypadControlServer::~JoypadControlServer()
{
    if(m_subDeviceOwned)
    {
        delete m_subDeviceOwned;
    }
    m_subDeviceOwned = nullptr;
    m_device         = nullptr;
}

bool JoypadControlServer::open(yarp::os::Searchable& params)
{
    if(params.check("help"))
    {
        yCInfo(JOYPADCONTROLSERVER)
            << "parameters:\n\n"
            << "period             - refresh period of the broadcasted values in ms.. default" << DEFAULT_THREAD_PERIOD * 1000 << "\n"
            << "use_separate_ports - set it to 1 to use separate ports (buttons, axes, trackballs, hats) and 0 to stream all in one single port\n"
            << "name               - Prefix name of the ports opened by the JoypadControlServer, e.g. /robotName/joypad\n"
            << "subdevice          - name of the subdevice to open\n"
            << "profile            - print the joypad data for debugging purpose";
        return false;
    }
    std::string rootName;
    if (!params.check("period", "refresh period of the broadcasted values in ms"))
    {
        yCInfo(JOYPADCONTROLSERVER) << "Using default 'period' parameter of" << DEFAULT_THREAD_PERIOD << "s";
    }
    else
    {
        m_period = params.find("period").asInt32() / 1000.0;
    }

    m_profile = params.check("profile");

    if(params.check("use_separate_ports"))
    {
        m_separatePorts = params.find("use_separate_ports").asBool();
        if(!m_separatePorts)
        {
            yCError(JOYPADCONTROLSERVER) << "Single port mode not supported at the moment";
            return false;
        }
    }
    else
    {
        yCError(JOYPADCONTROLSERVER) << "Missing use_separate_ports in configuration";
        return false;
    }
    m_coordsMode = yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN;
    rootName = params.check("name",Value("/"), "starting '/' if needed.").asString();

    if (!params.check("name", "Prefix name of the ports opened by the JoypadControlServer."))
    {
        yCError(JOYPADCONTROLSERVER) << "Missing 'name' parameter. Check you configuration file; it must be like:";
        yCError(JOYPADCONTROLSERVER) << "   name:         Prefix name of the ports opened by the JoypadControlServer, e.g. /robotName/joypad";
        return false;
    }

    rootName             = params.find("name").asString();
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
            yCError(JOYPADCONTROLSERVER) << "Error while opening subdevice";
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

    p.fromString(prop.toString());
    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    m_subDeviceOwned->open(p);

    if (!m_subDeviceOwned->isValid())
    {
        yCError(JOYPADCONTROLSERVER) << "Opening subdevice... FAILED";
        return false;
    }
    m_isSubdeviceOwned = true;
    if (!attach(m_subDeviceOwned)) {
        return false;
    }

    if(!m_parser.configure(m_device) )
    {
        yCError(JOYPADCONTROLSERVER) << "Error configuring interfaces for parsers";
        return false;
    }

    openPorts();
    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool JoypadControlServer::attach(PolyDriver* poly)
{
    if (poly) {
        poly->view(m_device);
    }

    if(m_device == nullptr)
    {
        yCError(JOYPADCONTROLSERVER) << "Attached device has no valid IJoypadController interface.";
        return false;
    }
    return true;
}

bool JoypadControlServer::attach(yarp::dev::IJoypadController *s)
{
    if(s == nullptr)
    {
        yCError(JOYPADCONTROLSERVER) << "Attached device has no valid IJoystickController interface.";
        return false;
    }
    m_device = s;
    return true;
}

bool JoypadControlServer::detach()
{
    m_device = nullptr;
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
        yCError(JOYPADCONTROLSERVER) << "Unable to open rpc Port" << m_rpcPortName.c_str();
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
        using countGet = bool (IJoypadController::*)(unsigned int&);

        struct solver
        {
            countGet                     getter;
            JoypadControl::LoopablePort* port;

            solver(countGet a, JoypadControl::LoopablePort* b) : getter(a), port(b)
            {}
        };

        vector<solver> getters;

        getters.emplace_back(&IJoypadController::getAxisCount,         &m_portAxis     );
        getters.emplace_back(&IJoypadController::getButtonCount,       &m_portButtons  );
        getters.emplace_back(&IJoypadController::getStickCount,        &m_portStick    );
        getters.emplace_back(&IJoypadController::getTouchSurfaceCount, &m_portTouch    );
        getters.emplace_back(&IJoypadController::getTrackballCount,    &m_portTrackball);
        getters.emplace_back(&IJoypadController::getHatCount,          &m_portHats     );

        for(auto& getter : getters)
        {
            if((m_device->*(getter.getter))(getter.port->count))
            {
                if(getter.port->count == 0)
                {
                    getter.port->valid = false;
                }
                else
                {
                    getter.port->contactable->open(getter.port->name);
                    getter.port->valid = true;
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
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "Hats: ";
    m_device->getHatCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        unsigned char data;
        m_device->getHat(i, data);
        message += to_string(data) + " ";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "Buttons: ";
    m_device->getButtonCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        float data;
        m_device->getButton(i, data);
        message += to_string(data) + " ";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;

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
    yCInfo(JOYPADCONTROLSERVER) << message;

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
    yCInfo(JOYPADCONTROLSERVER) << message;
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
            if (write) {
                m_portButtons.write();
            }
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
            if (write) {
                m_portHats.write();
            }
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
                    yCError(JOYPADCONTROLSERVER) << "Cannot get axis with id" << i;
                    write = false;
                    break;
                }
                b.push_back(v);
            }
            if (write) {
                m_portAxis.write();
            }
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
                    yCError(JOYPADCONTROLSERVER) << "Cannot get axis with id" << i;
                    write = false;
                    break;
                }
                cat(b, v);
            }
            if (write) {
                m_portTrackball.write();
            }
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
            if (write) {
                m_portStick.write();
            }
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

            if (write) {
                m_portTouch.write();
            }
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
        yCError(JOYPADCONTROLSERVER) << "Cannot attach more than one device";
        return false;
    }

    yarp::dev::PolyDriver* Idevice2attach = p[0]->poly;
    if(p[0]->key == "IJoypadController")
    {
        yCInfo(JOYPADCONTROLSERVER) << "Good name!";
    }
    else
    {
        yCInfo(JOYPADCONTROLSERVER) << "Bad name!";
    }

    if (!Idevice2attach->isValid())
    {
        yCError(JOYPADCONTROLSERVER) << "Device " << p[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(m_device);
    if (!attach(m_device)) {
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    if (!PeriodicThread::start()) {
        return false;
    }

    openPorts();
    return true;
}

bool JoypadControlServer::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    //check if we already instantiated a subdevice previously
    if (m_isSubdeviceOwned) {
        return false;
    }

    m_device = nullptr;
    return true;
}

bool JoypadControlServer::close()
{
    detachAll();

    // close subdevice if it was created inside the open (--subdevice option)
    if(m_isSubdeviceOwned)
    {
        if (m_subDeviceOwned) {
            m_subDeviceOwned->close();
        }

        m_subDeviceOwned   = nullptr;
        m_device           = nullptr;
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
