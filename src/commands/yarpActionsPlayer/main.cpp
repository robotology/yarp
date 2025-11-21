/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include "yarpActionsPlayer_IDL.h"

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperMultiple.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <map>
#include <mutex>

#include "robotDriver.h"
#include "robotAction.h"
#include "broadcastingThread.h"
#include "controlThread.h"

// ******************** THE MODULE
class scriptModule: public yarp::os::RFModule, public yarpActionsPlayer_IDL
{
protected:
    yarp::os::Port      m_rpcPort;
    std::string         m_name;
    bool                m_verbose;
    std::map<std::string,yarp::dev::PolyDriver*>  m_robotClients;
    std::map<std::string,yarp::dev::PolyDriver*>  m_robotRemappers;
    std::map<std::string,robotDriver*>  m_robotControllers;
    std::map<std::string,action_class>  m_actions;
    ControlThread*      m_wthread=nullptr;
    BroadcastingThread* m_bthread=nullptr;

    std::string         m_current_action_id;

    public: //yarpActionsPlayer_IDL methods
    bool start() override;
    bool stop() override;
    bool reset() override;
    bool forever() override;
    bool print_frames() override;
    bool speed_factor(const double value) override;
    bool resample(const double value) override;
    bool choose_action(const std::string& action_name) override;
    bool play_action(const std::string& action_name) override;
    bool show_actions() override;
    bool set_thread_period(const double value) override;
    bool set_initial_move_time(const double value) override;

    std::string vectorToString(const std::vector<double>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i + 1 != vec.size()) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    public:
    scriptModule()
    {
        m_verbose=true;
    }

    ~scriptModule()
    {
        // stop the threads
        if (m_wthread)
        {
            m_wthread->stop();
            yarp::os::Time::delay(0.100);
            delete m_wthread;
            m_wthread=nullptr;
        }

        if (m_bthread)
        {
            m_bthread->stop();
            yarp::os::Time::delay(0.100);
            delete m_bthread;
            m_bthread=nullptr;
        }

        //clear the controllers
        for (auto it = m_robotControllers.begin(); it != m_robotControllers.end(); it++)
        {
            if (it->second)
            {
                delete it->second;
                it->second = nullptr;
            }
        }
        m_robotControllers.clear();

        //clear the remappers
        for (auto it = m_robotRemappers.begin(); it != m_robotRemappers.end(); it++)
        {
            if (it->second)
            {
                delete it->second;
                it->second = nullptr;
            }
        }
        m_robotRemappers.clear();

        //clear the clients
        for (auto it = m_robotClients.begin(); it != m_robotClients.end(); it++)
        {
            if (it->second)
            {
                delete it->second;
                it->second = nullptr;
            }
        }
        m_robotClients.clear();

        //clear the storages
        m_actions.clear();

        yInfo() << "cleanup complete";
    }

    bool chooseActionByName(std::string id)
    {
        if (m_actions.find(id) == m_actions.end())
        {
            yError() << "action id not found";
            return false;
        }
        m_current_action_id = id;

        action_class *action = &m_actions[id];
        std::string controller_name = action->controller_name;
        robotDriver *driver = m_robotControllers[controller_name];

        if (!driver || !action)
        {
            yError() << "invalid driver/action pointer";
            return false;
        }
        yInfo() << "action selected:" << id;
        yDebug() << "action controller:" << controller_name;
        yDebug() << "number of action frames:" << action->action_frames_vector.size();
        return m_wthread->action_change(action, driver);
    }

    std::string string_list_actions()
    {
        std::string ss = "actions:\n";
        size_t i = 0;
        for (auto it=m_actions.begin(); it!=m_actions.end();it++)
        {
            ss = ss + "(" + std::to_string(i++) + ") " + it->second.action_name + "\n";
        }
        return ss;
    }

    bool loadConfiguration(std::string filename, double resample_period)
    {
        yarp::os::Property p;
        if (!p.fromConfigFile(filename))
        {
            yError() << "Unable to read configuration file!";
            return false;
        }

        //load the clients
        yarp::os::Bottle& bot_clients = p.findGroup("CLIENTS");
        if (bot_clients.size() == 0)
        {
            yError() << "Unable to read CLIENTS section";
            return false;
        }
        size_t num_of_clients = bot_clients.size();
        for (size_t i = 1; i < num_of_clients; i++)
        {
            yDebug() << bot_clients.get(i).toString();
            yarp::os::Bottle* bot_client_elem = bot_clients.get(i).asList();
            if (bot_client_elem == nullptr)
            {
                yError() << "Invalid entry in CLIENTS section";
                return false;
            }
            size_t num_of_client_elems = bot_client_elem->size();
            if (num_of_client_elems == 2)
            {
                //parse a line of the CLIENTS section
                std::string client_name = bot_client_elem->get(0).toString();
                std::string remoteControlBoards = bot_client_elem->get(1).toString();
                if (m_robotClients[client_name] != nullptr)
                {
                    yError() << "Duplicate client entry: " << client_name;
                    return false;
                }
                m_robotClients[client_name] = new yarp::dev::PolyDriver;
                yarp::os::Property clientoptions;
                clientoptions.put("device", "remote_controlboard");
                clientoptions.put("remote", remoteControlBoards);
                clientoptions.put("local", m_name + "/client/" + client_name);
                clientoptions.put("carrier", "fast_tcp");

                //configure the client
                bool rob_ok = m_robotClients[client_name]->open(clientoptions);
                if (!rob_ok)
                {
                    yError() << "Unable to initialize client: " << client_name;
                    return false;
                }
            }
            else
            {
                yError() << "Invalid number of entries in CLIENTS section";
                return false;
            }
        }

        //load the remappers
        yarp::os::Bottle& bot_cont = p.findGroup("REMAPPERS");
        if (bot_cont.size() == 0)
        {
            yError() << "Unable to read REMAPPERS section";
            return false;
        }
        size_t num_of_controllers = bot_cont.size();
        for (size_t i = 1; i < num_of_controllers; i++)
        {
            yDebug() << bot_cont.get(i).toString();
            yarp::os::Bottle* bot_cont_elem = bot_cont.get(i).asList();
            if (bot_cont_elem == nullptr)
            {
                yError() << "Invalid entry in REMAPPERS section";
                return false;
            }
            size_t num_of_celems = bot_cont_elem->size();
            if (num_of_celems == 3)
            {
                //parse a line of the remappers section
                std::string remapper_name = bot_cont_elem->get(0).toString();
                yarp::os::Bottle* clients_name = bot_cont_elem->get(1).asList();
                std::string axes_names = bot_cont_elem->get(2).toString();
                if (clients_name==nullptr || clients_name->size() == 0)
                {
                    yError() << "error in clients_name";
                    return false;
                }
                if (axes_names.empty())
                {
                    yError() << "error in axes_names";
                    return false;
                }

                //configure the remapper
                if (m_robotRemappers[remapper_name] != nullptr)
                {
                    yError() << "Duplicate remapper entry: " << remapper_name;
                    return false;
                }
                m_robotRemappers[remapper_name] = new yarp::dev::PolyDriver;
                yarp::os::Property rmoptions;
                rmoptions.put("device", "controlboardremapper");
                yarp::os::Value* jlist = yarp::os::Value::makeList(axes_names.c_str());
                rmoptions.put("axesNames",jlist);
                bool rob_ok = m_robotRemappers[remapper_name]->open(rmoptions);
                if (!rob_ok)
                {
                    yError() << "Unable to initialize remapper: " << remapper_name;
                    return false;
                }

                //attach the remapper to one or more clients
                {
                    yarp::dev::IMultipleWrapper* ww_rem=nullptr;
                    m_robotRemappers[remapper_name]->view(ww_rem);
                    yarp::dev::PolyDriverList pdlist;
                    for (size_t i=0; i<clients_name->size(); i++)
                    {
                        std::string cnn = clients_name->get(i).toString();
                        yarp::dev::PolyDriver* ddnwc = m_robotClients[cnn];
                        if (ddnwc == nullptr)
                        {
                            yError() << "Configuration error while searching for client" << cnn;
                            return false;
                        }
                        std::string boardname = "nwcboard" + std::to_string(i);
                        pdlist.push(ddnwc, boardname.c_str());
                    }
                    bool result_att = ww_rem->attachAll(pdlist);
                    if (!result_att)
                    {
                        yError() << "Configuration error while trying to attach the remapper: "
                                 << remapper_name << " with the control_boards: " << clients_name->toString();
                        return false;
                    }
                }

                //put the controller in the list
                if (m_robotControllers[remapper_name] != nullptr)
                {
                    yError() << "Duplicate controller entry: " << remapper_name;
                    return false;
                }
                m_robotControllers[remapper_name] = new robotDriver;
                rob_ok = m_robotControllers[remapper_name]->configure(m_robotRemappers[remapper_name]);
                rob_ok &= m_robotControllers[remapper_name]->init();
                if (!rob_ok)
                {
                    yError() << "Unable to initialize controller: " << remapper_name;
                    return false;
                }
            }
            else
            {
                yError() << "Invalid number of entries in REMAPPERS section";
                return false;
            }
        }

        yarp::os::Bottle& bot_action = p.findGroup("ACTIONS");
        if (bot_action.size() == 0)
        {
            yError() << "Unable to read ACTIONS section";
            return false;
        }
        for (size_t i = 1; i < bot_action.size(); i++)
        {
            std::string str = bot_action.toString();
            yDebug() << bot_action.get(i).toString();
            yarp::os::Bottle* bot_act_elem = bot_action.get(i).asList();
            if (bot_act_elem == nullptr)
            {
                yError() << "Invalid entry in ACTIONS section";
                return false;
            }
            size_t num_of_aelems = bot_act_elem->size();
            if (num_of_aelems==3 || num_of_aelems == 4)
            {
                //parse a line of the ACTIONS section
                std::string action_name = bot_act_elem->get(0).toString();
                std::string controller_name = bot_act_elem->get(1).toString();
                std::string action_file_name = bot_act_elem->get(2).toString();
                yarp::os::Bottle* tolerances_bot = bot_act_elem->get(3).asList();

                //check if the controller name exists
                if (m_robotControllers.find(controller_name) == m_robotControllers.end())
                {
                    yError() << controller_name << "in action" << action_name << "does not exists";
                    return false;
                }

                //load the action file
                action_class tmpAction;
                tmpAction.action_name = action_name;
                tmpAction.controller_name = controller_name;
                size_t njoints = m_robotControllers[controller_name]->getNJoints();

                if (!tmpAction.openFile(action_file_name, njoints, 0.010))
                {
                    yError() << "Unable to parse file";
                    return false;
                }
                if (resample_period!=0.0)
                {
                    tmpAction.interpolate_action_frames(resample_period);
                }

                tmpAction.m_tolerances.resize(njoints);
                for (size_t i=0; i<njoints; i++)
                {
                    tmpAction.m_tolerances[i] = 3.0; //deg
                }
                if (tolerances_bot)
                {
                    if (tolerances_bot->size() != njoints)
                    {
                        yError() << "Invalid size of tolerances vector, should be"<<njoints << "got " << tolerances_bot->size();
                        return false;
                    }
                    for (size_t i=0; i<njoints; i++)
                    {
                        tmpAction.m_tolerances[i] = tolerances_bot->get(i).asFloat64();
                    }
                }
                yDebug() << "Tolerances for action:" << action_name << vectorToString(tmpAction.m_tolerances);

                //put the action in the list
                m_actions[action_name] = tmpAction;
            }
            else
            {
                yError() << "Invalid entry in ACTIONS section, size should be 3 or 4, got " <<num_of_aelems;
                return false;
            }
        }

        yInfo() << "configuration file successfully loaded";
        return true;
    }

    void print_help()
    {
        yInfo();
        yInfo() << "Command line:";
        yInfo() << "yarpActionsPlayer --filename `name` [--name `module_name`] [--execute] [--period period_s] [--resample resample_period_s] [--pos_tolerance pos] [--pos_timeout pos] [--pos_strict_check enable] ";
        yInfo();
        yInfo() << "`name` : file containing the actions";
        yInfo() << "`module_name` : prefix of the ports opened by the module (default: /yarpActionsPlayer)";
        yInfo() << "`verbose` : if enabled, extra debug messages are shown.";
        yInfo() << "`execute` : if enabled, the yarpActionsPlayer will send commands to the robot. Otherwise, only it will operate in simulation mode only";
        yInfo() << "`pos_tolerance` : the tolerance (in degrees) that will be checked by the initial movement in position mode, before switching to positionDirect mode. Default: 2degrees";
        yInfo() << "`pos_timeout` : the amount of time (in seconds) the robot will attempt to reach the target position within the specified position tolerance. Default: 2s";
        yInfo() << "`pos_strict_check` : if set to true, the system will halt if home position is halted, otherwise it will continue after the timeout expires. Default: false";
        yInfo() << "`period` : the period (in s) of the thread processing the commands. Default 0.010s";
        yInfo() << "`resample`: all the loaded trajectory files are internally resampled at the specified period. Default: not enabled";
        yInfo() << "`initial_move_time`: the duration (in seconds) of the initial homing position of the joints before starting the trajectory. Default: 4s";
        yInfo();
    }

    virtual bool configure(yarp::os::ResourceFinder& rf)
    {
        std::string test_string = rf.toString();

        // generic configuration
        if (rf.check("name"))
            m_name = std::string("/") + rf.find("name").asString().c_str();
        else
            m_name = "/yarpActionsPlayer";

        // rpc port
        m_rpcPort.open((m_name + "/rpc").c_str());
        this->yarp().attachAsServer(m_rpcPort);

        // get the configuration for parameter period
        double period = 0.005;
        if (rf.check("period") == true)
        {
            period = rf.find("period").asFloat64();
        }

        // Instantiate the thread
        m_wthread = new ControlThread(m_name,period);

        //set the configuration for parameter execute
        if (rf.check("execute")==true)
        {
            yInfo() << "Enabling iPid->setReference() controller";
            m_wthread->m_enable_execute_joint_command = true;
        }
        else
        {
            yInfo() << "Not using iPid->setReference() controller";
            m_wthread->m_enable_execute_joint_command = false;
        }

        //set the configuration for parameter verbose
        if (rf.check("verbose")==true)
        {
            yInfo() << "yarpActionPlayer running in verbose mode";
            m_wthread->m_verbose = true;
        }
        else
        {
            yInfo() << "yarpActionPlayer running in non-verbose mode";
            m_wthread->m_verbose = false;
        }

        //set the position tolerance
        if (rf.check("help") == true)
        {
            print_help();
            return false;
        }

        if (rf.check("initial_move_time") == true)
        {
            double imt = rf.find("initial_move_time").asFloat64();
            m_wthread->setInitialMoveTime(imt);
        }

        //set the position tolerance
        if (rf.check("pos_tolerance") == true)
        {
            double tol = rf.find("tolerance").asFloat64();
            yInfo() << "Position tolerance set to " << tol << "degrees";
            m_wthread->setPositionTolerance(tol);
        }

        //set the position timeout
        if (rf.check("pos_timeout") == true)
        {
            double timeout = rf.find("pos_timeout").asFloat64();
            yInfo() << "Position timeout set to " << timeout << "seconds";
            m_wthread->setPositionTimeout(timeout);
        }

        //set the position timeout
        if (rf.check("pos_strict_check") == true)
        {
            bool enable = rf.find("pos_strict_check").asBool();
            m_wthread->setPositionStrictCheck(enable);
        }

        double resample_period = 0;
        if (rf.check("resample") == true)
        {
            resample_period = rf.find("resample_period").asFloat64();
            yInfo() << "Set resample period equal to:" << resample_period << "s";
        }

        //open the configuration file
        if (rf.check("filename")==true)
        {
            if (rf.find("filename").isString())
            {
                std::string filename = rf.find("filename").asString();
                bool b = loadConfiguration(filename, resample_period);
                if (!b)
                {
                    yError() << "Configuration error!";
                    return false;
                }
            }
            else
            {
                yError() << "`filename` option syntax error.";
                print_help();
                return false;
            }
        }
        else
        {
            yWarning() << "`filename` option not found. No sequence files loaded.";
        }

        //check if actions are valid
        if (m_actions.empty())
        {
            yInfo() << "There are no actions!";
            return false;
        }

        //select the first action
        yInfo() << "automatically selecting the first action";
        std::string first_action_name;
        first_action_name = this->m_actions.begin()->first;
        this->chooseActionByName(first_action_name);

        //start the thread
        if (!m_wthread->start())
        {
            yError() << "Working thread did not start, queue will not work";
        }
        else
        {
            yInfo() << "Working thread started";
        }

        yInfo() << "module successfully configured. ready.";
        return true;
    }

    virtual bool close()
    {
        m_rpcPort.close();
        return true;
    }

    virtual double getPeriod()    { return 1.0;  }
    virtual bool   updateModule() { return true; }
};

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("yarpActionsPlayer");
    rf.configure(argc,argv);

    yarp::os::Network yarp;

    if (!yarp.checkNetwork())
    {
        yError() << "yarp.checkNetwork() failed.";
        return -1;
    }

    scriptModule mod;

    return mod.runModule(rf);
}

bool scriptModule::start()
{
    return this->m_wthread->action_start();
}

bool scriptModule::stop()
{
    return this->m_wthread->action_stop();
}

bool scriptModule::reset()
{
    return this->m_wthread->action_reset();
}

bool scriptModule::forever()
{
    return this->m_wthread->action_forever();
}

bool scriptModule::print_frames()
{
    return this->m_wthread->action_print();
}

bool scriptModule::speed_factor(const double value)
{
    return this->m_wthread->action_setSpeedFactor(value);
}

bool scriptModule::resample(const double value)
{
    return this->m_wthread->action_resample(value);
}

bool scriptModule::choose_action(const std::string& action_name)
{
    return this->chooseActionByName(action_name);
}

bool scriptModule::play_action(const std::string& action_name)
{
    bool b = this->chooseActionByName(action_name);
    if (b)
    {
        bool b1 = this->m_wthread->action_start();
    }
    do { yarp::os::Time::delay(0.010); }
    while (this->m_wthread->getStatus() != action_status_enum::ACTION_IDLE);
    return true;
}

bool scriptModule::show_actions()
{
    std::string actions_str = this->string_list_actions();
    std::string current_action_name;
    bool b = m_wthread->action_getname(current_action_name);
    yInfo() << "current_action: " <<current_action_name;
    yInfo() << actions_str;
    return true;
}

bool scriptModule::set_thread_period(const double value)
{
    if (value > 0)
    {
        m_wthread->setPeriod(value);
        yError("invalid period value");
    }
    else
    {
        yInfo("Period set to %f", value);
    }
    return true;
}

bool scriptModule::set_initial_move_time(const double value)
{
    if (value > 0)
    {
        m_wthread->setInitialMoveTime(value);
        yError("invalid initial move time");
    }
    else
    {
        yInfo("Initial move time set to %f", value);
    }
    return true;
}
