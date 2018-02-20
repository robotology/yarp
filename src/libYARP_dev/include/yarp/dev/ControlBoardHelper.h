/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDHELPER_H
#define YARP_DEV_CONTROLBOARDHELPER_H

#include <cstring> // for memset
#include <cstdio> // for printf
#include <cmath> //fabs
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*
 * simple helper template to alloc memory.
 */
template <class T>
inline T* allocAndCheck(int size)
{
    T* t = new T[size];
    yAssert (t != 0);
    memset(t, 0, sizeof(T) * size);
    return t;
}

/*
 *
 */
template <class T>
inline void checkAndDestroy(T* &p) {
    if (p!=0) {
        delete [] p;
        p = 0;
    }
}


//////////////// Control Board Helper
class ControlBoardHelper
{
public:
    ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs, const double *newtons, const double *amps = NULL, const double *volts = NULL, const double *dutycycles = NULL) :
        zeros(0),
        signs(0),
        axisMap(0),
        invAxisMap(0),
        angleToEncoders(0),
        newtonsToSensors(0),
        ampereToSensors(0),
        voltToSensors(0),
        dutycycleToPWMs(0)
    {
        yAssert(n>=0);         // if number of joints is negative complain!
        yAssert(aMap!=0);      // at least the axisMap is required

        alloc(n);

        memcpy(axisMap, aMap, sizeof(int)*nj);

        if (zs!=0)
            memcpy(zeros, zs, sizeof(double)*nj);
        else
            std::fill_n(zeros, nj, 0.0);

        if (angToEncs!=0)
            memcpy(angleToEncoders, angToEncs, sizeof(double)*nj);
        else
            std::fill_n(angleToEncoders, nj, 1.0);

        if (newtons!=0)
            memcpy(newtonsToSensors, newtons, sizeof(double)*nj);
        else
            std::fill_n(newtonsToSensors, nj, 1.0);

        if (amps!=0)
            memcpy(ampereToSensors, amps, sizeof(double)*nj);
        else
            std::fill_n(ampereToSensors, nj, 1.0);

        if (volts!=0)
            memcpy(voltToSensors, volts, sizeof(double)*nj);
        else
            std::fill_n(voltToSensors, nj, 1.0);

        if (dutycycles != 0)
            memcpy(dutycycleToPWMs, dutycycles, sizeof(double)*nj);
        else
            std::fill_n(dutycycleToPWMs, nj, 1.0);

        // invert the axis map
        memset (invAxisMap, 0, sizeof(int) * nj);
        int i;
        for (i = 0; i < nj; i++)
        {
            int j;
            for (j = 0; j < nj; j++)
            {
                if (axisMap[j] == i)
                {
                    invAxisMap[i] = j;
                    break;
                }
            }
        }

    }

    ~ControlBoardHelper()
    {
        dealloc();
    }

    bool alloc(int n)
    {
        nj=n;
        if (nj<=0)
            return false;

        if (zeros!=0)
            dealloc();

        zeros=new double [nj];
        signs=new double [nj];
        axisMap=new int [nj];
        invAxisMap=new int [nj];
        angleToEncoders=new double [nj];
        newtonsToSensors=new double [nj];
        ampereToSensors=new double [nj];
        voltToSensors=new double [nj];
        dutycycleToPWMs = new double[nj];

        yAssert( zeros != 0);
        yAssert( signs != 0);
        yAssert( axisMap != 0);
        yAssert( invAxisMap != 0);
        yAssert( angleToEncoders != 0);
        yAssert( newtonsToSensors != 0);
        yAssert( ampereToSensors != 0);
        yAssert( voltToSensors != 0);
        yAssert( dutycycleToPWMs != 0);

        return true;
    }

    bool dealloc()
    {
        checkAndDestroy<double> (zeros);
        checkAndDestroy<double> (signs);
        checkAndDestroy<int> (axisMap);
        checkAndDestroy<int> (invAxisMap);
        checkAndDestroy<double> (angleToEncoders);
        checkAndDestroy<double> (newtonsToSensors);
        checkAndDestroy<double> (ampereToSensors);
        checkAndDestroy<double> (voltToSensors);
        checkAndDestroy<double>(dutycycleToPWMs);
        return true;
    }

    inline bool checkAxisId(int id)
    {
        if (id >= nj)
        {
            return false;
        }
        return true;
    }

    inline int toHw(int axis)
    { return axisMap[axis]; }

    inline int toUser(int axis)
    { return invAxisMap[axis]; }

    //map a vector, no conversion
    inline void toUser(const double *hwData, double *user)
    {
        for (int k=0;k<nj;k++)
         user[toUser(k)]=hwData[k];
    }

     //map a vector, no conversion
    inline void toUser(const int *hwData, int *user)
    {
        for (int k=0;k<nj;k++)
         user[toUser(k)]=hwData[k];
    }

    //map a vector, no conversion
    inline void toHw(const double *usr, double *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }

     //map a vector, no conversion
    inline void toHw(const int *usr, int *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }

    inline void posA2E(double ang, int j, double &enc, int &k)
    {
        enc=(ang+zeros[j])*angleToEncoders[j];
        k=toHw(j);
    }

    inline double posA2E(double ang, int j)
    {
        return (ang+zeros[j])*angleToEncoders[j];
    }

    inline void posE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);

        ang=(enc/angleToEncoders[k])-zeros[k];
    }

    inline double posE2A(double enc, int j)
    {
        int k=toUser(j);

        return (enc/angleToEncoders[k])-zeros[k];
    }

    inline void impN2S(double newtons, int j, double &sens, int &k)
    {
        sens=newtons*newtonsToSensors[j]/angleToEncoders[j];
        k=toHw(j);
    }

    inline double impN2S(double newtons, int j)
    {
        return newtons*newtonsToSensors[j]/angleToEncoders[j];
    }

    inline void impN2S(const double *newtons, double *sens)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            impN2S(newtons[j], j, tmp, index);
            sens[index]=tmp;
        }
    }

    inline void trqN2S(double newtons, int j, double &sens, int &k)
    {
        sens=newtons*newtonsToSensors[j];
        k=toHw(j);
    }

    inline double trqN2S(double newtons, int j)
    {
        return newtons*newtonsToSensors[j];
    }

    //map a vector, convert from newtons to sensors
    inline void trqN2S(const double *newtons, double *sens)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            trqN2S(newtons[j], j, tmp, index);
            sens[index]=tmp;
        }
    }

    //map a vector, convert from sensor to newtons
    inline void trqS2N(const double *sens, double *newtons)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            trqS2N(sens[j], j, tmp, index);
            newtons[index]=tmp;
        }
    }

    inline void trqS2N(double sens, int j, double &newton, int &k)
    {
        k=toUser(j);

        newton=(sens/newtonsToSensors[k]);
    }

    inline double trqS2N(double sens, int j)
    {
        int k=toUser(j);

        return (sens/newtonsToSensors[k]);
    }

    inline void impS2N(const double *sens, double *newtons)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            impS2N(sens[j], j, tmp, index);
            newtons[index]=tmp;
        }
    }

    inline void impS2N(double sens, int j, double &newton, int &k)
    {
        k=toUser(j);

        newton=(sens/newtonsToSensors[k]*angleToEncoders[k]);
    }

    inline double impS2N(double sens, int j)
    {
        int k=toUser(j);

        return (sens/newtonsToSensors[k]*angleToEncoders[k]);
    }

    inline void velA2E(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*angleToEncoders[j];
    }

    inline void velA2E_abs(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*fabs(angleToEncoders[j]);
    }

    inline void velE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/angleToEncoders[k];
    }

    inline void velE2A_abs(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/fabs(angleToEncoders[k]);
    }

    inline void accA2E(double ang, int j, double &enc, int &k)
    {
        velA2E(ang, j, enc, k);
    }

    inline void accA2E_abs(double ang, int j, double &enc, int &k)
    {
        velA2E_abs(ang, j, enc, k);
    }

    inline void accE2A(double enc, int j, double &ang, int &k)
    {
        velE2A(enc, j, ang, k);
    }

    inline void accE2A_abs(double enc, int j, double &ang, int &k)
    {
        velE2A_abs(enc, j, ang, k);
    }

    inline double velE2A(double enc, int j)
    {
        int k=toUser(j);
        return enc/angleToEncoders[k];
    }

    inline double velE2A_abs(double enc, int j)
    {
        int k=toUser(j);
        return enc/fabs(angleToEncoders[k]);
    }


    inline double accE2A(double enc, int j)
    {
        return velE2A(enc, j);
    }

    inline double accE2A_abs(double enc, int j)
    {
        return velE2A_abs(enc, j);
    }

    //map a vector, convert from angles to encoders
    inline void posA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    //map a vector, convert from encoders to angles
    inline void posE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void velA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void velA2E_abs(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velA2E_abs(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void velE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void velE2A_abs(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velE2A_abs(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void accA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void accA2E_abs(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accA2E_abs(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void accE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void accE2A_abs(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accE2A_abs(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    //***************** current ******************//
    inline void ampereA2S(double ampere, int j, double &sens, int &k)
    {
        if(ampereToSensors)
            sens=ampere*ampereToSensors[j];
        else
            sens=ampere;
        k=toHw(j);
    }

    inline double ampereA2S(double ampere, int j)
    {
        if(ampereToSensors)
            return ampere*ampereToSensors[j];
        else
            return ampere;
    }

    //map a vector, convert from ampere to sensors
    inline void ampereA2S(const double *ampere, double *sens)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            ampereA2S(ampere[j], j, tmp, index);
            sens[index]=tmp;
        }
    }

    //map a vector, convert from sensor to ampere
    inline void ampereS2A(const double *sens, double *ampere)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            ampereS2A(sens[j], j, tmp, index);
            ampere[index]=tmp;
        }
    }

    inline void ampereS2A(double sens, int j, double &ampere, int &k)
    {
        k=toUser(j);
        if(ampereToSensors)
            ampere=(sens/ampereToSensors[k]);
        else
            ampere=sens;  //conversion factor = 1 if not defined;
    }

    inline double ampereS2A(double sens, int j)
    {
        int k=toUser(j);

        if(ampereToSensors)
            return sens/ampereToSensors[k];
        else
            return sens;  //conversion factor = 1 if not defined;
    }
    // *******************************************//

    //***************** voltage ******************//
    inline void voltageV2S(double voltage, int j, double &sens, int &k)
    {
        if(voltToSensors)
            sens=voltage*voltToSensors[j];
        else
            sens=voltage;
        k=toHw(j);
    }

    inline double voltageV2S(double voltage, int j)
    {
        if(voltToSensors)
            return voltage*voltToSensors[j];
        else
            return voltage;
    }

    //map a vector, convert from voltage to sensors
    inline void voltageV2S(const double *voltage, double *sens)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            voltageV2S(voltage[j], j, tmp, index);
            sens[index]=tmp;
        }
    }

    //map a vector, convert from sensor to newtons
    inline void voltageS2V(const double *sens, double *voltage)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            voltageS2V(sens[j], j, tmp, index);
            voltage[index]=tmp;
        }
    }

    inline void voltageS2V(double sens, int j, double &voltage, int &k)
    {
        k=toUser(j);

        if(voltToSensors)
            voltage=(sens/voltToSensors[k]);
        else
            voltage = sens;
    }

    inline double voltageS2V(double sens, int j)
    {
        int k=toUser(j);

        return (sens/voltToSensors[k]);
    }
    // *******************************************//

    //***************** dutycycle ******************//
    inline void dutycycle2PWM(double dutycycle, int j, double &pwm, int &k)
    {
        if (dutycycleToPWMs)
            pwm = dutycycle*dutycycleToPWMs[j];
        else
            pwm = dutycycle;
        k = toHw(j);
    }

    inline double dutycycle2PWM(double dutycycle, int j)
    {
        if (dutycycleToPWMs)
            return dutycycle*dutycycleToPWMs[j];
        else
            return dutycycle;
    }

    inline void dutycycle2PWM(const double *dutycycle, double *sens)
    {
        double tmp;
        int index;
        for (int j = 0; j<nj; j++)
        {
            dutycycle2PWM(dutycycle[j], j, tmp, index);
            sens[index] = tmp;
        }
    }

    inline void PWM2dutycycle(const double *pwm, double *dutycycle)
    {
        double tmp;
        int index;
        for (int j = 0; j<nj; j++)
        {
            PWM2dutycycle(pwm[j], j, tmp, index);
            dutycycle[index] = tmp;
        }
    }

    inline void PWM2dutycycle(double pwm, int j, double &dutycycle, int &k)
    {
        k = toUser(j);

        if (dutycycleToPWMs)
            dutycycle = (pwm / dutycycleToPWMs[k]);
        else
            dutycycle = pwm;
    }

    inline double PWM2dutycycle(double pwm, int j)
    {
        int k = toUser(j);

        return (pwm / dutycycleToPWMs[k]);
    }
    // *******************************************//

    inline int axes()
    { return nj; }

    int nj;

    double *zeros;
    double *signs;
    int *axisMap;
    int *invAxisMap;
    double *angleToEncoders;
    double *newtonsToSensors;
    double *ampereToSensors;
    double *voltToSensors;
    double *dutycycleToPWMs;
};
inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }

#endif // YARP_DEV_CONTROLBOARDHELPER_H
