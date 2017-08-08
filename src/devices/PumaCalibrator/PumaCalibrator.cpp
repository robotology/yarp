/*
* Copyright (C) 2007 Mattia Castelnovi
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "PumaCalibrator.h"

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/os/Time.h>
#include <yarp/os/Value.h>


using namespace yarp::os;
using namespace yarp::dev;

// calibrator for the arm of the Arm iCub

PumaCalibrator::PumaCalibrator()
{
    type   = NULL;
    speed1 = NULL;
    accs = NULL;
    PositionZero = NULL;
    pos = NULL;
    vel = NULL;
}

PumaCalibrator::~PumaCalibrator()
{
    //empty now
}

bool PumaCalibrator::open(yarp::os::Searchable& config)
{
    Property p;
    p.fromConfigFile("MEIconfig.txt");

    if (!p.check("GENERAL")) {
        fprintf(stderr, "Cannot understand configuration parameters\n");
        return false;
    }

    int nj = p.findGroup("GENERAL").find("Joints").asInt();
    type = new unsigned char[nj];
    ACE_ASSERT (type != NULL);
    speed1 = new double[nj];
    ACE_ASSERT (speed1 != NULL);
    accs = new double[nj];
    ACE_ASSERT (accs != NULL);
    PositionZero = new double[nj];
    ACE_ASSERT (PositionZero != NULL);

    pos = new double[nj];
    ACE_ASSERT (pos != NULL);
    vel = new double[nj];
    ACE_ASSERT (vel != NULL);

    Bottle& xtmp = p.findGroup("CALIBRATION").findGroup("PositionZero");
    ACE_ASSERT (xtmp.size() == nj+1);
    for (int i = 1; i < xtmp.size(); i++)
        PositionZero[i-1] = xtmp.get(i).asDouble();

    xtmp = p.findGroup("CALIBRATION").findGroup("Speed1");
    ACE_ASSERT (xtmp.size() == nj+1);
    for (int i = 1; i < xtmp.size(); i++)
        speed1[i-1] = xtmp.get(i).asDouble();

    xtmp = p.findGroup("CALIBRATION").findGroup("accs");
    ACE_ASSERT (xtmp.size() == nj+1);
    for (int i = 1; i < xtmp.size(); i++)
        accs[i-1] = (unsigned char) xtmp.get(i).asDouble();

    return true;
}

bool PumaCalibrator::close ()
{

    if (speed1 != NULL) delete[] speed1;
    speed1 = NULL;
    if (accs != NULL) delete[] accs;
    accs = NULL;
    if (PositionZero != NULL) delete[] PositionZero;
    PositionZero = NULL;

    return true;
}

bool PumaCalibrator::calibrate(DeviceDriver *dd)
{
    fprintf(stderr, "Calling PumaCalibrator::calibrate\n");
    iCalibrate    = dynamic_cast<IControlCalibration *>(dd);
    iAmps        = dynamic_cast<IAmplifierControl *>(dd);
    iEncoders    = dynamic_cast<IEncoders *>(dd);
    iPosition    = dynamic_cast<IPositionControl *>(dd);
    iPids        = dynamic_cast<IPidControl *>(dd);

    if (!(iCalibrate&&iAmps&&iPosition&&iPids))
        return false;

    // ok we have all interfaces
    int nj=0;
    bool ret=iEncoders->getAxes(&nj);

    if (!ret)
        return false;

    int k;
    for (k = 0; k < nj; k++)
    {
        iAmps->enableAmp(k);
    }

    ret = true;
    for(k=nj;k>0;k--)
    {
        bool x = calibrateJoint(k-1);
        ret = ret && x;

    }

    return ret;
}

bool PumaCalibrator::calibrateJoint(int joint)
{
    double myzero=0.0;
    //second value has to be checked
    iCalibrate->calibrate(joint,myzero);


    return true;
}

void PumaCalibrator::goToZero(int j)
{
    iPosition->setRefSpeed(j, speed1[j]);
    iPosition->positionMove(j, PositionZero[j]);

    // wait.
    bool finished = false;
    int timeout = 0;
    while (!finished)
    {
        iPosition->checkMotionDone(j, &finished);

        Time::delay (0.5);
        timeout ++;
        if (timeout >= 20)
        {
            fprintf(stderr, "Timeout on joint %d while going to zero\n", j);
            finished = true;
        }
    }
}

bool PumaCalibrator::park(DeviceDriver *dd, bool wait)
{
    int nj=0;
    bool ret=iEncoders->getAxes(&nj);
    if (!ret)
        return false;

    int timeout = 0;
    fprintf(stderr, "ARMCALIB::Calling iCubHeadCalibrator::park()");
    iPosition->setRefSpeeds(speed1);
    iPosition->positionMove(PositionZero);

    if (wait)
    {
        bool done=false;
        while((!done) && (timeout<20))
        {
            iPosition->checkMotionDone(&done);
            fprintf(stderr, ".");
            SystemClock::delaySystem(0.5);
            timeout++;
        }
        if(!done)
        {
            for(int j=0; j < nj; j++)
            {
                iPosition->checkMotionDone(j, &done);
                if (iPosition->checkMotionDone(j, &done))
                {
                    if (!done)
                        fprintf(stderr, "iCubArmCalibrator::park() : joint %d not in position ", j);
                }
                else
                    fprintf(stderr, "iCubArmCalibrator::park() : joint %d did not answer ", j);
            }
        }
    }

    fprintf(stderr, "ARMCALIB::done!\n");
    return true;
}

bool PumaCalibrator::quitCalibrate()
{
    printf("\n not yet implemented for puma!");
    return false;
}

bool PumaCalibrator::quitPark()
{
    printf("\n not yet implemented for puma!");
    return false;
}
