/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

import yarp.Network;
import yarp.ImageRgb;
import yarp.Time;
import yarp.BufferedPortImageRgb;


class Example2 {

    public static void main(String[] args) {
        System.loadLibrary("yarp_java");
        Network.init();
        Network.connect("/image","/view");
        BufferedPortImageRgb p = new BufferedPortImageRgb();
        p.open("/image");
        // connect to a viewer called "/view" if present
        Network.connect("/image","/view");
        short ct = 0;
        while(true) {
            System.out.println("waiting for an image...");
            ImageRgb imgIn = p.read();
            if (imgIn!=null) {
                System.out.println("got a " + imgIn.width() + "x" +
                                   imgIn.height() + " image");
                ImageRgb imgOut = p.prepare();
                imgOut.copy(imgIn);
                for (int x=0; x<imgOut.width(); x++) {
                    for (int y=0; y<imgOut.height(); y++) {
                        // vary blue level
                        imgOut.pixel(x,y).setB(ct);
                    }
                }
                ct += 20;
                p.write();
                System.out.println("send a processed image");
                Time.delay(0.25); // Java isn't very fast, don't stress it
            }
        }
        //Network.fini(); // never reached
    }
}
