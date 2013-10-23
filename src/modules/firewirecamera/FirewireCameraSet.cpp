// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <unistd.h>
#include <stdio.h>
#include "FirewireCameraDC1394.h"
#include "FirewireCameraSet.h"

///////////////////////////////////////////////////////
// Init/Close functions
///////////////////////////////////////////////////////

bool CFWCameraSet::Init(int port)
{
    if (m_pHandle)
    {
        printf("ERROR: Firewire already open, shutdown first or restart\n");
        fflush(stdout);
        return false;
    }

    m_pHandle=dc1394_create_handle(m_iPort=port);

    if (!m_pHandle)
    {
        printf("Unable to aquire a raw1394 handle\n\n"
               "Please check:\n"
               "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
               "  - if you have read/write access to /dev/raw1394\n\n");

        fflush(stdout);

        return false;
    }

    m_nNumNodes=raw1394_get_nodecount(m_pHandle);
    m_pCameraNodes=dc1394_get_camera_nodes(m_pHandle,&m_nNumCameras,1); // 1 prints camera list
    fflush(stdout);

    if (m_nNumCameras<=0 || !m_pCameraNodes)
    {
        dc1394_destroy_handle(m_pHandle);
        printf("Error: no camera found!\n");
        fflush(stdout);
        return false;
    }

    bool bRoot=false;

    for (int c=0; c<m_nNumCameras; ++c)
    {
        if (m_pCameraNodes[c]==m_nNumCameras-1)
        {
            printf("WARNING: camera[%d] is ROOT node\n",c);
            fflush(stdout);
            bRoot=true;
        }
    }

    printf("Init Firewire OK\n");
    fflush(stdout);

    m_apCamera=new CFWCamera*[m_nNumCameras];

    for (int cam=0; cam<m_nNumCameras; ++cam) m_apCamera[cam]=0;

    return true;
}

void CFWCameraSet::Shutdown()
{
    if (!m_pHandle) return;

    if (m_apCamera)
    {
        for (int cam=0; cam<m_nNumCameras; ++cam)
            ShutdownCamera(cam);

        delete [] m_apCamera;
        m_apCamera=0;
    }

    dc1394_destroy_handle(m_pHandle);

    if (m_pCameraNodes)
    {
        dc1394_free_camera_nodes(m_pCameraNodes);
        m_pCameraNodes=0;
    }

    m_pHandle=0;
    m_nNumCameras=0;
    m_ActiveCams=0;
}

void CFWCameraSet::Restart(int port)
{
    Shutdown();
    Init(port);
}

bool CFWCameraSet::StartCamera(int idCamera,int xdim,int ydim,bool bDma)
{
    if (idCamera<0 || idCamera>=m_nNumCameras)
    {
        printf("Invalid camera number\n");
        fflush(stdout);
        return false;
    }

    if (m_apCamera[idCamera])
    {
        printf("ERROR: camera already started!\n");
        fflush(stdout);
        return false;
    }

    m_apCamera[idCamera]=new CFWCamera();

    bool bOk=m_apCamera[idCamera]->Create(m_pHandle,m_pCameraNodes[idCamera],idCamera,bDma,xdim,ydim,m_iPort);

    if (!bOk)
    {
        delete m_apCamera[idCamera];
        m_apCamera[idCamera]=NULL;

        printf("ERROR: can't start camera %d\n",idCamera);
        fflush(stdout);
        return false;
    }

    ++m_ActiveCams;

    return true;
}

void CFWCameraSet::ShutdownCamera(int idCamera)
{
    if (IsCameraReady(idCamera))
    {
        m_apCamera[idCamera]->Close();
        delete m_apCamera[idCamera];
        m_apCamera[idCamera]=NULL;
        --m_ActiveCams;
    }
}

void CFWCameraSet::GetCameraInfo(int idCamera)
{
    if (IsCameraReady(idCamera))
    {
        m_apCamera[idCamera]->GetCameraInfo();
    }
}

///////////////////////////////////////////////////////
// Capture functions
///////////////////////////////////////////////////////

bool CFWCameraSet::Capture(int idCamera,unsigned char* image)
{
    if (!IsCameraReady(idCamera)) return false;

    return m_apCamera[idCamera]->Capture(image);
}

bool CFWCameraSet::CaptureRaw(int idCamera,unsigned char* image)
{
    if (!IsCameraReady(idCamera)) return false;

    return m_apCamera[idCamera]->CaptureRaw(image);
}

////////////////////////////////////////////////////////////////////////
// camera controls
////////////////////////////////////////////////////////////////////////

bool CFWCameraSet::SetShutter(int camera,double dShutter)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetShutter(dShutter);

    return true;
}

bool CFWCameraSet::SetGain(int camera,double dGain)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetGain(dGain);

    return true;
}

bool CFWCameraSet::SetBrightness(int camera,double dBrightness)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetBrightness(dBrightness);

    return true;
}

bool CFWCameraSet::SetWhiteBalance(int camera,double dUB, double dVR)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetWhiteBalance(dUB,dVR);

    return true;
}

double CFWCameraSet::GetShutter(int camera)
{
    if (!IsCameraReady(camera)) return -1.0;

    return m_apCamera[camera]->settings.GetShutter();
}

double CFWCameraSet::GetGain(int camera)
{
    if (!IsCameraReady(camera)) return -1.0;

    return m_apCamera[camera]->settings.GetGain();
}

double CFWCameraSet::GetBrightness(int camera)
{
    if (!IsCameraReady(camera)) return -1.0;

    return m_apCamera[camera]->settings.GetBrightness();
}

bool CFWCameraSet::GetWhiteBalance(int camera,double& dUB, double& dVR)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.GetWhiteBalance(dUB,dVR);

    return true;
}

bool CFWCameraSet::SetAutoShutter(int camera,bool bAuto)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetAutoShutter(bAuto);

    return true;
}

bool CFWCameraSet::SetAutoGain(int camera,bool bAuto)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetAutoGain(bAuto);

    return true;
}

bool CFWCameraSet::SetAutoBrightness(int camera,bool bAuto)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetAutoBrightness(bAuto);

    return true;
}

bool CFWCameraSet::SetAutoWhiteBalance(int camera,bool bAuto)
{
    if (!IsCameraReady(camera)) return false;

    m_apCamera[camera]->settings.SetAutoWhiteBalance(bAuto);

    return true;
}
