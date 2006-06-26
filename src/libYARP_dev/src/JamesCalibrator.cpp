#include <yarp/dev/JamesCalibrator.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

JamesCalibrator::JamesCalibrator()
{
    //empty now
}

JamesCalibrator::~JamesCalibrator()
{
    //empty now
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
   
    ret = false;
    for(int k=0;k<nj;k++)
        {
            ret=ret&&calibrateJoint(k);
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
                
                iCalibrate->calibrate(2);

                int i;
                for (i = 0; i < timeout; i++)
                    {
                        iCalibrate->done(2);
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

                iCalibrate->calibrate(1);

                int i;
                for (i = 0; i < timeout; i++)
                    {
                        iCalibrate->done(1);
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

                iCalibrate->calibrate(2);

                int i;
                for (i = 0; i < timeout; i++)
                    {
                        iCalibrate->done(2);
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
                iCalibrate->calibrate(13);
                int i;
                for (i = 0; i < timeout; i++)
                    {
                        iCalibrate->done(13);
                        Time::delay(1.0);
                    }
                if (i == timeout)
                    return false;

                goToZero(13);

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
    iPosition->positionMove(j, 0);
}
