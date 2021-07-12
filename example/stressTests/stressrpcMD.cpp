/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Property.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Vector.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#include <string>
#include <sstream>

using namespace std;

int main(int argc, char **argv)
{
    Network yarp;

    printf("Going to stress rpc connections to the robot\n");
    printf("Run as --id unique-id\n");
    printf("Optionally:\n");
    printf("--part robot-part\n");
    printf("--prot protocol\n");
    printf("--time dt (seconds)\n");
    printf("--robot name \n");

    Property parameters;
    parameters.fromCommand(argc, argv);

    std::string part=parameters.find("part").asString();
    int id=parameters.find("id").asInt32();
    double time=0;
    if (parameters.check("time"))
        {
            time=parameters.find("time").asFloat64();
        }
    else
        time=-1;

    std::string protocol;
    if (parameters.check("prot"))
    {
        protocol=parameters.find("prot").asString();
    }
    else
        protocol="tcp";

    std::string rname;
    if (parameters.check("robot"))
    {
        rname=parameters.find("robot").asString();
    }
    else
        rname="controlboard";

    PolyDriver dd;
    Property p;

    string remote=string("/")+rname.c_str();
    if (part!="")
        {
            remote+=string("/");
            remote+=part;
        }
    string local=string("/")+string(rname.c_str());
    if (part!="")
        {
            local+=string("/");
            local+=part;
        }
    local+=string("/stress");

    stringstream lStream;
    lStream << id;
    local += lStream.str();

    p.put("device", "remote_controlboard");
    p.put("local", local.c_str());
    p.put("remote", remote.c_str());
    p.put("carrier", protocol.c_str());
    dd.open(p);

    if (!dd.isValid())
    {
        fprintf(stderr, "Error, could not open controlboard\n");
        return -1;
    }

    IEncoders *ienc;
    IPositionControl *ipos;
    IAmplifierControl *iamp;
    IPidControl *ipid;
    IControlLimits *ilim;
    IControlCalibration2 *ical;

    dd.view(ienc);
    dd.view(ipos);
    dd.view(iamp);
    dd.view(ipid);
    dd.view(ilim);
    dd.view(ical);

    int c=100;
    int nj;
    Vector encoders;
    ienc->getAxes(&nj);
    encoders.resize(nj);

    int count=0;
    bool done=false;
    double startT=Time::now();
    double now=0;
    while((!done) || (time==-1))
        {
            count++;
            double v;
            int jj=0;

            for(jj=0; jj<nj; jj++)
                {
                    //    ienc->getEncoder(jj, encoders.data()+jj);
                    //    iamp->enableAmp(jj);
                    //    ilim->setLimits(jj, 0, 0);
                    //    double max;
                    //    double min;
                    //    ilim->getLimits(jj, &min, &max);
                    fprintf(stderr, "Calling rpc ");
                    // Pid pid;
                    // ipid->getPid(jj, &pid);
                    bool done;
                    ipos->checkMotionDone(jj, &done);
                    fprintf(stderr, "done\n");
                }

            fprintf(stderr, "read %u times\n", count);

            Time::delay(0.1);

            now=Time::now();
            if ((now-startT)>time)
                done=true;
        }

    printf("bye bye from %d\n", id);

    return 0;
}
