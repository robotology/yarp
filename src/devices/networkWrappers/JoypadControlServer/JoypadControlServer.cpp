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

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(JOYPADCONTROLSERVER, "yarp.device.JoypadControlServer")
}


JoypadCtrlParser::JoypadCtrlParser() : m_device(nullptr)
{
    m_countGetters.insert(std::make_pair(VOCAB_BUTTON, &IJoypadController::getButtonCount));
    m_countGetters.insert(std::make_pair(VOCAB_HAT, &IJoypadController::getHatCount));
    m_countGetters.insert(std::make_pair(VOCAB_TRACKBALL, &IJoypadController::getTrackballCount));
    m_countGetters.insert(std::make_pair(VOCAB_AXIS, &IJoypadController::getAxisCount));
    m_countGetters.insert(std::make_pair(VOCAB_STICK, &IJoypadController::getStickCount));
    m_countGetters.insert(std::make_pair(VOCAB_TOUCH, &IJoypadController::getTouchSurfaceCount));
}

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
        m_device = interface;
        ret = true;
    }
    else
    {
        m_device = nullptr;
        ret = false;
    }

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
            if(m_countGetters.find(toGet) != m_countGetters.end())
            {
                unsigned int   count;
                getcountmethod getter;
                getter = m_countGetters[toGet];
                if((m_device->*getter)(count))
                {
                    response.addVocab32(VOCAB_OK);
                    response.addInt32(count);
                    ret = true;
                }
            }
            else if (toGet == VOCAB_STICKDOF && cmd.get(4).isInt32())
            {
                unsigned int count;
                if (m_device->getStickDoF(cmd.get(4).asInt32(), count))
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
                if(cmd.get(4).isInt32() && m_device->getButton(cmd.get(4).asInt32(), value))
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
                if(cmd.get(4).isInt32() && m_device->getAxis(cmd.get(4).asInt32(), value))
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
                    if(cmd.get(5).isInt32() && m_device->getStick(cmd.get(5).asInt32(), frame, mode))
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

                if(cmd.get(5).isInt32() && m_device->getStickDoF(cmd.get(5).asInt32(), dofCount))
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
                if(cmd.get(4).isInt32() && m_device->getTouch(id, pos))
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
                if(cmd.get(4).isInt32() && m_device->getTrackball(id, axes))
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
                if(cmd.get(4).isInt32() && m_device->getHat(cmd.get(4).asInt32(), value))
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


JoypadControlServer::JoypadControlServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

JoypadControlServer::~JoypadControlServer()
{
}

bool JoypadControlServer::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    if(m_use_separate_ports == false)
    {
        yCError(JOYPADCONTROLSERVER) << "Single port mode not supported at the moment";
        return false;
    }

    m_rpcPortName        = m_name + "/rpc:i";
    m_portButtons.name   = m_name + "/buttons:o";
    m_portAxis.name      = m_name + "/axis:o";
    m_portStick.name     = m_name + "/stick:o";
    m_portTouch.name     = m_name + "/touch:o";
    m_portTrackball.name = m_name + "/trackball:o";
    m_portHats.name      = m_name + "/hat:o";

    yCInfo(JOYPADCONTROLSERVER) << "Running, waiting for attach...";
    return true;
}

bool JoypadControlServer::attach(PolyDriver* poly)
{
    if (poly) {
        poly->view(m_IJoypad);
    }

    if(m_IJoypad == nullptr)
    {
        yCError(JOYPADCONTROLSERVER) << "Attached device has no valid IJoypadController interface.";
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    if (!PeriodicThread::start()) {
        return false;
    }

    openPorts();
    if (!m_parser.configure(m_IJoypad))
    {
        yCError(JOYPADCONTROLSERVER) << "Error configuring interfaces for parsers";
        return false;
    }

    return true;
}

bool JoypadControlServer::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    m_IJoypad = nullptr;
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
    if(!m_IJoypad)
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
    if(m_use_separate_ports)
    {
        using countGet = bool (IJoypadController::*)(unsigned int&);

        struct solver
        {
            countGet                     getter;
            JoypadControl::LoopablePort* port;

            solver(countGet a, JoypadControl::LoopablePort* b) : getter(a), port(b)
            {}
        };

        std::vector<solver> getters;

        getters.emplace_back(&IJoypadController::getAxisCount,         &m_portAxis     );
        getters.emplace_back(&IJoypadController::getButtonCount,       &m_portButtons  );
        getters.emplace_back(&IJoypadController::getStickCount,        &m_portStick    );
        getters.emplace_back(&IJoypadController::getTouchSurfaceCount, &m_portTouch    );
        getters.emplace_back(&IJoypadController::getTrackballCount,    &m_portTrackball);
        getters.emplace_back(&IJoypadController::getHatCount,          &m_portHats     );

        for(auto& getter : getters)
        {
            if((m_IJoypad->*(getter.getter))(getter.port->count))
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
    std::string       message;
    unsigned int count;

    message = "Axes: ";
    m_IJoypad->getAxisCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        double data;
        m_IJoypad->getAxis(i, data);
        message += std::to_string(data) + " ";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "Hats: ";
    m_IJoypad->getHatCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        unsigned char data;
        m_IJoypad->getHat(i, data);
        message += std::to_string(data) + " ";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "Buttons: ";
    m_IJoypad->getButtonCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        float data;
        m_IJoypad->getButton(i, data);
        message += std::to_string(data) + " ";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "Stick: ";
    m_IJoypad->getStickCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_IJoypad->getStick(i, data, yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN);
        message += "n_" + std::to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += std::to_string(data[j]) + " ";
        }
        message += "\n";

    }
    yCInfo(JOYPADCONTROLSERVER) << message;

    message = "trackball: ";
    m_IJoypad->getTrackballCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_IJoypad->getTrackball(i, data);
        message += "n_" + std::to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += std::to_string(data[j]) + " ";
        }
        message += "\n";
    }

    message = "touch Surface: ";
    m_IJoypad->getTouchSurfaceCount(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        Vector data;
        m_IJoypad->getTouch(i, data);
        message += "n_" + std::to_string(i) + ": ";
        for (size_t j = 0; j < data.size(); ++j)
        {
            message += std::to_string(data[j]) + " ";
        }
        message += "\n";
    }
    yCInfo(JOYPADCONTROLSERVER) << message;
}

void JoypadControlServer::run()
{
    if(m_use_separate_ports)
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
                if(!m_IJoypad->getButton(i, v))
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
                if(!m_IJoypad->getHat(i, v))
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
                if(!m_IJoypad->getAxis(i, v))
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
                if(!m_IJoypad->getTrackball(i, v))
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
                if(!m_IJoypad->getStick(i, v, m_coordsMode) || !m_IJoypad->getStickDoF(i, dofCount) || v.size() != dofCount)
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
                if(!m_IJoypad->getTouch(i, v))
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

bool JoypadControlServer::close()
{
    detachAll();

    // Closing port
    std::vector<JoypadControl::LoopablePort*> portv;
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
