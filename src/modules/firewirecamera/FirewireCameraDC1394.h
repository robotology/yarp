// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __FIREWIRE_CAMERA_H__
#define __FIREWIRE_CAMERA_H__

#include <stdio.h>
#include <memory.h>

extern "C"{
#include <libdc1394/dc1394_control.h>
}

#include "dc1394settings.h"

#define USED_ACQ_MODE MODE_640x480_MONO
// the half, because of bayer mosaic structure
#define XDIM 320
#define YDIM 240

#define FW_UNINIT 0
#define FW_NORMAL 1
#define FW_HALF   2
#define FW_CUSTOM 3

class CFWCamera
{
public:
    CDC1394Settings settings;

    CFWCamera() : settings()
    {
        m_aXinc=m_aYinc=NULL;
        m_Xdim=m_Ydim=0;
        m_PlaneSize=0;
        m_RowLen=0;
        m_AcqMode=FW_UNINIT;

        m_pHandle=NULL;
        m_CameraNode=0;
        m_CameraId=-1;
        m_iPort=0;
        m_bDma=true;
    }

    bool Create(raw1394handle_t pHandle,nodeid_t CameraNode,int CameraId,bool bDma=true,int xdim=XDIM,int ydim=YDIM,int iPort=0)
    {
        if (m_AcqMode!=FW_UNINIT)
        {
            printf("Camera %d already open\n",CameraId);
            fflush(stdout);
            return false;
        }

        settings.Create(pHandle,CameraNode);

        m_pHandle=pHandle;
        m_CameraNode=CameraNode;
        m_CameraId=CameraId;
        m_bDma=bDma;
        m_Xdim=xdim;
        m_Ydim=ydim;
        m_PlaneSize=m_Xdim*m_Ydim;
        m_RowLen=XDIM*2;
        m_iPort=iPort;

        if (m_bDma)
        {
            sprintf(dma_device_file,"/dev/video1394/%d",m_iPort);

            if (access(dma_device_file,R_OK)) // !=0 == error
            {
                sprintf(dma_device_file,"/dev/video1394");

                if (access(dma_device_file,R_OK)) // !=0 == error
                {
                    printf("ERROR: failed to open device file /dev/video1394\n\n"
                           "       Have you setup /dev/video1394 as root?\n\n"
                           "       mkdir /dev/video1394\n"
                           "       mknod -m 666 /dev/video1394/0 c 171 16\n\n");
                    fflush(stdout);

                    return false;
                }
            }

            int num_dma_buffers=4;
            int drop_frames=1;
            //int do_extra_buffering = 0; //1 introduces high latency when it's turned on

            if (dc1394_dma_setup_capture(m_pHandle,
                                         m_CameraNode,
                                         m_CameraId,
                                         FORMAT_VGA_NONCOMPRESSED,
                                         USED_ACQ_MODE,
                                         SPEED_400,
                                         FRAMERATE_30,
                                         num_dma_buffers,
                                         //do_extra_buffering, //removed for libdc1394-11-dev
                                         drop_frames,
                                         dma_device_file,
                                         &capture)
                != DC1394_SUCCESS)
            {
                    printf("unable to setup camera %d\n"
                           "  - the camera may already be in use\n"
                           "  - the requested video mode, framerate or format,\n"
                           "as specified on line %d of file %s \n"
                           "may not be supported by the camera\n\n",m_CameraId,__LINE__,__FILE__);

                 //this command seems to seg fault here, not having it may lead to a memory leak
                 //dc1394_release_camera(m_pHandle,&(m_aCameras[idCamera].capture));

                 fflush(stdout);
                 return false;
            }
        }
        else
        {
            if (dc1394_setup_capture(m_pHandle,
                                     m_CameraNode,
                                     m_CameraId,
                                     FORMAT_VGA_NONCOMPRESSED,
                                     USED_ACQ_MODE,
                                     SPEED_400,
                                     FRAMERATE_30,
                                     &capture)
                !=DC1394_SUCCESS)
            {
                printf("unable to setup camera %d\n"
                           "  - the camera may already be in use\n"
                           "  - the requested video mode, framerate or format,\n"
                           "as specified on line %d of file %s \n"
                           "may not be supported by the camera\n\n",m_CameraId,__LINE__,__FILE__);

                //this command seems to seg fault here, not having it may lead to a memory leak
                //dc1394_release_camera(m_pHandle,&(m_aCameras[idCamera].capture));

                fflush(stdout);
                return false;
            }
        }

        // Start isochronous data transmission
        if (dc1394_start_iso_transmission(m_pHandle,capture.node) != DC1394_SUCCESS)
        {
            printf("ERROR unable to start camera %d iso transmission\n",m_CameraId);
            fflush(stdout);

            if (m_pHandle) dc1394_release_camera(m_pHandle,&capture);

            return false;
        }

        if (xdim==XDIM && ydim==YDIM)
        {
            m_aXinc=m_aYinc=NULL;
            m_AcqMode=FW_NORMAL;
        }
        else if (xdim<<1==XDIM && ydim<<1==YDIM)
        {
            m_aXinc=m_aYinc=NULL;
            m_AcqMode=FW_HALF;
        }
        else
        {
            m_aXinc=new int[xdim];
            m_aYinc=new int[ydim];
            m_AcqMode=FW_CUSTOM;

            double dmx=double(XDIM<<1)/double(xdim);

            for (int x=0; x<xdim; ++x)
                if ((m_aXinc[x]=int(double(x)*dmx)) & 0x01) --m_aXinc[x];

            for (int x=0; x<xdim-1; ++x)
                m_aXinc[x]=m_aXinc[x+1]-m_aXinc[x];

            double dmy=double(YDIM<<1)/double(ydim);

            for (int y=0; y<ydim; ++y)
                if ((m_aYinc[y]=int(double(y)*dmy)) & 0x01) --m_aYinc[y];

            for (int y=0; y<ydim-1; ++y)
                m_aYinc[y]=m_aYinc[y+1]-m_aYinc[y];
        }

        return true;
    }

    bool Close()
    {
        if (m_AcqMode==FW_UNINIT) return false;

        m_AcqMode=FW_UNINIT;

        dc1394_stop_iso_transmission(m_pHandle,capture.node);

        if (m_bDma)
        {
            dc1394_dma_unlisten(m_pHandle,&capture);
            dc1394_dma_release_camera(m_pHandle,&capture);
        }
        else
        {
            dc1394_release_camera(m_pHandle,&capture);
        }

        if (m_aXinc) delete [] m_aXinc;
        if (m_aYinc) delete [] m_aYinc;

        m_aXinc=m_aYinc=NULL;

        return true;
    }

    ~CFWCamera()
    {
        Close();
    }

    void GetCameraInfo()
    {
        if (m_AcqMode==FW_UNINIT) return;

        if (!dc1394_get_camera_misc_info(m_pHandle,m_CameraNode,&misc_info) ||
            !dc1394_get_camera_info(m_pHandle,m_CameraNode,&camera_info) ||
            !dc1394_get_camera_feature_set(m_pHandle,camera_info.id,&feature_set))
        {
            printf("Could not get camera basic informations!\n");
            fflush(stdout);
            return;
        }

        printf("\n\n");
        dc1394_print_feature_set(&feature_set);
        printf("\n\n");

        unsigned int channel,speed;

        if (dc1394_get_iso_channel_and_speed(m_pHandle,camera_info.id,&channel,&speed)!=DC1394_SUCCESS)
            printf("Can't get iso channel and speed\n");

        if (dc1394_set_iso_channel_and_speed(m_pHandle,camera_info.id,m_CameraId,speed)!=DC1394_SUCCESS)
            printf("Can't set iso channel and speed\n");

        fflush(stdout);
    }

    bool Capture(unsigned char* image)
    {
        switch (m_AcqMode)
        {
        case FW_UNINIT:
            return false;

        case FW_NORMAL:
            return CaptureZoom100(image);

        case FW_HALF:
            return CaptureZoom50(image);

        case FW_CUSTOM:
            return CaptureCustomZoom(image);
        }

        return false;
    }

    bool CaptureRaw(unsigned char* image)
    {
        unsigned char* buffer=CaptureFrame();
        if (!buffer) return false;

        memcpy(image,buffer,m_PlaneSize<<2);

        if (m_bDma) dc1394_dma_done_with_buffer(&capture);

        return true;
    }

protected:
    dc1394_camerainfo camera_info;
    dc1394_feature_set feature_set;
    dc1394_miscinfo misc_info;
    dc1394_cameracapture capture;

    int *m_aXinc,*m_aYinc;
    int m_Xdim,m_Ydim;
    int m_RowLen,m_PlaneSize;
    int m_AcqMode;

    char dma_device_file[64];
    raw1394handle_t m_pHandle;
    nodeid_t m_CameraNode;
    int m_CameraId;
    int m_iPort;
    bool m_bDma;

    bool CaptureZoom100(unsigned char* image)
    {
        if (m_AcqMode!=FW_NORMAL) return false;

        unsigned char *pR=CaptureFrame(),*pB=pR+m_RowLen;

        if (!pR) return false;

        int x;

        for (int y=0; y<m_Ydim; ++y)
        {
            for (x=0; x<m_Xdim; ++x)
            {
                *image++=*pR++;
                *image++=(int(*pR++)+int(*pB++))>>1;
                *image++=*pB++;
            }

            pR+=m_RowLen;
            pB+=m_RowLen;
        }

        if (m_bDma) dc1394_dma_done_with_buffer(&capture);

        return true;
    }

    bool CaptureZoom50(unsigned char* image)
    {
        if (m_AcqMode!=FW_HALF) return false;

        unsigned char* buffer=CaptureFrame();

        if (!buffer) return false;

        int _3_row_len=3*m_RowLen;

        unsigned char *pRin1=buffer,*pRin2=pRin1+2,*pRin3=pRin1+2*m_RowLen,*pRin4=pRin2+2*m_RowLen;
        unsigned char *pBin1=pRin1+m_RowLen,*pBin2=pBin1+2,*pBin3=pBin1+2*m_RowLen,*pBin4=pBin2+2*m_RowLen;

        for (int y=0; y<m_Ydim; ++y)
        {
            for (int x=0; x<m_Xdim; ++x)
            {
                *image++=(int(*pRin1++)+int(*pRin2++)+int(*pRin3++)+int(*pRin4++))>>2; //red
                *image++=(int(*pRin1++)+int(*pRin2++)+int(*pRin3++)+int(*pRin4++)
                         +int(*pBin1++)+int(*pBin2++)+int(*pBin3++)+int(*pBin4++))>>3; //green
                *image++=(int(*pBin1++)+int(*pBin2++)+int(*pBin3++)+int(*pBin4++))>>2; //blue

                pRin1+=2; pRin2+=2; pRin3+=2; pRin4+=2;
                pBin1+=2; pBin2+=2; pBin3+=2; pBin4+=2;
            }

            pRin1+=_3_row_len;
            pRin2+=_3_row_len;
            pRin3+=_3_row_len;
            pRin4+=_3_row_len;
            pBin1+=_3_row_len;
            pBin2+=_3_row_len;
            pBin3+=_3_row_len;
            pBin4+=_3_row_len;
        }

        if (m_bDma) dc1394_dma_done_with_buffer(&capture);

        return true;
    }

    bool CaptureCustomZoom(unsigned char *image)
    {
        if (m_AcqMode!=FW_CUSTOM) return false;

        unsigned char* buffer=CaptureFrame();

        if (!buffer) return false;

        int row_len=XDIM<<1;
        int col_len=YDIM<<1;

        double dmy=double(col_len)/double(m_Ydim);

        int inc,yin;

        unsigned char *pRin,*pGin1,*pGin2,*pBin;

        for (int y=0; y<m_Ydim; ++y)
        {
            if ((yin=int(double(y)*dmy)) & 0x01) --yin;

            pRin=buffer+yin*row_len;
            pGin1=pRin+1;
            pGin2=pRin+row_len;
            pBin=pGin2+1;

            for (int x=0; x<m_Xdim; ++x)
            {
                *image++=*pRin; //red
                *image++=(int(*pGin1)+int(*pGin2))>>1; //green
                *image++=*pBin; //blue

                pRin+=(inc=m_aXinc[x]); pBin+=inc; pGin1+=inc; pGin2+=inc;
            }
        }

        if (m_bDma) dc1394_dma_done_with_buffer(&capture);

        return true;
    }

    unsigned char* CaptureFrame()
    {
        if (m_bDma)
        {
            if (dc1394_dma_single_capture(&capture) != DC1394_SUCCESS)
            {
                printf("ERROR: dma capture failed on camera %d shutdown firewire\n",m_CameraId);
                fflush(stdout);
                return NULL;
            }
        }
        else if (dc1394_single_capture(m_pHandle,&capture) != DC1394_SUCCESS)
        {
            printf("ERROR: capture failed on camera %d shutdown firewire\n",m_CameraId);
            fflush(stdout);
            return NULL;
        }

        return (unsigned char *)(capture.capture_buffer);
    }
};

#endif
