// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __FIREWIRE_CAMERA_SET_H__
#define __FIREWIRE_CAMERA_SET_H__

extern "C"{
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
}

#include "FirewireCameraDC1394.h"

class CFWCameraSet
{
public:
    CFWCameraSet()
    {
        m_pHandle=NULL;
        m_pCameraNodes=NULL;
        m_nNumCameras=0;
        m_apCamera=NULL;
        m_iPort=0;
        m_ActiveCams=0;
    }

    virtual ~CFWCameraSet()
    {
        Shutdown();
    }

    ///////////////////////////////////////////////////////
    // Init/Close functions
    ///////////////////////////////////////////////////////

    bool Init(int port=0);
    void Shutdown();
    void Restart(int port=0);
    bool StartCamera(int idCamera,int xdim=XDIM,int ydim=YDIM,bool bDma=true);
    void ShutdownCamera(int idCamera);
    void GetCameraInfo(int idCamera);

    ///////////////////////////////////////////////////////
    // Capture functions
    ///////////////////////////////////////////////////////

    bool Capture(int idCamera,unsigned char* image);
    bool CaptureRaw(int idCamera,unsigned char* image);

    ///////////////////////////////////////////////////////
    // camera controls
    ///////////////////////////////////////////////////////

    bool SetShutter(int camera,double dShutter);
    bool SetGain(int camera,double dGain);
    bool SetBrightness(int camera,double dBrightness);
    bool SetWhiteBalance(int camera,double dUB, double dVR);

    double GetShutter(int camera);
    double GetGain(int camera);
    double GetBrightness(int camera);
    bool   GetWhiteBalance(int camera,double& dUB, double& dVR);

    bool SetAutoShutter(int camera,bool bAuto=true);
    bool SetAutoGain( int camera,bool bAuto=true);
    bool SetAutoBrightness(int camera,bool bAuto=true);
    bool SetAutoWhiteBalance( int camera,bool bAuto=true);

    int GetCameraNum()
    {
        return m_ActiveCams;
    }

    void PrintSettings(int idCamera)
    {
        if (!IsCameraReady(idCamera)) return;

        m_apCamera[idCamera]->settings.PrintSettings();
    }

protected:
    CFWCamera** m_apCamera;
    raw1394handle_t m_pHandle;
    nodeid_t* m_pCameraNodes;
    int m_iPort;

    int m_nNumNodes,m_nNumCameras;
    int m_ActiveCams;

    char dma_device_file[32];

    bool IsCameraReady(int idCamera)
    {
        return m_pHandle && idCamera>=0 && idCamera<m_nNumCameras && m_apCamera && m_apCamera[idCamera];
    }

    unsigned char* CaptureFrame(int camera);
};

#endif
