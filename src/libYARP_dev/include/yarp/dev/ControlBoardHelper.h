/*
 * Copyright (C) 2012 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_CONTROL_BOARD_HELPER
#define YARP_CONTROL_BOARD_HELPER

#include <string.h> // for memset
#include <stdio.h> // for printf
#include <math.h> //fabs
#include <yarp/os/Log.h>

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
    ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs, const double *nw, const double *amps=NULL, const double *volts=NULL): zeros(0),
        signs(0),
        axisMap(0),
        invAxisMap(0),
        angleToEncoders(0),
        newtonsToSensors(0),
        ampereToSensors(0),
        voltToSensors(0)
    {
        yAssert(n>=0);         // if number of joints is negative complain!
        yAssert(aMap!=0);      // at least the axisMap is required

        alloc(n, amps, volts);

        memcpy(axisMap, aMap, sizeof(int)*nj);

        if (zs!=0)
            memcpy(zeros, zs, sizeof(double)*nj);
        else
            memset(zeros, 0, sizeof(double)*nj);

        if (angToEncs!=0)
            memcpy(angleToEncoders, angToEncs, sizeof(double)*nj);
        else
            memset(angleToEncoders, 0, sizeof(double)*nj);

        if (nw!=0)
            memcpy(newtonsToSensors, nw, sizeof(double)*nj);
        else
            memset(newtonsToSensors, 0, sizeof(double)*nj);

        if (amps!=0)
            memcpy(ampereToSensors, amps, sizeof(double)*nj);

        if (volts!=0)
            memcpy(voltToSensors, volts, sizeof(double)*nj);;

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

    bool alloc(int n, const double *amps, const double *volts)
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

        if(amps)
        {
            ampereToSensors=new double [nj];
            yAssert(ampereToSensors != 0);
        }

        if(volts)
        {
            voltToSensors=new double [nj];
            yAssert(voltToSensors != 0);
        }

        yAssert(zeros != 0 && signs != 0 && axisMap != 0 && invAxisMap != 0 && angleToEncoders != 0 && newtonsToSensors != 0);
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
};
inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }

#endif
