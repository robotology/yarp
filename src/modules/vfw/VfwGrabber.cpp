/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// based on material from cvcap_vfw.cpp from the OpenCV library.
// changes are under the GPL.  Statement for original material follows.


/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#include <windows.h>
#include <VfwGrabber.h>
using namespace yarp::dev;

#include <stdio.h>

#include <vfw.h>

// MINGW headers are incomplete at time of writing
#ifdef __MINGW__
#include <vfw_extra_from_wine.h>
#include <vfw_extra_from_australia.h>
#include <vfw_extra_from_paulfitz.h>
#ifndef True
#define True TRUE
#endif
#endif

#include <yarp/sig/Image.h>

typedef yarp::sig::ImageOf<yarp::sig::PixelBgr> Image;


typedef struct CvCaptureCAM_VFW
{
    CAPDRIVERCAPS caps;
    HWND   capWnd;
    VIDEOHDR* hdr;
    DWORD  fourcc;
    HIC    hic;
    Image frame;
}
CvCaptureCAM_VFW;

static LRESULT PASCAL FrameCallbackProc( HWND hWnd, VIDEOHDR* hdr ) 
{ 
    CvCaptureCAM_VFW* capture = 0;

    if (!hWnd) return FALSE;

    capture = (CvCaptureCAM_VFW*)capGetUserData(hWnd);
    capture->hdr = hdr;

    printf("Frame arrived!\n");

    //hdr->lpData;
    //hdr->dwBytesUsed;

    return (LRESULT)TRUE; 
} 


// Initialize camera input
static int icvOpenCAM_VFW( CvCaptureCAM_VFW* capture, int wIndex )
{
    char szDeviceName[80];
    char szDeviceVersion[80];
    HWND hWndC = 0;
    
    if( (unsigned)wIndex >= 10 )
        wIndex = 0;

    for( ; wIndex < 10; wIndex++ ) 
        {
            if( capGetDriverDescription( wIndex, szDeviceName, 
                                         sizeof (szDeviceName), szDeviceVersion, 
                                         sizeof (szDeviceVersion))) 
                {
                    printf("Possible input: %s\n", szDeviceName);
                    hWndC = capCreateCaptureWindow ( "My Own Capture Window", 
                                                     WS_POPUP | WS_CHILD, 0, 0, 320, 240, 0, 0);
                    if( capDriverConnect (hWndC, wIndex))
                        break;
                    DestroyWindow( hWndC );
                    hWndC = 0;
                }
        }
    
    capture->capWnd = 0;
    if( hWndC )
        {
            printf("got a window\n");
            capture->capWnd = hWndC;
            capture->hdr = 0;
            capture->hic = 0;
            capture->fourcc = (DWORD)-1;
        
            memset( &capture->caps, 0, sizeof(capture->caps));
            capDriverGetCaps( hWndC, &capture->caps, sizeof(&capture->caps));
            ::MoveWindow( hWndC, 0, 0, 320, 240, TRUE );
            capSetUserData( hWndC, (size_t)capture );
            capSetCallbackOnFrame( hWndC, FrameCallbackProc ); 
            CAPTUREPARMS p;
            capCaptureGetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
            p.dwRequestMicroSecPerFrame = 66667/2;
            capCaptureSetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
            //capPreview( hWndC, 1 );
            capPreviewScale(hWndC,FALSE);
            capPreviewRate(hWndC,1);
        }
    return capture->capWnd != 0;
}

static  void icvCloseCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture && capture->capWnd )
        {
            capSetCallbackOnFrame( capture->capWnd, NULL ); 
            capDriverDisconnect( capture->capWnd );
            DestroyWindow( capture->capWnd );
            if( capture->hic )
                {
                    ICDecompressEnd( capture->hic );
                    ICClose( capture->hic );
                }

            capture->capWnd = 0;
            capture->hic = 0;
            capture->hdr = 0;
            capture->fourcc = 0;
        }
}


static int icvGrabFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture->capWnd )
        {
            SendMessage( capture->capWnd, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0 );
            return 1;
        }
    return 0;
}


static BITMAPINFOHEADER icvBitmapHeader( int width, int height, int bpp, int compression = BI_RGB )
{
    BITMAPINFOHEADER bmih;
    memset( &bmih, 0, sizeof(bmih));
    bmih.biSize = sizeof(bmih);
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biBitCount = (WORD)bpp;
    bmih.biCompression = compression;
    bmih.biPlanes = 1;

    return bmih;
}

static Image* icvRetrieveFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
    bool done = false;
    if( capture->capWnd )
        {
            BITMAPINFO vfmt;
            memset( &vfmt, 0, sizeof(vfmt));
            int sz = capGetVideoFormat( capture->capWnd, &vfmt, sizeof(vfmt));

            if( capture->hdr && capture->hdr->lpData && sz != 0 )
                {
                    long code = ICERR_OK;
                    char* frame_data = (char*)capture->hdr->lpData;

                    if( vfmt.bmiHeader.biCompression != BI_RGB ||
                        vfmt.bmiHeader.biBitCount != 24 )
                        {
                            printf("funky format\n");
                            BITMAPINFOHEADER& vfmt0 = vfmt.bmiHeader;
                            BITMAPINFOHEADER vfmt1 = icvBitmapHeader( vfmt0.biWidth, vfmt0.biHeight, 24 );
                            code = ICERR_ERROR;

                            if( capture->hic == 0 ||
                                capture->fourcc != vfmt0.biCompression ||
                                vfmt0.biWidth != capture->frame.width() ||
                                vfmt0.biHeight != capture->frame.height() )
                                {
                                    if( capture->hic )
                                        {
                                            ICDecompressEnd( capture->hic );
                                            ICClose( capture->hic );
                                        }
                                    capture->hic = ICOpen( MAKEFOURCC('V','I','D','C'),
                                                           vfmt0.biCompression, ICMODE_DECOMPRESS );
                                    if( capture->hic &&
                                        ICDecompressBegin( capture->hic, &vfmt0, &vfmt1 ) == ICERR_OK )
                                        {

                                            capture->frame.setTopIsLowIndex(false);
                                            capture->frame.resize(vfmt0.biWidth, vfmt0.biHeight);

                                            code = ICDecompress( capture->hic, 0,
                                                                 &vfmt0, capture->hdr->lpData,
                                                                 &vfmt1, capture->frame.getRawImage() );
                                            if (code == ICERR_OK) {
                                                done = true;
                                            }

                                        }
                                }
                        }
        
                    if( code == ICERR_OK )
                        {
                            if (!done) {

                                capture->frame.setTopIsLowIndex(false);
                                capture->frame.setExternal(frame_data,
                                                           vfmt.bmiHeader.biWidth,
                                                           vfmt.bmiHeader.biHeight);
                            }
                            return &capture->frame;
                        }
                }
        }

    return 0;
}



#define HELPER(x) (*((CvCaptureCAM_VFW *)(x)))

bool VfwGrabber::open(yarp::os::Searchable& config) {
    system_resource = new CvCaptureCAM_VFW;
    if (system_resource!=NULL) {
        int index = config.check("index",
                                 yarp::os::Value(0),
                                 "VFW device index").asInt();
        int result = icvOpenCAM_VFW(&HELPER(system_resource),index);
        if (!result) {
            printf("failed to find camera\n");
            close();
        }
    }
    return system_resource!=NULL;
}

bool VfwGrabber::close() {
    if (system_resource!=NULL) {
        icvCloseCAM_VFW(&HELPER(system_resource));
        delete &HELPER(system_resource);
        system_resource = NULL;
    }
    return true;
}

bool VfwGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
    icvGrabFrameCAM_VFW(&HELPER(system_resource));
    Image *img = icvRetrieveFrameCAM_VFW(&HELPER(system_resource));
    //printf("image size %d %d\n", img->width(), img->height());
    image.copy(*img);
    _width = img->width();
    _height = img->height();
    return img->width()>0;
}
