// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/dev/JamesCalibrator.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

// calibrator for the arm of the robot James
// LATER: change name of the class to JamesArmCalibrator.

JamesCalibrator::JamesCalibrator()
{
    param = NULL;
	pos = NULL;
	vel = NULL;
}

JamesCalibrator::~JamesCalibrator()
{
    //empty now
}

bool JamesCalibrator::open (yarp::os::Searchable& config)
{
    Property p;
    p.fromString(config.toString());

    if (!p.check("GENERAL")) {
        fprintf(stderr, "Cannot understand configuration parameters\n");
        return false;
    }

    int nj = p.findGroup("GENERAL").find("Joints").asInt();
	param = new double[nj];
	ACE_ASSERT (param != NULL);
	pos = new double[nj];
	ACE_ASSERT (pos != NULL);
	vel = new double[nj];
	ACE_ASSERT (vel != NULL);

	Bottle& xtmp = p.findGroup("CALIBRATION").findGroup("Calibration");
	ACE_ASSERT (xtmp.size() == nj+1);
	int i;
	for (i = 1; i < xtmp.size(); i++)
		param[i-1] = xtmp.get(i).asDouble();

	xtmp = p.findGroup("CALIBRATION").findGroup("PositionZero");
	ACE_ASSERT (xtmp.size() == nj+1);
	for (i = 1; i < xtmp.size(); i++)
		pos[i-1] = xtmp.get(i).asDouble();

	xtmp = p.findGroup("CALIBRATION").findGroup("VelocityZero");
	ACE_ASSERT (xtmp.size() == nj+1);
	for (i = 1; i < xtmp.size(); i++)
		vel[i-1] = xtmp.get(i).asDouble();

	return true;
}

bool JamesCalibrator::close ()
{
	if (param != NULL) delete[] param;
	param = NULL;
	if (pos != NULL) delete[] pos;
	pos = NULL;
	if (vel != NULL) delete[] vel;
	vel = NULL;

	return true;
}

bool JamesCalibrator::calibrate(DeviceDriver *dd)
{
    iCalibrate = dynamic_cast<IControlCalibration *>(dd);
    iAmps =  dynamic_cast<IAmplifierControl *>(dd);
    iEncoders = dynamic_cast<IEncoders *>(dd);
    iPosition = dynamic_cast<IPositionControl *>(dd);
    iPids = dynamic_cast<IPidControl *>(dd);

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
		iPids->enablePid(2);
		iAmps->enableAmp(2);
	}

    ret = true;
	calibrateJoint(1);
	calibrateJoint(0);
    for(k=2;k<nj;k++)
    {
		bool x = calibrateJoint(k);
		ret = ret && x;
    }
    
    return ret;
}

bool JamesCalibrator::calibrateJoint(int joint)
{
    const int timeout = 60;

    switch (joint)
        {
        case 0:
            {
                iPids->disablePid(2);
                iAmps->disableAmp(2);

                iEncoders->resetEncoder(2);

                iPids->enablePid(2);
                iAmps->enableAmp(2);
                
                iCalibrate->calibrate(0, param[0]);

                int i;
                for (i = 0; i < timeout; i++)
                {
                    if (iCalibrate->done(0))
						break;
                    Time::delay(1.0);
                }
                if (i == timeout)
                    return false;
                
                return true;
            }
            break;

        case 1:
            {
                iPids->disablePid(0);
                iAmps->disableAmp(0);

                iPids->disablePid(2);
                iAmps->disableAmp(2);

                iCalibrate->calibrate(1, param[1]);

                int i;
                for (i = 0; i < timeout; i++)
                {
                    if (iCalibrate->done(1))
						break;
                    Time::delay(1.0);
                }
                if (i == timeout)
                    return false;
                
                return true;
            }
            break;

        case 2:
            {
                iEncoders->resetEncoder(2);

                iPids->enablePid(2);
                iAmps->enableAmp(2);

                iCalibrate->calibrate(2, param[2]);

                int i;
                for (i = 0; i < timeout; i++)
                {
                    if (iCalibrate->done(2))
						break;
                    Time::delay(1);
                }
                if (i == timeout)
                    return false;

                goToZero(0);
                goToZero(1);
                goToZero(2);
                return true;
            }
            break;

        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            {
                iCalibrate->calibrate(joint, param[joint]);
                int i;
                for (i = 0; i < timeout; i++)
                {
                    if (iCalibrate->done(joint))
						break;
                    Time::delay(1.0);
                }
                if (i == timeout)
                    return false;

                goToZero(joint);

                return true;
            }
            break;

        default:
            break;
        }

    return false;
}

void JamesCalibrator::goToZero(int j)
{
	iPosition->setRefSpeed(j, vel[j]);
    iPosition->positionMove(j, pos[j]);
}
