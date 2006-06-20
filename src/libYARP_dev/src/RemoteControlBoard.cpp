// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/String.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace yarp{
    namespace dev {
      class RemoteControlBoard;
      class ServerControlBoard;
      class CommandsHelper;
      class ControlHelper;
    }
}

#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')

// interface IPidControl
#define VOCAB_PID VOCAB3('p','i','d')
#define VOCAB_PIDS VOCAB4('p','i','d','s')

// interface IPositionControl gets
#define VOCAB_AXES VOCAB4('a','x','e','s')

// interface IPositionControl sets
#define VOCAB_POSITION_MODE VOCAB4('p','o','s','m')

/**
 * Helper object for reading config commands for the ServerControlBoard
 * class.
 */
class yarp::dev::CommandsHelper : public PortReader {
protected:
    yarp::dev::ServerControlBoard *caller;
    yarp::dev::IPositionControl *ctrl;

public:
    /**
     * Constructor.
     * @param x is the pointer to the instance of the object that uses the CommandsHelper.
     * This is required to recover the pointers to the interfaces that implement the responses
     * to the commands.
     */
    CommandsHelper(yarp::dev::ServerControlBoard *x);

    /**
     * read from the connection.
     * @param connection is a reference to a ConnectionReader object which encapsulates
     * the current port connection.
     * @return true on a successful read.
     */
    virtual bool read(ConnectionReader& connection);
};

/**
 * Helper object for reading control commands for the ServerControlBoard
 * class.
 */
class yarp::dev::ControlHelper : public PortReader {
protected:
    yarp::dev::ServerControlBoard *caller;
    yarp::dev::IPositionControl *ctrl;

public:
    /**
     * Constructor.
     * @param x is the pointer to the instance of the object that uses the ControlHelper.
     * This is required to recover the pointers to the interfaces that implement the responses
     * to the commands.
     */
    ControlHelper(yarp::dev::ServerControlBoard *x);

    /**
     * read from the connection.
     * @param connection is a reference to a ConnectionReader object which encapsulates
     * the current port connection.
     * @return true on a successful read.
     */
    virtual bool read(ConnectionReader& connection);
};

/**
 * ServerControlBoard class, implement the server side of a remote
 * control board device driver. The device contains three ports:
 * - command_p handling the configuration interfaces of the robot
 * - state_p streaming information about the current state of the robot
 * - control_p receiving a stream of control commands (e.g. position)
 * 
 */
class yarp::dev::ServerControlBoard : 
            public DeviceDriver, 
            public Thread,
            public IPositionControl
            // convenient to put these here just to make sure all
            // methods get implemented
{
private:
	bool spoke;
    Port command_p; // RPC to configure the robot
    Port state_p;   // out port to read the state
    Port control_p; // in port to command the robot

    PortWriterBuffer<yarp::sig::Vector> state_writer;
    
    yarp::dev::CommandsHelper command_reader;
    yarp::dev::ControlHelper control_reader;

    PolyDriver poly;

    IPositionControl *ctrl;
    // LATER: other interfaces here.

    Property settings;

public:
    ServerControlBoard() : command_reader(this), control_reader(this) {
        ctrl = NULL;
		spoke = false;
    }
    
    virtual bool open() {
        return false;
    }
    
    virtual bool close() {
        return true;
    }
    
    virtual bool open(Searchable& prop) {
        // attach readers.
        command_p.setReader(command_reader);
        control_p.setReader(control_reader);
        // attach writers.
        state_writer.attach(state_p);
        
        BottleBit *name;
        if (prop.check("subdevice",name)) {
            ACE_OS::printf("Subdevice %s\n", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                Property p;
                p.fromString(prop.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                poly.open(*name);
            }
            if (!poly.isValid()) {
                ACE_OS::printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            ACE_OS::printf("\"--subdevice <name>\" not set for server_framegrabber\n");
            return false;
        }
        if (prop.check("name",name)) {
            String s((size_t)1024);
            ACE_OS::sprintf(&s[0], "%s/rpc", name->asString().c_str());
            command_p.open(s.c_str());
            ACE_OS::sprintf(&s[0], "%s/control", name->asString().c_str());
            control_p.open(s.c_str());
            ACE_OS::sprintf(&s[0], "%s/state", name->asString().c_str());
            state_p.open(s.c_str());
        } else {
            command_p.open("/controlboard/rpc");
            control_p.open("/controlboard/control");
            state_p.open("/controlboard/state");
        }
        if (poly.isValid()) {
            poly.view(ctrl);
        }
        if (ctrl != NULL) {
            start();
            return true;
        }
        ACE_OS::printf("subdevice <%s> doesn't look like a control board\n",
               name->toString().c_str());
        return false;
    }

    /**
     * The thread main loop deals with writing on ports here.
     */
    virtual void run() {
        ACE_OS::printf("Server control board starting\n");
        while (!isStopping()) {
            yarp::sig::Vector q (10, 0.0);

            yarp::sig::Vector& v = state_writer.get();
            // getCurrentPosition()
			if (!spoke) {
                ACE_OS::printf("Network control board writing a %d vector of double...\n",
                    v.size());
				spoke = true;
			}
            state_writer.write();
        }
        ACE_OS::printf("Server control board stopping\n");
    }

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    virtual bool getAxes(int *ax) {
        if (ctrl) {
            ctrl->getAxes(ax);
            return true;
        }

        *ax = 0;
        return false;
    }

    /** 
     * Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * return true/false on success/failure
     */
    virtual bool setPositionMode() {
        if (ctrl) {
            ctrl->setPositionMode();
            return true;
        }

        return false;
    }

    /** Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref) { return false; }

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs) { return false; }

    /** Set relative position. The command is relative to the 
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool relativeMove(int j, double delta) { return false; }

    /** Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const double *deltas) { return false; }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(int j, bool *flag) { return false; }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) { return false; }

    /** Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp) { return false; }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const double *spds) { return false; }

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc) { return false; }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const double *accs) { return false; }

    /** Get reference speed for a joint. Returns the speed used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref) { return false; }

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual bool getRefSpeeds(double *spds) { return false; }

    /** Get reference acceleration for a joint. Returns the acceleration used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual bool getRefAcceleration(int j, double *acc) { return false; }

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure 
     */
    virtual bool getRefAccelerations(double *accs) { return false; }

    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool stop(int j) { return false; }

    /** Stop motion, multiple joints 
     * @return true/false on success/failure
     */
    virtual bool stop() { return false; }
};

/**
 * The remoted side of the control board (client side).
 */
class yarp::dev::RemoteControlBoard : 
            public IPositionControl, 
            // LATER: public more interfaces here,
            public DeviceDriver {
protected:
    Port port;
    PortReaderBuffer<yarp::sig::Vector> reader;
    String remote;
    String local;

    bool setCommand(int code) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        port.write(cmd);
        return true;
    }

    bool setCommand(int code, double v) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        port.write(cmd);
        return true;
    }

    bool setCommand(int code, int v) {
        Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addInt(v);
        port.write(cmd);
        return true;
    }

    bool getCommand(int code, double& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        v = response.get(2).asDouble();
        return true;
    }

    bool getCommand(int code, int& v) const {
        Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        v = response.get(2).asInt();
        return true;
    }

public:
    RemoteControlBoard() {
    }

    virtual ~RemoteControlBoard() {
    }

    virtual bool open() {
        return true;
    }

    virtual bool open(Searchable& config) {
        remote = config.find("remote").asString().c_str();
        local = config.find("local").asString().c_str();
        if (local != "") {
            port.open(local.c_str());
        }
        if (remote != "") {
            Network::connect(remote.c_str(),local.c_str());

            // reverse connection for RPC
            // could choose to do this only on need
            Network::connect(local.c_str(), remote.c_str());
        }
        reader.attach(port);
        return true;
    }

    virtual bool close() {
        return true;
    }

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param ax pointer to storage
     * @return true/false.
     */
    virtual bool getAxes(int *ax) {
        return getCommand(VOCAB_AXES, *ax);
    }

    /** 
     * Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * return true/false on success/failure
     */
    virtual bool setPositionMode() {
        return setCommand(VOCAB_POSITION_MODE);
    }

    /** Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref) { return false; }

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs) { return false; }

    /** Set relative position. The command is relative to the 
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool relativeMove(int j, double delta) { return false; }

    /** Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const double *deltas) { return false; }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(int j, bool *flag) { return false; }

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(bool *flag) { return false; }

    /** Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp) { return false; }

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const double *spds) { return false; }

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc) { return false; }

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const double *accs) { return false; }

    /** Get reference speed for a joint. Returns the speed used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref) { return false; }

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual bool getRefSpeeds(double *spds) { return false; }

    /** Get reference acceleration for a joint. Returns the acceleration used to 
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual bool getRefAcceleration(int j, double *acc) { return false; }

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure 
     */
    virtual bool getRefAccelerations(double *accs) { return false; }

    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool stop(int j) { return false; }

    /** Stop motion, multiple joints 
     * @return true/false on success/failure
     */
    virtual bool stop() { return false; }
};



// implementation of CommandsHelper
yarp::dev::CommandsHelper::CommandsHelper(yarp::dev::ServerControlBoard *x) { 
    ACE_ASSERT (x != NULL);
    caller = x; 
    ctrl = dynamic_cast<yarp::dev::IPositionControl *> (caller);
}

bool yarp::dev::CommandsHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    cmd.read(connection);
    ACE_OS::printf("command received: %s\n", cmd.toString().c_str());
    int code = cmd.get(0).asVocab();
    switch (code) {
    case VOCAB_SET:
        ACE_OS::printf("set command received\n");
        {
            bool ok = false;
            switch(cmd.get(1).asVocab()) {
            case VOCAB_POSITION_MODE:
                ok = ctrl->setPositionMode();
                break;
            default:
                ACE_OS::printf("received an unknown command after a VOCAB_SET\n");
                break;
            }
        }
        break;

    case VOCAB_GET:
        ACE_OS::printf("get command received\n");
        {
            bool ok = false;
            int tmp = 0;
            response.addVocab(VOCAB_IS);
            response.addBit(cmd.get(1));
            switch(cmd.get(1).asVocab()) {
            case VOCAB_AXES:
                ok = true;
                ctrl->getAxes(&tmp);
                response.addInt(tmp);
                break;
            default:
                ACE_OS::printf("received an unknown request after a VOCAB_GET\n");
                break;
            }

            if (!ok) {
                // leave answer blank
            }
        }
        break;
    }

    if (response.size()>=1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=NULL) {
            response.write(*writer);
            ACE_OS::printf("response sent: %s\n", response.toString().c_str());
        }
    }
    return true;
}

// implementation of ControlHelper
yarp::dev::ControlHelper::ControlHelper(yarp::dev::ServerControlBoard *x) { 
    ACE_ASSERT (x != NULL);
    caller = x; 
    ctrl = dynamic_cast<yarp::dev::IPositionControl *> (caller);
}

bool yarp::dev::ControlHelper::read(ConnectionReader& connection) {
    Vector v;
    v.read(connection);
    ACE_OS::printf("data received of size: %d\n", v.size());

    // do I need to check for the type of data (e.g. Bottle)?
    // Vector should be binary compatible with Bottle...

    // LATER: switch depending on the control mode.
    // ctrl->positionMove (&v[0]);

    // no response is needed here.
    return true;
}

// needed for the driver factory.
yarp::dev::DriverCreator *createServerControlBoard() {
    return new DriverCreatorOf<ServerControlBoard>("controlboard", "controlboard");
}

yarp::dev::DriverCreator *createRemoteControlBoard() {
    return new DriverCreatorOf<RemoteControlBoard>("remote_controlboard", "controlboard");
}
