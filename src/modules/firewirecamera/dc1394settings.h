// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __DC1394_SETTINGS_H__
#define __DC1394_SETTINGS_H__

extern "C"{
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
}

class CDC1394Settings
{
public:
    CDC1394Settings()
    {
        m_pHandle=NULL;
        m_CameraNode=0;
    }

    void Create(raw1394handle_t pHandle,nodeid_t CameraNode)
    {
        m_pHandle=pHandle;
        m_CameraNode=CameraNode;

        dc1394_get_min_value(m_pHandle,m_CameraNode,FEATURE_BRIGHTNESS,&m_iMinBrightness);
        dc1394_get_min_value(m_pHandle,m_CameraNode,FEATURE_GAIN,&m_iMinGain);
        dc1394_get_min_value(m_pHandle,m_CameraNode,FEATURE_SHUTTER,&m_iMinShutter);
        dc1394_get_min_value(m_pHandle,m_CameraNode,FEATURE_WHITE_BALANCE,&m_iMinWB);

        dc1394_get_max_value(m_pHandle,m_CameraNode,FEATURE_BRIGHTNESS,&m_iMaxBrightness);
        dc1394_get_max_value(m_pHandle,m_CameraNode,FEATURE_GAIN,&m_iMaxGain);
        dc1394_get_max_value(m_pHandle,m_CameraNode,FEATURE_SHUTTER,&m_iMaxShutter);
        dc1394_get_max_value(m_pHandle,m_CameraNode,FEATURE_WHITE_BALANCE,&m_iMaxWB);

        SetAutoBrightness(false);
        SetAutoGain(false);
        SetAutoShutter(false);
        SetAutoWhiteBalance(false);

        // set to default values
        SetBrightness(0.5);
        SetGain(0.5);
        SetShutter(0.5);
        SetWhiteBalance(0.5,0.5);
    }

    void SetBrightness(double dBrightness)
    {
        if (!m_pHandle) return;

        m_dBrightness=dBrightness;

        dc1394_set_brightness(m_pHandle,m_CameraNode,NormToValue(m_dBrightness,m_iMinBrightness,m_iMaxBrightness));
    }

    void SetGain(double dGain)
    {
        if (!m_pHandle) return;

        m_dGain=dGain;

        dc1394_set_gain(m_pHandle,m_CameraNode,NormToValue(m_dGain,m_iMinGain,m_iMaxGain));
    }

    void SetShutter(double dShutter)
    {
        if (!m_pHandle) return;

        m_dShutter=dShutter;

        dc1394_set_shutter(m_pHandle,m_CameraNode,NormToValue(m_dShutter,m_iMinShutter,m_iMaxShutter));
    }

    void SetWhiteBalance(double dUB,double dVR)
    {
        if (!m_pHandle) return;

        m_dUB=dUB;
        m_dVR=dVR;

        dc1394_set_white_balance(m_pHandle,m_CameraNode,
                                 NormToValue(m_dUB,m_iMinWB,m_iMaxWB),
                                 NormToValue(m_dVR,m_iMinWB,m_iMaxWB));
    }

    void SetAutoBrightness(bool bAuto=true)
    {
        if (!m_pHandle) return;

        dc1394_auto_on_off(m_pHandle,m_CameraNode,FEATURE_BRIGHTNESS,bAuto);
    }

    void SetAutoGain(bool bAuto=true)
    {
        if (!m_pHandle) return;

        dc1394_auto_on_off(m_pHandle,m_CameraNode,FEATURE_GAIN,bAuto);
    }

    void SetAutoShutter(bool bAuto=true)
    {
        if (!m_pHandle) return;

        dc1394_auto_on_off(m_pHandle,m_CameraNode,FEATURE_SHUTTER,bAuto);
    }

    void SetAutoWhiteBalance(bool bAuto=true)
    {
        if (!m_pHandle) return;

        dc1394_auto_on_off(m_pHandle,m_CameraNode,FEATURE_WHITE_BALANCE,bAuto);
    }

    double GetBrightness()
    {
        unsigned int iVal;
        dc1394_get_brightness(m_pHandle,m_CameraNode,&iVal);
        return m_dBrightness=ValueToNorm(iVal,m_iMinBrightness,m_iMaxBrightness);
    }

    double GetGain()
    {
        unsigned int iVal;
        dc1394_get_gain(m_pHandle,m_CameraNode,&iVal);
        return m_dGain=ValueToNorm(iVal,m_iMinGain,m_iMaxGain);
    }

    double GetShutter()
    {
        unsigned int iVal;
        dc1394_get_shutter(m_pHandle,m_CameraNode,&iVal);
        return m_dShutter=ValueToNorm(iVal,m_iMinShutter,m_iMaxShutter);
    }

    void GetWhiteBalance(double &dUB,double& dVR)
    {
        unsigned int iUB,iVR;
        dc1394_get_white_balance(m_pHandle,m_CameraNode,&iUB,&iVR);
        m_dUB=dUB=ValueToNorm(iUB,m_iMinWB,m_iMaxWB);
        m_dVR=dVR=ValueToNorm(iVR,m_iMinWB,m_iMaxWB);
    }

    void PrintSettings()
    {
        double dUB,dVR;
        GetBrightness();
        GetGain();
        GetShutter();
        GetWhiteBalance(dUB,dVR);

        printf("Brightness = %lf\n",m_dBrightness);
        printf("Gain       = %lf\n",m_dGain);
        printf("Shutter    = %lf\n",m_dShutter);
        printf("White Bal  = %lf %lf\n",m_dUB,m_dVR);
        fflush(stdout);
    }

protected:
    raw1394handle_t m_pHandle;
    nodeid_t m_CameraNode;

    double m_dShutter,m_dGain,m_dBrightness,m_dUB,m_dVR;
    unsigned int m_iMinShutter,m_iMinGain,m_iMinBrightness,m_iMinWB;
    unsigned int m_iMaxShutter,m_iMaxGain,m_iMaxBrightness,m_iMaxWB;

    unsigned int NormToValue(double& dVal,unsigned int iMin,unsigned int iMax)
    {
        if (dVal<0.0) dVal=0.0;
        if (dVal>1.0) dVal=1.0;

        unsigned int iVal=iMin+(unsigned int)(dVal*double(iMax-iMin));

        if (iVal<iMin) iVal=iMin;
        if (iVal>iMax) iVal=iMax;

        return iVal;
    }

    double ValueToNorm(unsigned int iVal,unsigned int iMin,unsigned int iMax)
    {
        double dVal=double(iVal-iMin)/double(iMax-iMin);

        if (dVal<0.0) return 0.0;
        if (dVal>1.0) return 1.0;

        return dVal;
    }
};

#endif
