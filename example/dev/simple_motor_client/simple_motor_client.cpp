/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
  To try it out, start:
   yarpdev --device fakeMotionControl --name /fakebot/head --GENERAL::Joints 8

  Then do:
   ./simple_motor_client --robot fakebot --part head

  It should say "working with 8 axes" and give you a prompt.

  You can type things like:
    help
    [get] [axes]
    [set] [pos] 4 5.0
    [get] [encs]

  Other commands may not be supported by fakeMotionControl, but may be useful
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

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/dev/PolyDriver.h>

#include <assert.h>

using yarp::dev::Drivers;
using yarp::dev::IAmplifierControl;
using yarp::dev::IControlLimits;
using yarp::dev::IEncoders;
using yarp::dev::IPidControl;
using yarp::dev::IPositionControl;
using yarp::dev::IVelocityControl;
using yarp::dev::Pid;
using yarp::dev::PolyDriver;
using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Property;
using yarp::os::Value;


int main(int argc, char* argv[])
{
    // just list the devices if no argument given
    if (argc <= 2) {
        printf("You can call %s like this:\n", argv[0]);
        printf("   %s --robot ROBOTNAME --OPTION VALUE ...\n", argv[0]);
        printf("For example:\n");
        printf("   %s --robot icub --part any --remote /controlboard\n", argv[0]);
        printf("Here are devices listed for your system:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 0;
    }

    // get command line options
    Property options;
    options.fromCommand(argc, argv);
    if (!options.check("robot") || !options.check("part")) {
        printf("Missing either --robot or --part options\n");
        return 0;
    }

    Network yarp;

    char name[1024];
    Value& v = options.find("robot");
    Value& part = options.find("part");

    Value* val;
    if (!options.check("device", val)) {
        options.put("device", "remote_controlboard");
    }
    if (!options.check("local", val)) {
        sprintf(name, "/%s/%s/client", v.asString().c_str(), part.asString().c_str());
        options.put("local", name);
    }
    if (!options.check("remote", val)) {
        sprintf(name, "/%s/%s", v.asString().c_str(), part.asString().c_str());
        options.put("remote", name);
    }

    fprintf(stderr, "%s", options.toString().c_str());


    // create a device
    PolyDriver dd(options);
    if (!dd.isValid()) {
        printf("Device not available.  Here are the known devices:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 1;
    }

    IPositionControl* pos;
    IVelocityControl* vel;
    IEncoders* enc;
    IPidControl* pid;
    IAmplifierControl* amp;
    IControlLimits* lim;

    bool ok;
    ok = dd.view(pos);
    ok &= dd.view(vel);
    ok &= dd.view(enc);
    ok &= dd.view(pid);
    ok &= dd.view(amp);
    ok &= dd.view(lim);

    if (!ok) {
        printf("Problems acquiring interfaces\n");
        return 1;
    }

    int jnts = 0;
    pos->getAxes(&jnts);
    printf("Working with %d axes\n", jnts);
    double* tmp = new double[jnts];
    assert(tmp != nullptr);

    printf("Device active...\n");
    while (dd.isValid()) {
        char s[1024];

        printf("-> ");
        char c = 0;
        int i = 0;
        while (c != '\n') {
            c = static_cast<char>(fgetc(stdin));
            s[i++] = c;
        }
        s[i - 1] = s[i] = 0;

        Bottle p;
        p.fromString(s);
        printf("Bottle: %s\n", p.toString().c_str());

        switch (p.get(0).asVocab32()) {
        case VOCAB_HELP:
            printf("\n\n");
            printf("Available commands:\n\n");

            printf("type [get] and one of the following:\n");
            printf("[%s] to read the number of controlled axes\n", yarp::os::Vocab32::decode(VOCAB_AXES).c_str());
            printf("[%s] to read the encoder value for all axes\n", yarp::os::Vocab32::decode(VOCAB_ENCODERS).c_str());
            printf("[%s] <int> to read the PID values for a single axis\n", yarp::os::Vocab32::decode(VOCAB_PID).c_str());
            printf("[%s] <int> to read the limit values for a single axis\n", yarp::os::Vocab32::decode(VOCAB_LIMITS).c_str());
            printf("[%s] to read the PID error for all axes\n", yarp::os::Vocab32::decode(VOCAB_ERRS).c_str());
            printf("[%s] to read the PID output for all axes\n", yarp::os::Vocab32::decode(VOCAB_OUTPUTS).c_str());
            printf("[%s] to read the reference position for all axes\n", yarp::os::Vocab32::decode(VOCAB_REFERENCES).c_str());
            printf("[%s] to read the reference speed for all axes\n", yarp::os::Vocab32::decode(VOCAB_REF_SPEEDS).c_str());
            printf("[%s] to read the reference acceleration for all axes\n", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATIONS).c_str());
            printf("[%s] to read the current consumption for all axes\n", yarp::os::Vocab32::decode(VOCAB_AMP_CURRENTS).c_str());

            printf("\n");

            printf("type [set] and one of the following:\n");
            printf("[%s] <int> <double> to move a single axis\n", yarp::os::Vocab32::decode(VOCAB_POSITION_MOVE).c_str());
            printf("[%s] <int> <double> to accelerate a single axis to a given speed\n", yarp::os::Vocab32::decode(VOCAB_VELOCITY_MOVE).c_str());
            printf("[%s] <int> <double> to set the reference speed for a single axis\n", yarp::os::Vocab32::decode(VOCAB_REF_SPEED).c_str());
            printf("[%s] <int> <double> to set the reference acceleration for a single axis\n", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATION).c_str());
            printf("[%s] <list> to move multiple axes\n", yarp::os::Vocab32::decode(VOCAB_POSITION_MOVES).c_str());
            printf("[%s] <list> to accelerate multiple axes to a given speed\n", yarp::os::Vocab32::decode(VOCAB_VELOCITY_MOVES).c_str());
            printf("[%s] <list> to set the reference speed for all axes\n", yarp::os::Vocab32::decode(VOCAB_REF_SPEEDS).c_str());
            printf("[%s] <list> to set the reference acceleration for all axes\n", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATIONS).c_str());
            printf("[%s] <int> to stop a single axis\n", yarp::os::Vocab32::decode(VOCAB_STOP).c_str());
            printf("[%s] <int> to stop all axes\n", yarp::os::Vocab32::decode(VOCAB_STOPS).c_str());
            printf("[%s] <int> <list> to set the PID values for a single axis\n", yarp::os::Vocab32::decode(VOCAB_PID).c_str());
            printf("[%s] <int> <list> to set the limits for a single axis\n", yarp::os::Vocab32::decode(VOCAB_LIMITS).c_str());
            printf("[%s] <int> to disable the PID control for a single axis\n", yarp::os::Vocab32::decode(VOCAB_DISABLE).c_str());
            printf("[%s] <int> to enable the PID control for a single axis\n", yarp::os::Vocab32::decode(VOCAB_ENABLE).c_str());
            printf("[%s] <int> <double> to set the encoder value for a single axis\n", yarp::os::Vocab32::decode(VOCAB_ENCODER).c_str());
            printf("[%s] <list> to set the encoder value for all axes\n", yarp::os::Vocab32::decode(VOCAB_ENCODERS).c_str());
            printf("\n");
            break;

        case VOCAB_QUIT:
            goto ApplicationCleanQuit;
            break;

        case VOCAB_GET:
            switch (p.get(1).asVocab32()) {
            case VOCAB_AXES: {
                int nj = 0;
                enc->getAxes(&nj);
                printf("%s: %d\n", yarp::os::Vocab32::decode(VOCAB_AXES).c_str(), nj);
            } break;

            case VOCAB_ENCODERS: {
                enc->getEncoders(tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_ENCODERS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_PID: {
                Pid pd;
                int j = p.get(2).asInt32();
                pid->getPid(yarp::dev::VOCAB_PIDTYPE_POSITION, j, &pd);
                printf("%s: ", yarp::os::Vocab32::decode(VOCAB_PID).c_str());
                printf("kp %.2f ", pd.kp);
                printf("kd %.2f ", pd.kd);
                printf("ki %.2f ", pd.ki);
                printf("maxi %.2f ", pd.max_int);
                printf("maxo %.2f ", pd.max_output);
                printf("off %.2f ", pd.offset);
                printf("scale %.2f ", pd.scale);
                printf("\n");
            } break;

            case VOCAB_LIMITS: {
                double min;
                double max;
                int j = p.get(2).asInt32();
                lim->getLimits(j, &min, &max);
                printf("%s: ", yarp::os::Vocab32::decode(VOCAB_LIMITS).c_str());
                printf("limits: (%.2f %.2f)\n", min, max);
            } break;

            case VOCAB_ERRS: {
                pid->getPidErrorLimits(yarp::dev::VOCAB_PIDTYPE_POSITION, tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_ERRS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_OUTPUTS: {
                pid->getPidErrors(yarp::dev::VOCAB_PIDTYPE_POSITION, tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_OUTPUTS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_REFERENCES: {
                pid->getPidReferences(yarp::dev::VOCAB_PIDTYPE_POSITION, tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_REFERENCES).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_REF_SPEEDS: {
                pos->getRefSpeeds(tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_REF_SPEEDS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_REF_ACCELERATIONS: {
                pos->getRefAccelerations(tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATIONS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;

            case VOCAB_AMP_CURRENTS: {
                amp->getCurrents(tmp);
                printf("%s: (", yarp::os::Vocab32::decode(VOCAB_AMP_CURRENTS).c_str());
                for (i = 0; i < jnts; i++) {
                    printf("%.2f ", tmp[i]);
                }
                printf(")\n");
            } break;
            }
            break;

        case VOCAB_SET:
            switch (p.get(1).asVocab32()) {
            case VOCAB_POSITION_MOVE: {
                int j = p.get(2).asInt32();
                double ref = p.get(3).asFloat64();
                printf("%s: moving %d to %.2f\n", yarp::os::Vocab32::decode(VOCAB_POSITION_MOVE).c_str(), j, ref);
                pos->positionMove(j, ref);
            } break;

            case VOCAB_VELOCITY_MOVE: {
                int j = p.get(2).asInt32();
                double ref = p.get(3).asFloat64();
                printf("%s: accelerating %d to %.2f\n", yarp::os::Vocab32::decode(VOCAB_VELOCITY_MOVE).c_str(), j, ref);
                vel->velocityMove(j, ref);
            } break;

            case VOCAB_REF_SPEED: {
                int j = p.get(2).asInt32();
                double ref = p.get(3).asFloat64();
                printf("%s: setting speed for %d to %.2f\n", yarp::os::Vocab32::decode(VOCAB_REF_SPEED).c_str(), j, ref);
                pos->setRefSpeed(j, ref);
            } break;

            case VOCAB_REF_ACCELERATION: {
                int j = p.get(2).asInt32();
                double ref = p.get(3).asFloat64();
                printf("%s: setting acceleration for %d to %.2f\n", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATION).c_str(), j, ref);
                pos->setRefAcceleration(j, ref);
            } break;

            case VOCAB_POSITION_MOVES: {
                Bottle* l = p.get(2).asList();
                for (i = 0; i < jnts; i++) {
                    tmp[i] = l->get(i).asFloat64();
                }
                printf("%s: moving all joints\n", yarp::os::Vocab32::decode(VOCAB_POSITION_MOVES).c_str());
                pos->positionMove(tmp);
            } break;

            case VOCAB_VELOCITY_MOVES: {
                Bottle* l = p.get(2).asList();
                for (i = 0; i < jnts; i++) {
                    tmp[i] = l->get(i).asFloat64();
                }
                printf("%s: moving all joints\n", yarp::os::Vocab32::decode(VOCAB_VELOCITY_MOVES).c_str());
                vel->velocityMove(tmp);
            } break;

            case VOCAB_REF_SPEEDS: {
                Bottle* l = p.get(2).asList();
                for (i = 0; i < jnts; i++) {
                    tmp[i] = l->get(i).asFloat64();
                }
                printf("%s: setting speed for all joints\n", yarp::os::Vocab32::decode(VOCAB_REF_SPEEDS).c_str());
                pos->setRefSpeeds(tmp);
            } break;

            case VOCAB_REF_ACCELERATIONS: {
                Bottle* l = p.get(2).asList();
                for (i = 0; i < jnts; i++) {
                    tmp[i] = l->get(i).asFloat64();
                }
                printf("%s: setting acceleration for all joints\n", yarp::os::Vocab32::decode(VOCAB_REF_ACCELERATIONS).c_str());
                pos->setRefAccelerations(tmp);
            } break;

            case VOCAB_STOP: {
                int j = p.get(2).asInt32();
                printf("%s: stopping axis %d\n", yarp::os::Vocab32::decode(VOCAB_STOP).c_str(), j);
                pos->stop(j);
            } break;

            case VOCAB_STOPS: {
                printf("%s: stopping all axes\n", yarp::os::Vocab32::decode(VOCAB_STOPS).c_str());
                pos->stop();
            } break;

            case VOCAB_ENCODER: {
                int j = p.get(2).asInt32();
                double ref = p.get(3).asFloat64();
                printf("%s: setting the encoder value for %d to %.2f\n", yarp::os::Vocab32::decode(VOCAB_ENCODER).c_str(), j, ref);
                enc->setEncoder(j, ref);
            } break;

            case VOCAB_ENCODERS: {
                Bottle* l = p.get(2).asList();
                for (i = 0; i < jnts; i++) {
                    tmp[i] = l->get(i).asFloat64();
                }
                printf("%s: setting the encoder value for all joints\n", yarp::os::Vocab32::decode(VOCAB_ENCODERS).c_str());
                enc->setEncoders(tmp);
            } break;

            case VOCAB_PID: {
                Pid pd;
                int j = p.get(2).asInt32();
                Bottle* l = p.get(3).asList();
                pd.kp = l->get(0).asFloat64();
                pd.kd = l->get(1).asFloat64();
                pd.ki = l->get(2).asFloat64();
                pd.max_int = l->get(3).asFloat64();
                pd.max_output = l->get(4).asFloat64();
                pd.offset = l->get(5).asFloat64();
                pd.scale = l->get(6).asFloat64();
                printf("%s: setting PID values for axis %d\n", yarp::os::Vocab32::decode(VOCAB_PID).c_str(), j);
                pid->setPid(yarp::dev::VOCAB_PIDTYPE_POSITION, j, pd);
            } break;

            case VOCAB_DISABLE: {
                int j = p.get(2).asInt32();
                printf("%s: disabling control for axis %d\n", yarp::os::Vocab32::decode(VOCAB_DISABLE).c_str(), j);
                pid->disablePid(yarp::dev::VOCAB_PIDTYPE_POSITION, j);
                amp->disableAmp(j);
            } break;

            case VOCAB_ENABLE: {
                int j = p.get(2).asInt32();
                printf("%s: enabling control for axis %d\n", yarp::os::Vocab32::decode(VOCAB_ENABLE).c_str(), j);
                amp->enableAmp(j);
                pid->enablePid(yarp::dev::VOCAB_PIDTYPE_POSITION, j);
            } break;

            case VOCAB_LIMITS: {
                int j = p.get(2).asInt32();
                printf("%s: setting limits for axis %d\n", yarp::os::Vocab32::decode(VOCAB_LIMITS).c_str(), j);
                Bottle* l = p.get(3).asList();
                lim->setLimits(j, l->get(0).asFloat64(), l->get(1).asFloat64());
            } break;
            }
            break;
        } /* switch get(0) */

    } /* while () */

ApplicationCleanQuit:
    dd.close();
    delete[] tmp;

    return 0;
}
