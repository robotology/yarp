/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/yarpbroker.h>

#include <csignal>
#include <cstring>

#define YARPRUN_OK                  0
#define YARPRUN_NORESPONSE          1
#define YARPRUN_NOCONNECTION        2
#define YARPRUN_CONNECTION_TIMOUT   3
#define YARPRUN_SEMAPHORE_PARAM     4
#define YARPRUN_UNDEF               5

#define CONNECTION_TIMEOUT      2.0         //seconds
#define RUN_TIMEOUT             10.0        //seconds
#define STOP_TIMEOUT            15.0
#define KILL_TIMEOUT            10.0
#define EVENT_THREAD_PERIOD     0.5 //seconds

#if defined(_WIN32)
    #define SIGKILL 9
#endif

const char* yarprun_err_msg[] = { " (Ok) ",
                                  " (Remote host does not respond) ",
                                  " (Remote host does no exist) ",
                                  " (Timeout while connecting to the remote host) ",
                                  " (Blocked in broker semaphor) ",
                                  " (Undefined message) " };

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;
using namespace yarp::manager;


YarpBroker::YarpBroker() : PeriodicThread(EVENT_THREAD_PERIOD)
{
    bOnlyConnector = bInitialized = false;
    ID = generateID();
    strStdioUUID.clear();
}


YarpBroker::~YarpBroker()
{
    fini();
}

void YarpBroker::fini()
{
    if(PeriodicThread::isRunning())
        PeriodicThread::stop();
    //port.close();
}

bool YarpBroker::init()
{
    //if(bInitialized)
    //  return true;

    if(!NetworkBase::checkNetwork(CONNECTION_TIMEOUT))
    {
        strError = "YARP network server is not up.";
        return false;
    }
    bInitialized = true;
    bOnlyConnector = true;

    /*
    semParam.wait();
    __trace_message = "(init) opening port ...";
    port.setTimeout(CONNECTION_TIMEOUT);
    port.open("...");
    __trace_message.clear();
    semParam.post();
    */
    return true;
}

bool YarpBroker::init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv )
{
    //if(bInitialized)
    //  return true;

    semParam.wait();

    strCmd.clear();
    strParam.clear();
    strHost.clear();
    strStdio.clear();
    strWorkdir.clear();
    strTag.clear();
    strEnv.clear();

    if(!szcmd)
    {
        strError = "command is not specified.";
        semParam.post();
        return false;
    }

    if(!szhost)
    {
        strError = "remote host port is not specified.";
        semParam.post();
        return false;
    }

    if(szhost[0] != '/')
        strHost = string("/") + string(szhost);
    else
        strHost = szhost;

    strCmd = szcmd;
    if(strlen(szparam))
        strParam = szparam;
    if(strlen(szworkdir))
        strWorkdir = szworkdir;

    if(strlen(szstdio))
    {
        if(szstdio[0] != '/')
            strStdio = string("/") + string(szstdio);
        else
            strStdio = szstdio;
    }

    if(strlen(szenv))
        strEnv = szenv;

    OSTRINGSTREAM sstrID;
    sstrID<<(int)ID;
    strTag = strHost + strCmd + strParam + strEnv + sstrID.str();
    string::iterator itr;
    for(itr=strTag.begin(); itr!=strTag.end(); itr++)
        if(((*itr) == ' ') || ((*itr) == '/') )
            (*itr) = ':';

   __trace_message = "(init) checking yarp network";
    if(!NetworkBase::checkNetwork(5.0))
    {
        strError = "YARP network server is not up.";
        __trace_message.clear();
        semParam.post();
        return false;
    }
    __trace_message = string("(init) checking existence of ") + strHost;
    if(!exists(strHost.c_str()))
    {
        strError = szhost;
        strError += " does not exist. check yarprun is running as server.";
        __trace_message.clear();
        semParam.post();
        return false;
    }

    /*
    port.setTimeout(CONNECTION_TIMEOUT);
    __trace_message = "(init) opening port ...";
    port.open("...");
    __trace_message.clear();
    */

    bInitialized = true;
    semParam.post();

    return true;
}


bool YarpBroker::start()
{
    if(!bInitialized) return false;
    if(bOnlyConnector) return false;

    strError.clear();
    int ret = requestServer(runProperty());
    if(ret != YARPRUN_OK)
    {
        strError = "cannot ask ";
        strError += strHost;
        strError += " to run ";
        strError += strCmd;
        strError += yarprun_err_msg[ret];
        if(ret == YARPRUN_SEMAPHORE_PARAM)
            strError += string(" due to " + __trace_message);
        return false;
    }

    double base = SystemClock::nowSystem();
    while(!timeout(base, RUN_TIMEOUT))
    {
        if(running() == 1)
        {
            if(strStdioUUID.size())
            {
                if(PeriodicThread::isRunning())
                    PeriodicThread::stop();
                PeriodicThread::start();
            }
            return true;
        }
    }

    strError = "cannot run ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    return false;
}

bool YarpBroker::stop()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;

    strError.clear();
    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(strHost.c_str());
    msg.addList()=grp;
    grp.clear();
    grp.addString("sigterm");
    grp.addString(strTag.c_str());
    msg.addList()=grp;
    int ret = SendMsg(msg, strHost, response, CONNECTION_TIMEOUT);
    if(ret != YARPRUN_OK)
    {
        strError = "cannot ask ";
        strError += strHost;
        strError += " to stop ";
        strError += strCmd;
        strError += yarprun_err_msg[ret];
        if(ret == YARPRUN_SEMAPHORE_PARAM)
            strError += string(" due to " + __trace_message);
        return false;
    }

    double base = SystemClock::nowSystem();
    while(!timeout(base, STOP_TIMEOUT))
    {
        if(running() == 0)
        {
            PeriodicThread::stop();
            return true;
        }
    }

    strError = "Timeout! Cannot stop ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    PeriodicThread::stop();
    return false;
}

bool YarpBroker::kill()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;

    strError.clear();

    yarp::os::Bottle msg,grp,response;
    grp.clear();
    grp.addString("on");
    grp.addString(strHost.c_str());
    msg.addList() = grp;
    grp.clear();
    grp.addString("kill");
    grp.addString(strTag.c_str());
    grp.addInt32(SIGKILL);
    msg.addList() = grp;
    int ret = SendMsg(msg, strHost, response, CONNECTION_TIMEOUT);
    if(ret != YARPRUN_OK)
    {
        strError = "cannot ask ";
        strError += strHost;
        strError += " to kill ";
        strError += strCmd;
        strError += yarprun_err_msg[ret];
        if(ret == YARPRUN_SEMAPHORE_PARAM)
            strError += string(" due to " + __trace_message);
        return false;
    }

    double base = SystemClock::nowSystem();
    while(!timeout(base, KILL_TIMEOUT))
    {
        if(running() == 0)
        {
            PeriodicThread::stop();
            return true;
        }
    }

    strError = "cannot kill ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    PeriodicThread::stop();
    return false;
}


int YarpBroker::running()
{
    if(!bInitialized) return -1;
    if(bOnlyConnector) return -1;

    strError.clear();
    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("on");
    grp.addString(strHost.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("isrunning");
    grp.addString(strTag.c_str());
    msg.addList()=grp;

    int ret = SendMsg(msg, strHost, response, 3.0);
    if(ret != YARPRUN_OK)
    {
        strError = "cannot ask ";
        strError += strHost;
        strError += " to check for status of ";
        strError += strCmd;
        strError += yarprun_err_msg[ret];
        if(ret == YARPRUN_SEMAPHORE_PARAM)
            strError += string(" due to " + __trace_message);
        return -1;
    }
    return ((response.get(0).asString() == "running")?1:0);
}


bool YarpBroker::attachStdout()
{
    return true;
}

void YarpBroker::detachStdout()
{
}


Property& YarpBroker::runProperty()
{
    command.clear();
    string cmd = strCmd + string(" ") + strParam;
    command.put("cmd", cmd);
    command.put("on", strHost);
    command.put("as", strTag);
    if(!strWorkdir.empty())
        command.put("workdir", strWorkdir);
    if(!strStdio.empty())
        command.put("stdio", strStdio);
    if(!strEnv.empty())
        command.put("env", strEnv);
    //command.put("hold", "hold");
    return command;
}


/**
 *  connection broker
 */
bool YarpBroker::connect(const char* from, const char* to,
            const char* carrier, bool persist)
{
    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    style.carrier = carrier;

    if(!persist)
    {
        /*
         * TODO: this check should be removed and
         *       the necessary modification should be done inside NetworkBase::isConnected!!!
         */
        string strCarrier = carrier;
        bool needDisconnect = strCarrier.find("udp") == (size_t)0;
        needDisconnect |= strCarrier.find("mcast") == (size_t)0;
        if(needDisconnect == false) {
            if(NetworkBase::isConnected(from, to, style))
                return true;
        }

        NetworkBase::connect(from, to, style);
        if(!connected(from, to, carrier))
        {
            strError = "cannot connect ";
            strError +=from;
            strError += " to " + string(to);
            return false;
        }
    }
    else
    {
        string topic = string("topic:/") + string(from) + string(to);
        NetworkBase::connect(from, topic, style);
        NetworkBase::connect(topic, to, style);
        if(!connected(from, to, carrier))
        {
            strError = "a persistent connection from ";
            strError +=from;
            strError += " to " + string(to);
            strError += " is created but not connected.";
            return false;
        }

    }

    return true;
}

bool YarpBroker::disconnect(const char* from, const char* to, const char* carrier)
{

    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    /*
    if(!exists(from))
    {
        strError = from;
        strError += " does not exist.";
        return true;
    }

    if(!exists(to))
    {
        strError = to;
        strError += " does not exist.";
        return true;
    }
    */

    if(!connected(from, to, carrier))
        return true;

    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    style.carrier = carrier;
    if(!NetworkBase::disconnect(from, to, style))
    {
        strError = "cannot disconnect ";
        strError +=from;
        strError += " from " + string(to);
        return false;
    }
    return true;

}

bool YarpBroker::exists(const char* szport)
{
    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    return NetworkBase::exists(szport, style);
}

const char* YarpBroker::requestRpc(const char* szport, const char* request, double timeout)
{
    if((szport==nullptr) || (request==nullptr))
        return nullptr;

    if(!exists(szport))
        return nullptr;

    // opening the port
    yarp::os::Port port;
    port.setTimeout((float)((timeout>0.0) ? timeout : CONNECTION_TIMEOUT));
    if(!port.open("..."))
        return nullptr;

    ContactStyle style;
    style.quiet = true;
    style.timeout = (timeout>0.0) ? timeout : CONNECTION_TIMEOUT;
    bool ret;
    for(int i=0; i<10; i++) {
        ret = NetworkBase::connect(port.getName(), szport, style);
        if(ret) break;
        SystemClock::delaySystem(1.0);
    }

    if(!ret) {
        port.close();
        return nullptr;
    }

    Bottle msg, response;
    msg.fromString(request);
    ret = port.write(msg, response);
    NetworkBase::disconnect(port.getName(), szport);
    if(!response.size() || !ret) {
        port.close();
        return nullptr;
    }

    port.close();
    return response.toString().c_str();
}

bool YarpBroker::connected(const char* from, const char* to, const char* carrier)
{
    if(!exists(from) || !exists(to))
        return false;
    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    style.carrier = carrier;
    return NetworkBase::isConnected(from, to, style);
}

bool YarpBroker::getSystemInfo(const char* server, SystemInfoSerializer& info)
{
    if(!strlen(server))
        return false;
    if(!semParam.check())
        return false;

    yarp::os::Port port;
    // opening the port
    port.setTimeout(CONNECTION_TIMEOUT);
    if(!port.open("...")) {
        __trace_message.clear();
        semParam.post();
        return false;
    }

    yarp::os::Bottle msg, grp;
    grp.clear();
    grp.addString("sysinfo");
    msg.addList() = grp;

    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    //style.carrier = carrier;


    __trace_message = "(getSystemInfo) connecting to " + string(port.getName());
    bool connected = yarp::os::NetworkBase::connect(port.getName(), server, style);
    if(!connected)
    {
        port.close();
        strError = string("Cannot connect to ") + string(server);
        __trace_message.clear();
        semParam.post();
        return false;
    }

    __trace_message = "(getSystemInfo) writing to " + string(port.getName());
    bool ret = port.write(msg, info);
    __trace_message = "(getSystemInfo) disconnecting from " + string(port.getName());
    NetworkBase::disconnect(port.getName(), server);

    if(!ret)
    {
        port.close();
        strError = string(server) + string(" does not respond");
        __trace_message.clear();
        semParam.post();
        return false;
    }

    port.close();
    __trace_message.clear();
    semParam.post();
    return true;
}

bool YarpBroker::getAllPorts(vector<string> &ports)
{
    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    Bottle cmd, reply;
    cmd.addString("list");

    bool ret = NetworkBase::writeToNameServer(cmd, reply, style);
    if (!ret)
    {
        strError = "Failed to reach name server\n";
        return false;
    }

    if((reply.size()!=1) || (!reply.get(0).isString()))
        return false;

    std::string str = reply.get(0).asString();
    const char* delm = "registration name ";
    size_t pos1, pos2;
    while((pos1 = str.find(delm)) != std::string::npos)
    {
        str = str.substr(pos1+strlen(delm));
        if((pos2 = str.find(' ')) != std::string::npos)
            ports.push_back(str.substr(0, pos2));
    }

    return true;
}

bool YarpBroker::getAllProcesses(const char* server,
                                 ProcessContainer& processes)
{
    if(!strlen(server))
        return false;

    processes.clear();
    strError.clear();
    yarp::os::Bottle msg,grp,response;

    grp.clear();
    grp.addString("ps");
    msg.addList()=grp;

    int ret = SendMsg(msg, server, response, 3.0);
    if((ret == YARPRUN_OK) || (ret == YARPRUN_NORESPONSE))
    {
        for(size_t i=0; i<response.size(); i++)
        {
            Process proc;
            std::string sprc;
            if(response.get(i).check("pid"))
                proc.pid = response.get(i).find("pid").asInt32();
            if(response.get(i).check("cmd"))
               sprc = response.get(i).find("cmd").asString();
            if(response.get(i).check("env") &&
               response.get(i).find("env").asString().length())
               sprc.append("; ").append(response.get(i).find("env").asString());
            proc.command = sprc;
            processes.push_back(proc);
        }
        return true;
    }

    strError = "cannot ask ";
    strError += server;
    strError += " to give the list of running processes.";
    strError += yarprun_err_msg[ret];
    if(ret == YARPRUN_SEMAPHORE_PARAM)
        strError += string(" due to " + __trace_message);
    return false;
}


bool YarpBroker::rmconnect(const char* from, const char* to)
{
    string topic = string(from) + string(to);
    Bottle cmd, reply;
    cmd.addString("untopic");
    cmd.addString(topic.c_str());
    return NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply,
                                 false,
                                 true,
                                 CONNECTION_TIMEOUT);
}

bool YarpBroker::setQos(const char* from, const char *to,
                        const char *qosFrom, const char *qosTo) {
    strError.clear();

    if(qosFrom && qosTo && !strlen(qosFrom) && !strlen(qosTo))
        return true;

    QosStyle styleFrom;
    QosStyle styleTo;
    if(qosFrom != nullptr && strlen(qosFrom)) {
        if(!getQosFromString(qosFrom, styleFrom)) {
            strError = "Error in parsing Qos properties of " + string(from);
            return false;
        }
    }
    if(qosTo != nullptr && strlen(qosTo))
        if(!getQosFromString(qosTo, styleTo)) {
            strError = "Error in parsing Qos properties of " + string(to);
            return false;
        }
    return NetworkBase::setConnectionQos(from, to, styleFrom, styleTo, true);
}

bool YarpBroker::getQosFromString(const char* qos, yarp::os::QosStyle& style) {
    string strQos(qos);
    transform(strQos.begin(), strQos.end(), strQos.begin(),
              (int(*)(int))toupper);
    strQos.erase( std::remove_if( strQos.begin(), strQos.end(), ::isspace ), strQos.end() );

    //level:high; priority:10; policy:1
    stringstream ss(strQos); // Turn the string into a stream.
    string prop;
    while(getline(ss, prop, ';')) {
        size_t p = prop.find(':');
        if (p != prop.npos) {
            string key = prop.substr(0, p);
            string value = prop.substr(p+1);
            if (key.length() > 0 && value.length() > 0) {
                if (key == "LEVEL" || key=="DSCP" || key == "TOS") {
                    if(!style.setPacketPriority(prop))
                        return false;
                }
                else if (key == "PRIORITY") {
                    char* p;
                    int prio = strtol(value.c_str(), &p, 10);
                    style.setThreadPriority(prio);
                }
                else if (key == "POLICY") {
                    char* p;
                    int policy = strtol(value.c_str(), &p, 10);
                    style.setThreadPolicy(policy);
                }
            }
        }
    }
    return true;
}

const char* YarpBroker::error()
{
    return strError.c_str();
}


bool YarpBroker::timeout(double base, double timeout)
{
    SystemClock::delaySystem(1.0);
    if((SystemClock::nowSystem()-base) > timeout)
        return true;
    return false;
}

bool YarpBroker::threadInit()
{
    if(!strStdioUUID.size())
        return false;

    string strStdioPort = strStdioUUID + "/stdout";
    stdioPort.open("...");

    double base = SystemClock::nowSystem();
    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;
    while(!timeout(base, 5.0))
    {
        if(NetworkBase::connect(strStdioPort, stdioPort.getName(), style))
            return true;
    }

    strError = "Cannot connect to stdio port ";
    strError += strStdioPort;
    stdioPort.close();
    return false;
}


void YarpBroker::run()
{
    Bottle *input;
    if( (input=stdioPort.read(false)) && eventSink)
    {
        for (size_t i=0; i<input->size(); i++)
            eventSink->onBrokerStdout(input->get(i).asString().c_str());
    }
}


void YarpBroker::threadRelease()
{
    NetworkBase::disconnect(stdioPort.getName(), strStdioUUID);
    stdioPort.close();
}


int YarpBroker::SendMsg(Bottle& msg, std::string target, Bottle& response, float fTimeout)
{
    if(!exists(target.c_str()))
        return YARPRUN_NOCONNECTION;

    if(!semParam.check())
        return YARPRUN_SEMAPHORE_PARAM;

    // opening the port
    yarp::os::Port port;
    port.setTimeout(fTimeout);
    if(!port.open("..."))
    {
        __trace_message.clear();
        semParam.post();
        return YARPRUN_CONNECTION_TIMOUT;
    }

    ContactStyle style;
    style.quiet = true;
    style.timeout = CONNECTION_TIMEOUT;

    bool ret;
    __trace_message = "(SendMsg) connecting to " + string(target);
    for(int i=0; i<10; i++)
    {
        ret = NetworkBase::connect(port.getName(), target, style);
        if(ret) break;
        SystemClock::delaySystem(1.0);
    }

    if(!ret)
    {
        port.close();
        __trace_message.clear();
        semParam.post();
        return YARPRUN_CONNECTION_TIMOUT;
    }

    __trace_message = "(SendMsg) writing to " + string(target);
    ret = port.write(msg, response);
    __trace_message = "(SendMsg) disconnecting from " + string(target);
    NetworkBase::disconnect(port.getName(),target);
    __trace_message.clear();
    semParam.post();

    if(!response.size() || !ret) {
        port.close();
        return YARPRUN_NORESPONSE;
    }

    port.close();

    return YARPRUN_OK;
}


int YarpBroker::requestServer(Property& config)
{
    yarp::os::Bottle msg;

    // USE A YARP RUN SERVER TO MANAGE STDIO
    //
    // client -> stdio server -> cmd server
    //
    if (config.check("cmd") && config.check("stdio"))
    {
        if (config.find("stdio").asString()=="") {return YARPRUN_UNDEF; }
        if (config.find("cmd").asString()=="")   {return YARPRUN_UNDEF; }
        if (!config.check("as") || config.find("as").asString()=="") { return YARPRUN_UNDEF; }
        if (!config.check("on") || config.find("on").asString()=="") { return YARPRUN_UNDEF; }

        msg.addList()=config.findGroup("stdio");
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");
        msg.addList()=config.findGroup("on");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("geometry")) msg.addList()=config.findGroup("geometry");
        if (config.check("hold")) msg.addList()=config.findGroup("hold");
        if (config.check("env")) msg.addList()=config.findGroup("env");

        Bottle response;
        int ret = SendMsg(msg, config.find("stdio").asString(),
                          response, CONNECTION_TIMEOUT);
        if (ret != YARPRUN_OK)
            return ret;

        if(response.size() > 2)
            strStdioUUID = response.get(2).asString();

        return ((response.get(0).asInt32()>0)?YARPRUN_OK:YARPRUN_UNDEF);
    }

    // DON'T USE A RUN SERVER TO MANAGE STDIO
    //
    // client -> cmd server
    //
    if (config.check("cmd"))
    {
        if (config.find("cmd").asString()=="")   { return YARPRUN_UNDEF; }
        if (!config.check("as") || config.find("as").asString()=="") {return YARPRUN_UNDEF; }
        if (!config.check("on") || config.find("on").asString()=="") {return YARPRUN_UNDEF; }

        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");

        if (config.check("workdir")) msg.addList()=config.findGroup("workdir");
        if (config.check("env")) msg.addList()=config.findGroup("env");

        Bottle response;
        int ret = SendMsg(msg, config.find("on").asString(),
                          response, CONNECTION_TIMEOUT);
        if (ret != YARPRUN_OK)
            return ret;

        return ((response.get(0).asInt32()>0)?YARPRUN_OK:YARPRUN_UNDEF);
    }

    return YARPRUN_UNDEF;
}
