/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

import yarp.Network;
import yarp.ImageRgb;
import yarp.Time;
import yarp.Property;
import yarp.BufferedPortImageRgb;
import yarp.PolyDriver;
import yarp.IFrameGrabberImage;

class Example3 {

    public static void main(String[] args) {
        System.loadLibrary("yarp_java");
        Network.init();

        // set up a description of the device we want
        // in this case, a fake framegrabber
        Property prop = new Property();
        prop.put("device","fakeFrameGrabber");
        prop.put("width", 640);
        prop.put("height", 480);
        System.out.println("Property is " + prop);

        // create the device
        PolyDriver dev = new PolyDriver(prop);

        // get the "IFrameGrabberImage" interface
        IFrameGrabberImage grabber = dev.viewFrameGrabberImage();

        // grab 30 images
        ImageRgb img = new ImageRgb();
        for (int i=0; i<30; i++) {
            grabber.getImage(img);
            int w = img.width();
            int h = img.height();
            System.out.println("Got a " + w + "x" + h + " image");
        }

        // shut things down
        dev.close();

        Network.fini();
    }
}
