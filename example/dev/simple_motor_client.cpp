// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


/*

  This is imported from the iCub repository.

  To try it out, start:
   yarpdev --device test_motor --name /fakebot/head --axes 8

  Then do:
   ./simple_motor_client --robot fakebot --part head

  It should say "working with 8 axes" and give you a prompt.

  You can type things like:
    help
    [get] [axes]
    [set] [pos] 4 5.0
    [get] [encs]

  Other commands may not be supported by test_motor, but may be useful
  for your specific hardware.

  When you type a command, there is an echo with the prefix "Bottle:".
  This gives you the exact syntax (in text form) of the message being
  sent.  You can then use such commands with yarp rpc:

  $ yarp rpc /fakebot/head/rpc:i
    RPC connection to /fakebot/head/rpc:i at tcp://127.0.0.1:10002 (...)
    [get] [axes]
    Response: [is] [axes] 8 [ok]
    [set] [pos] 4 5.0
    Response: [ok]
    [get] [encs]
    Response: [is] [encs] (0.0 0.0 0.0 0.0 5.0 0.0 0.0 0.0) [ok]
    ...

 */

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/String.h> 

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp;

#define VOCAB_HELP VOCAB4('h','e','l','p')
#define VOCAB_QUIT VOCAB4('q','u','i','t')

//
int main(int argc, char *argv[]) 
{
    // just list the devices if no argument given
    if (argc <= 2) {
        ACE_OS::printf("You can call %s like this:\n", argv[0]);
        ACE_OS::printf("   %s --robot ROBOTNAME --OPTION VALUE ...\n", argv[0]);
        ACE_OS::printf("For example:\n");
        ACE_OS::printf("   %s --robot icub --part any --remote /controlboard\n", argv[0]);
        ACE_OS::printf("Here are devices listed for your system:\n");
        ACE_OS::printf("%s", Drivers::factory().toString().c_str());
        return 0;
    }

    // get command line options
    Property options;
    options.fromCommand(argc, argv);
    if (!options.check("robot") || !options.check("part")) {
        ACE_OS::printf("Missing either --robot or --part options\n");
        return 0;
    }

    Network yarp;
	Time::turboBoost();
    
    yarp::String name((size_t)1024);
    Value& v = options.find("robot");
    Value& part = options.find("part");

    Value *val;
    if (!options.check("device", val)) {
        options.put("device", "remote_controlboard");
    }
    if (!options.check("local", val)) {
        ACE_OS::sprintf(&name[0], "/%s/%s/client", v.asString().c_str(), part.asString().c_str());
        options.put("local", name.c_str());
    }
    if (!options.check("remote", val)) {
        ACE_OS::sprintf(&name[0], "/%s/%s", v.asString().c_str(), part.asString().c_str());
        options.put("remote", name.c_str());
    }

	fprintf(stderr, "%s", options.toString().c_str());

    
    // create a device 
    PolyDriver dd(options);
    if (!dd.isValid()) {
        ACE_OS::printf("Device not available.  Here are the known devices:\n");
        ACE_OS::printf("%s", Drivers::factory().toString().c_str());
        return 1;
    }

    IPositionControl *pos;
    IVelocityControl *vel;
    IEncoders *enc;
    IPidControl *pid;
    IAmplifierControl *amp;
    IControlLimits *lim;

    bool ok;
    ok = dd.view(pos);
    ok &= dd.view(vel);
    ok &= dd.view(enc);
    ok &= dd.view(pid);
    ok &= dd.view(amp);
    ok &= dd.view(lim);

    if (!ok) {
        ACE_OS::printf("Problems acquiring interfaces\n");
        return 1;
    }

    int jnts = 0;
    pos->getAxes(&jnts);
    ACE_OS::printf("Working with %d axes\n", jnts);
    double *tmp = new double[jnts];
    ACE_ASSERT (tmp != NULL);

    ACE_OS::printf("Device active...\n");
    while (dd.isValid()) {
        String s;
        s.resize(1024);
        
        ACE_OS::printf("-> ");
        char c = 0;
        int i = 0;
        while (c != '\n') {
            c = (char)ACE_OS::fgetc(stdin);
            s[i++] = c;
        }
        s[i-1] = s[i] = 0;

        Bottle p;
        p.fromString(s.c_str());
        ACE_OS::printf("Bottle: %s\n", p.toString().c_str());

        switch(p.get(0).asVocab()) {        
        case VOCAB_HELP:
            ACE_OS::printf("\n\n");
            ACE_OS::printf("Available commands:\n\n");

            ACE_OS::printf("type [get] and one of the following:\n");
            ACE_OS::printf("[%s] to read the number of controlled axes\n", Vocab::decode(VOCAB_AXES).c_str());
            ACE_OS::printf("[%s] to read the encoder value for all axes\n", Vocab::decode(VOCAB_ENCODERS).c_str());
            ACE_OS::printf("[%s] <int> to read the PID values for a single axis\n", Vocab::decode(VOCAB_PID).c_str());
            ACE_OS::printf("[%s] <int> to read the limit values for a single axis\n", Vocab::decode(VOCAB_LIMITS).c_str());
            ACE_OS::printf("[%s] to read the PID error for all axes\n", Vocab::decode(VOCAB_ERRS).c_str());
            ACE_OS::printf("[%s] to read the PID output for all axes\n", Vocab::decode(VOCAB_OUTPUTS).c_str());
            ACE_OS::printf("[%s] to read the reference position for all axes\n", Vocab::decode(VOCAB_REFERENCES).c_str());
            ACE_OS::printf("[%s] to read the reference speed for all axes\n", Vocab::decode(VOCAB_REF_SPEEDS).c_str());
            ACE_OS::printf("[%s] to read the reference acceleration for all axes\n", Vocab::decode(VOCAB_REF_ACCELERATIONS).c_str());
            ACE_OS::printf("[%s] to read the current consumption for all axes\n", Vocab::decode(VOCAB_AMP_CURRENTS).c_str());

            ACE_OS::printf("\n");

            ACE_OS::printf("type [set] and one of the following:\n");
            ACE_OS::printf("[%s] <int> <double> to move a single axis\n", Vocab::decode(VOCAB_POSITION_MOVE).c_str());
            ACE_OS::printf("[%s] <int> <double> to accelerate a single axis to a given speed\n", Vocab::decode(VOCAB_VELOCITY_MOVE).c_str());            
            ACE_OS::printf("[%s] <int> <double> to set the reference speed for a single axis\n", Vocab::decode(VOCAB_REF_SPEED).c_str());
            ACE_OS::printf("[%s] <int> <double> to set the reference acceleration for a single axis\n", Vocab::decode(VOCAB_REF_ACCELERATION).c_str());
            ACE_OS::printf("[%s] <list> to move multiple axes\n", Vocab::decode(VOCAB_POSITION_MOVES).c_str());
            ACE_OS::printf("[%s] <list> to accelerate multiple axes to a given speed\n", Vocab::decode(VOCAB_VELOCITY_MOVES).c_str());
            ACE_OS::printf("[%s] <list> to set the reference speed for all axes\n", Vocab::decode(VOCAB_REF_SPEEDS).c_str());
            ACE_OS::printf("[%s] <list> to set the reference acceleration for all axes\n", Vocab::decode(VOCAB_REF_ACCELERATIONS).c_str());          
            ACE_OS::printf("[%s] <int> to stop a single axis\n", Vocab::decode(VOCAB_STOP).c_str());
            ACE_OS::printf("[%s] <int> to stop all axes\n", Vocab::decode(VOCAB_STOPS).c_str());
            ACE_OS::printf("[%s] <int> <list> to set the PID values for a single axis\n", Vocab::decode(VOCAB_PID).c_str());
            ACE_OS::printf("[%s] <int> <list> to set the limits for a single axis\n", Vocab::decode(VOCAB_LIMITS).c_str());
            ACE_OS::printf("[%s] <int> to disable the PID control for a single axis\n", Vocab::decode(VOCAB_DISABLE).c_str());
            ACE_OS::printf("[%s] <int> to enable the PID control for a single axis\n", Vocab::decode(VOCAB_ENABLE).c_str());
            ACE_OS::printf("[%s] <int> <double> to set the encoder value for a single axis\n", Vocab::decode(VOCAB_ENCODER).c_str());
            ACE_OS::printf("[%s] <list> to set the encoder value for all axes\n", Vocab::decode(VOCAB_ENCODERS).c_str());
            ACE_OS::printf("\n");
            break;

        case VOCAB_QUIT:
            goto ApplicationCleanQuit;
            break;

        case VOCAB_GET:
            switch(p.get(1).asVocab()) {
                case VOCAB_AXES: {
                    int nj = 0;
                    enc->getAxes(&nj);
                    ACE_OS::printf ("%s: %d\n", Vocab::decode(VOCAB_AXES).c_str(), nj);
                }
                break;

                case VOCAB_ENCODERS: {
                    enc->getEncoders(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_ENCODERS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");
                }
                break;

                case VOCAB_PID: {
                    Pid pd;
                    int j = p.get(2).asInt();
                    pid->getPid(j, &pd);
                    ACE_OS::printf("%s: ", Vocab::decode(VOCAB_PID).c_str());
                    ACE_OS::printf("kp %.2f ", pd.kp);
                    ACE_OS::printf("kd %.2f ", pd.kd);
                    ACE_OS::printf("ki %.2f ", pd.ki);
                    ACE_OS::printf("maxi %.2f ", pd.max_int);
                    ACE_OS::printf("maxo %.2f ", pd.max_output);
                    ACE_OS::printf("off %.2f ", pd.offset);
                    ACE_OS::printf("scale %.2f ", pd.scale);
                    ACE_OS::printf("\n");
                }
                break;

                case VOCAB_LIMITS: {
                    double min, max;
                    int j = p.get(2).asInt();
                    lim->getLimits(j, &min, &max);
                    ACE_OS::printf("%s: ", Vocab::decode(VOCAB_LIMITS).c_str());
                    ACE_OS::printf("limits: (%.2f %.2f)\n", min, max);
                }
                break;

                case VOCAB_ERRS: {
                    pid->getErrorLimits(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_ERRS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");
                }
                break;

                case VOCAB_OUTPUTS: {
                    pid->getErrors(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_OUTPUTS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");
                }
                break;

                case VOCAB_REFERENCES: {
                    pid->getReferences(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_REFERENCES).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");                    
                }
                break;

                case VOCAB_REF_SPEEDS: {
                    pos->getRefSpeeds(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_REF_SPEEDS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");                    
                }
                break;

                case VOCAB_REF_ACCELERATIONS: {
                    pos->getRefAccelerations(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_REF_ACCELERATIONS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");                    
                }
                break;

                case VOCAB_AMP_CURRENTS: {
                    amp->getCurrents(tmp);
                    ACE_OS::printf ("%s: (", Vocab::decode(VOCAB_AMP_CURRENTS).c_str());
                    for(i = 0; i < jnts; i++)
                        ACE_OS::printf ("%.2f ", tmp[i]);
                    ACE_OS::printf (")\n");
                }
                break;
            }
            break;

        case VOCAB_SET:
            switch(p.get(1).asVocab()) {
                case VOCAB_POSITION_MOVE: {
                    int j = p.get(2).asInt();
                    double ref = p.get(3).asDouble();
                    ACE_OS::printf("%s: moving %d to %.2f\n", Vocab::decode(VOCAB_POSITION_MOVE).c_str(), j, ref);
                    pos->positionMove(j, ref);
                }
                break;

                case VOCAB_VELOCITY_MOVE: {
                    int j = p.get(2).asInt();
                    double ref = p.get(3).asDouble();
                    ACE_OS::printf("%s: accelerating %d to %.2f\n", Vocab::decode(VOCAB_VELOCITY_MOVE).c_str(), j, ref);
                    vel->velocityMove(j, ref);
                }
                break;

                case VOCAB_REF_SPEED: {
                    int j = p.get(2).asInt();
                    double ref = p.get(3).asDouble();
                    ACE_OS::printf("%s: setting speed for %d to %.2f\n", Vocab::decode(VOCAB_REF_SPEED).c_str(), j, ref);
                    pos->setRefSpeed(j, ref);
                }
                break;

                case VOCAB_REF_ACCELERATION: {
                    int j = p.get(2).asInt();
                    double ref = p.get(3).asDouble();
                    ACE_OS::printf("%s: setting acceleration for %d to %.2f\n", Vocab::decode(VOCAB_REF_ACCELERATION).c_str(), j, ref);
                    pos->setRefAcceleration(j, ref);
                }
                break;

                case VOCAB_POSITION_MOVES: {
                    Bottle *l = p.get(2).asList();
                    for (i = 0; i < jnts; i++) {
                        tmp[i] = l->get(i).asDouble();
                    }
                    ACE_OS::printf("%s: moving all joints\n", Vocab::decode(VOCAB_POSITION_MOVES).c_str());
                    pos->positionMove(tmp);
                }
                break;

                case VOCAB_VELOCITY_MOVES: {
                    Bottle *l = p.get(2).asList();
                    for (i = 0; i < jnts; i++) {
                        tmp[i] = l->get(i).asDouble();
                    }
                    ACE_OS::printf("%s: moving all joints\n", Vocab::decode(VOCAB_VELOCITY_MOVES).c_str());
                    vel->velocityMove(tmp);
                }
                break;

                case VOCAB_REF_SPEEDS: {
                    Bottle *l = p.get(2).asList();
                    for (i = 0; i < jnts; i++) {
                        tmp[i] = l->get(i).asDouble();
                    }
                    ACE_OS::printf("%s: setting speed for all joints\n", Vocab::decode(VOCAB_REF_SPEEDS).c_str());
                    pos->setRefSpeeds(tmp);
                }
                break;

                case VOCAB_REF_ACCELERATIONS: {
                    Bottle *l = p.get(2).asList();
                    for (i = 0; i < jnts; i++) {
                        tmp[i] = l->get(i).asDouble();
                    }
                    ACE_OS::printf("%s: setting acceleration for all joints\n", Vocab::decode(VOCAB_REF_ACCELERATIONS).c_str());
                    pos->setRefAccelerations(tmp);
                }
                break;

                case VOCAB_STOP: {
                    int j = p.get(2).asInt();
                    ACE_OS::printf("%s: stopping axis %d\n", Vocab::decode(VOCAB_STOP).c_str());
                    pos->stop(j);
                }
                break;

                case VOCAB_STOPS: {
                    ACE_OS::printf("%s: stopping all axes %d\n", Vocab::decode(VOCAB_STOPS).c_str());
                    pos->stop();
                }
                break;

                case VOCAB_ENCODER: {
                    int j = p.get(2).asInt();
                    double ref = p.get(3).asDouble();
                    ACE_OS::printf("%s: setting the encoder value for %d to %.2f\n", Vocab::decode(VOCAB_ENCODER).c_str(), j, ref);
                    enc->setEncoder(j, ref);                    
                }
                break; 

                case VOCAB_ENCODERS: {
                    Bottle *l = p.get(2).asList();
                    for (i = 0; i < jnts; i++) {
                        tmp[i] = l->get(i).asDouble();
                    }
                    ACE_OS::printf("%s: setting the encoder value for all joints\n", Vocab::decode(VOCAB_ENCODERS).c_str());
                    enc->setEncoders(tmp);
                }
                break;

                case VOCAB_PID: {
                    Pid pd;
                    int j = p.get(2).asInt();
                    Bottle *l = p.get(3).asList();
                    pd.kp = l->get(0).asDouble();
                    pd.kd = l->get(1).asDouble();
                    pd.ki = l->get(2).asDouble();
                    pd.max_int = l->get(3).asDouble();
                    pd.max_output = l->get(4).asDouble();
                    pd.offset = l->get(5).asDouble();
                    pd.scale = l->get(6).asDouble();
                    ACE_OS::printf("%s: setting PID values for axis %d\n", Vocab::decode(VOCAB_PID).c_str(), j);
                    pid->setPid(j, pd);
                }
                break;

                case VOCAB_DISABLE: {
                    int j = p.get(2).asInt();
                    ACE_OS::printf("%s: disabling control for axis %d\n", Vocab::decode(VOCAB_DISABLE).c_str(), j);
                    pid->disablePid(j);
                    amp->disableAmp(j);
                }
                break;

                case VOCAB_ENABLE: {
                    int j = p.get(2).asInt();
                    ACE_OS::printf("%s: enabling control for axis %d\n", Vocab::decode(VOCAB_ENABLE).c_str(), j);
                    amp->enableAmp(j);
                    pid->enablePid(j);
                }
                break;

                case VOCAB_LIMITS: {
                    int j = p.get(2).asInt();
                    ACE_OS::printf("%s: setting limits for axis %d\n", Vocab::decode(VOCAB_LIMITS).c_str(), j);
                    Bottle *l = p.get(3).asList();
                    lim->setLimits(j, l->get(0).asDouble(), l->get(1).asDouble());
                }
                break;
            }
            break;
        } /* switch get(0) */

    } /* while () */

ApplicationCleanQuit:
    dd.close();
    delete[] tmp;

    return 0;
}
