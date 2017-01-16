/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/ConstString.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IVisualParams.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::dev;
using namespace yarp::sig;



class TestFrameGrabberTest : public UnitTest
{
public:
    virtual ConstString getName() { return "TestFrameGrabberTest"; }

    void testTestFrameGrabber() {
        report(0,"\ntest the TestFrameGrabber ");
        yInfo() << "Test 1";
        //First test: I try to open a TestFrameGrabber I check its existence,
        //I try to take an image and I check the relative port and rpc port
        {
            PolyDriver dd;
            Property p;
            p.put("device","grabber");
            p.put("subdevice","test_grabber");
            bool result;
            result = dd.open(p);
            checkTrue(result,"open reported successful");
            IFrameGrabberImage *grabber = NULL;
            result = dd.view(grabber);
            checkTrue(result,"interface reported");
            ImageOf<PixelRgb> img;
            grabber->getImage(img);
            checkTrue(img.width()>0,"interface seems functional");
            result = dd.close();
            checkTrue(result,"close reported successful");
        }
        yDebug("\n\n---------------------------------------\n\n");

        yInfo() << "Test 2";
        {
            report(0,"test the grabber wrapper");
            bool result;
            PolyDriver dd,dd2;
            Property p,p2;
            p.put("device","remote_grabber");
            p.put("remote","/grabber");
            p.put("local","/grabber/client");
            p2.put("device","grabber");
            p2.put("subdevice","test_grabber");
            result =dd2.open(p2);
            checkTrue(result,"server open reported successful");
            result = dd.open(p);
            checkTrue(result,"client open reported successful");

            IFrameGrabberImage *grabber = NULL;
            result = dd.view(grabber);
            checkTrue(result,"interface reported");
            ImageOf<PixelRgb> img;
            grabber->getImage(img);
            checkTrue(img.width()>0,"interface seems functional");
            result = dd.close() && dd2.close();
            checkTrue(result,"close reported successful");
        }


        yDebug("\n\n---------------------------------------\n\n");

        yInfo() << "Test 3";
        //Second test: I try to open a TestFrameGrabber and I check all the parameters
        {
            PolyDriver dd;
            Property p,intrinsics;
            Bottle* retM=0;
            p.put("device","test_grabber");
            bool result;
            result = dd.open(p);
            checkTrue(result,"open reported successful");
            IRgbVisualParams* rgbParams=NULL;
            result= dd.view(rgbParams);
            checkTrue(result,"interface rgb params reported");
            // controllo che ci siano i parametri di default
            double hfov=0.0,vfov=0.0;
            rgbParams->getRgbFOV(hfov,vfov);
            result=(hfov==1.0 && vfov==2.0);
            checkTrue(result,"checking fov");
            result=rgbParams->getRgbHeight()== 240;
            checkTrue(result,"checking height");
            result=rgbParams->getRgbWidth()== 320;
            checkTrue(result,"checking width");
            rgbParams->getRgbMirroring(result);
            checkFalse(result,"checking mirroring");
            rgbParams->getRgbIntrinsicParam(intrinsics);
            result=(intrinsics.find("focalLengthX").asDouble())==4.0;
            checkTrue(result,"checking focalLength X");
            result=(intrinsics.find("focalLengthY").asDouble())==5.0;
            checkTrue(result,"checking focalLength Y");
            result=(intrinsics.find("principalPointX").asDouble())==6.0;
            checkTrue(result,"checking principalPoint X");
            result=(intrinsics.find("principalPointY").asDouble())==7.0;
            checkTrue(result,"checking principalPoint Y");
            result=(intrinsics.find("k1").asDouble())==8.0;
            checkTrue(result,"checking k1");
            result=(intrinsics.find("k2").asDouble())==9.0;
            checkTrue(result,"checking k2");
            result=(intrinsics.find("k3").asDouble())==10.0;
            checkTrue(result,"checking k3");
            result=(intrinsics.find("t1").asDouble())==11.0;
            checkTrue(result,"checking t1");
            result=(intrinsics.find("t2").asDouble())==12.0;
            checkTrue(result,"checking t2");
            result=(intrinsics.find("distortionModel").asString())=="FishEye";
            checkTrue(result,"checking distorionModel");
            retM=intrinsics.find("retificationMatrix").asList();
            double data[9]= {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
            Vector v(9,data);
            Vector v2;
            Portable::copyPortable(*retM,v2);
            result=true;
            for(int i=0; i<3; i++)
            {
                for(int j=0; j<3; j++)
                {
                    if(retM->get(i*3+j).asDouble()!=v(i*3+j) || v2(i*3+j)!=v(i*3+j))
                        result=false;
                }
            }
            checkTrue(result,"checking the retificationMatrix");
            result = dd.close();
            checkTrue(result,"close reported successful");
            //yDebug()<<b.asList()->get(0).asDouble();
        }

    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testTestFrameGrabber();
        Network::setLocalMode(false);
    }
};

static TestFrameGrabberTest theTestFrameGrabberTest;

UnitTest& getTestFrameGrabberTest() {
    return theTestFrameGrabberTest;
}
