
#include <iostream>
#include <yarp/dev/AnalogWrapper.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createAnalogWrapper() {
    return new DriverCreatorOf<AnalogWrapper>("analogServer",
        "analogServer",
        "AnalogWrapper");
}

namespace analogWrapper_yarp_internal_namespace {
/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
  **/

AnalogServerHandler::AnalogServerHandler(const char* n)
{
    rpcPort.open(n);
    rpcPort.setReader(*this);
}

AnalogServerHandler::~AnalogServerHandler()
{
    rpcPort.close();
    is = 0;
}

void AnalogServerHandler::setInterface(yarp::dev::IAnalogSensor *is)
{
    this->is = is;
}

bool AnalogServerHandler::_handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply)
{
    if (is==0)
      return false;

    int msgsize=cmd.size();

    int code=cmd.get(1).asVocab();
    switch (code)
    {
    case VOCAB_CALIBRATE:
      if (msgsize==2)
        is->calibrateSensor();
      else
      {
        //read Vector of values and pass to is->calibrate();
      }
      return true;
      break;
    case VOCAB_CALIBRATE_CHANNEL:
      if (msgsize==3)
      {
        int ch=cmd.get(2).asInt();
        is->calibrateChannel(ch);
      }
      if (msgsize==4)
      {
        int ch=cmd.get(2).asInt();
        double v=cmd.get(3).asDouble();
        is->calibrateChannel(ch, v);
      }

      return true;
      break;
    default:
      return false;
    }
}

bool AnalogServerHandler::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok=in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret=false;
    if (code==VOCAB_IANALOG)
    {
        ret=_handleIAnalog(in, out);
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender!=NULL) {
        out.write(*returnToSender);
    }
    return true;
}


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */

AnalogPortEntry::AnalogPortEntry(void) { }

AnalogPortEntry::AnalogPortEntry(const AnalogPortEntry &alt)
{
    this->length = alt.length;
    this->offset = alt.offset;
    this->port_name = alt.port_name;
}

AnalogPortEntry &AnalogPortEntry::operator =(const AnalogPortEntry &alt)
{
    this->length = alt.length;
    this->offset = alt.offset;
    this->port_name = alt.port_name;
    return *this;
}

} // closing namespace analogWrapper_yarp_internal_namespace

/**
  * It reads the data from an analog sensor and sends them on one or more ports.
  * It creates one rpc port and its related handler for every output port.
  */

// Constructor used when there is only one output port
AnalogWrapper::AnalogWrapper(const char* name, int rate): RateThread(rate)
{
    createPort(name, rate);
}

bool AnalogWrapper::createPort(const char* name, int rate)
{
    analogSensor_p=0;
    analogPorts.resize(1);
    analogPorts[0].offset = 0;
    analogPorts[0].length = -1; // max length
    analogPorts[0].port_name = std::string(name);
    setHandlers();
    setRate(rate);
    return true;
}

// Contructor used when one or more output ports are specified
AnalogWrapper::AnalogWrapper(const std::vector<AnalogPortEntry>& _analogPorts, int rate): RateThread(rate)
{
  createPorts(_analogPorts, rate);
}
bool AnalogWrapper::createPorts(const std::vector<AnalogPortEntry>& _analogPorts, int rate)
{
    analogSensor_p=0;
    this->analogPorts=_analogPorts;
    setHandlers();
    setRate(rate);
    return true;
}

AnalogWrapper::AnalogWrapper(): RateThread(0)
{
    _rate = 0;
    analogSensor_p = NULL;
}

AnalogWrapper::~AnalogWrapper()
{
    threadRelease();
    _rate = 0;
    analogSensor_p = NULL;
}

void AnalogWrapper::setHandlers()
{
    for(unsigned int i=0;i<analogPorts.size(); i++)
    {
        std::string rpcPortName = analogPorts[i].port_name;
        rpcPortName += "/rpc:i";
        AnalogServerHandler* ash = new AnalogServerHandler(rpcPortName.c_str());
        handlers.push_back(ash);
    }
}

void AnalogWrapper::removeHandlers()
{
    for(unsigned int i=0; i<handlers.size(); i++)
    {
        if (handlers[i]!=NULL)
            delete handlers[i];
    }
    handlers.clear();
}


/**
  * Specify which analog sensor this thread has to read from.
  */

bool AnalogWrapper::attachAll(const PolyDriverList &analog2attach)
{
    if (analog2attach.size() != 1)
    {
        std::cerr<<"AnalogServer: cannot attach more than one device\n";
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach=analog2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(analogSensor_p);
    }

    if(NULL == analogSensor_p)
    {
        std::cerr << "AnalogServer: subdevice passed to attach method is invalid!!!";
        return false;
    }
    attach(analogSensor_p);
    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool AnalogWrapper::detachAll()
{
    analogSensor_p = NULL;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
    return true;
}

void AnalogWrapper::attach(yarp::dev::IAnalogSensor *s)
{
    analogSensor_p=s;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
}

void AnalogWrapper::detach()
{
    // Set interface to NULL
    analogSensor_p = NULL;
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        handlers[i]->setInterface(analogSensor_p);
    }
}

bool AnalogWrapper::threadInit()
{
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        // open data port
        if (!analogPorts[i].port.open(analogPorts[i].port_name.c_str()))
            return false;
    }
    return true;
}

void AnalogWrapper::setId(const std::string &i)
{
    id=i;
}

std::string AnalogWrapper::getId()
{
    return id;
}

Bottle AnalogWrapper::getOptions()
{
    Bottle options;
    options.addString("robotName, mandatory");
    options.addString("deviceId, mandatory");
    options.addString("period");
    return options;
}

bool AnalogWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());
    bool correct=true;

    // Verify minimum set of parameters required
    if(!params.check("robotName", "name of the robot.") )
    {
        correct=false;
        cerr << "AnalogServer missing robot Name, check your configuration file!! Quitting\n";
        return false;
    }

    if(params.check("deviceId"))
    {
      string tmp(params.find("deviceId").asString());// .asList();
//       string tmp(deviceId->get(0).asString());
      cout << "AnalogWrapper Debug" << tmp;
      setId(tmp);
    }
    else
    {
        printf("no device Id found\n");
        return false;
    }

    if (params.check("period"))
    {
        _rate=params.find("period").asInt();
    }
    else
    {
        _rate=20;
        std::cout <<"Warning: part "<< id <<" using default period ("<<_rate<<")\n";
    }

    // Create the list of ports
    std::string robotName=params.find("robotName").asString().c_str();
    std::string root_name;
    root_name+="/";
    root_name+=robotName;
    root_name+= "/" + this->id + "/analog";

    // port names are optional, do not check for correctness.
    if(!params.check("ports"))
    {
     // if there is no "ports" section open only 1 port and use name as is.
        createPort((root_name+":o" ).c_str(), _rate );
        cout << "opening port " << root_name.c_str();

    }
    else
    {
        Bottle *ports=params.find("ports").asList();

        if (!params.check("total_taxels", "number of taxels of the part"))
            return false;
        int total_taxels=params.find("total_taxels").asInt();
        int nports=ports->size();
        int totalT = 0;
        std::vector<AnalogPortEntry> tmpPorts;
        tmpPorts.resize(nports);

        for(int k=0;k<ports->size();k++)
        {
            Bottle parameters=params.findGroup(ports->get(k).asString().c_str());

            if (parameters.size()!=5)
            {
                cerr <<"check skin port parameters in part description";
                cerr << "--> I was expecting "<<ports->get(k).asString().c_str() << " followed by four integers";
                return false;
            }

            int wBase=parameters.get(1).asInt();
            int wTop=parameters.get(2).asInt();
            int base=parameters.get(3).asInt();
            int top=parameters.get(4).asInt();

            cout<<"--> "<<wBase<<" "<<wTop<<" "<<base<<" "<<top<<endl;

            //check consistenty
            if(wTop-wBase != top-base){
                cerr<<"Error: check skin port parameters in part description"<<endl;
                cerr<<"Numbers of mapped taxels do not match.\n";
                return false;
            }
            int taxels=top-base+1;

            tmpPorts[k].length = taxels;
            tmpPorts[k].offset = wBase;
            cout << "opening port " << ports->get(k).asString().c_str();
            tmpPorts[k].port_name = root_name+ "/" +string(ports->get(k).asString().c_str()) + ":o";

            createPorts(tmpPorts, _rate);
            totalT+=taxels;
        }

        if (totalT!=total_taxels)
        {
            cerr << "Error total number of mapped taxels does not correspond to total taxels";
            return false;
        }
    }
    return true;
}

void AnalogWrapper::threadRelease()
{
    for(unsigned int i=0; i<analogPorts.size(); i++)
    {
        analogPorts[i].port.interrupt();
        analogPorts[i].port.close();
    }
}

void AnalogWrapper::run()
{
    int first, last, ret;
    if (analogSensor_p!=0)
    {
        // read from the analog sensor
        yarp::sig::Vector v;

        ret=analogSensor_p->read(v);

        if (ret==yarp::dev::IAnalogSensor::AS_OK)
        {
            if (v.size()>0)
            {
                lastStateStamp.update();
                // send the data on the port(s), splitting them as specified in the config file
                for(unsigned int i=0; i<analogPorts.size(); i++){
                    yarp::sig::Vector &pv = analogPorts[i].port.prepare();
                    first = analogPorts[i].offset;
                    if(analogPorts[i].length==-1)   // read the max length available
                        last = v.size()-1;
                    else
                        last = analogPorts[i].offset + analogPorts[i].length - 1;
                    // check vector limit
                    if(last>=(int)v.size()){
                        cerr<<"Error while sending analog sensor output on port "<< analogPorts[i].port_name<< endl;
                        cerr<<"Vector size expected to be at least "<<last<<" whereas it is "<< v.size()<< endl;
                        continue;
                    }
                    pv = v.subVector(first, last);
                    analogPorts[i].port.setEnvelope(lastStateStamp);
                    analogPorts[i].port.write();
                }
            }
        }
        else
        {
            //todo release
        }
    }
}

bool AnalogWrapper::close()
{
    fprintf(stderr, "AnalogServer::Close\n");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    RateThread::stop();
    detachAll();
    removeHandlers();
    return true;
}

// eof



