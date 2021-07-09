/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

import yarp.Network;
import yarp.Time;
import yarp.Property;
import yarp.PolyDriver;
import yarp.IPositionControl;
import yarp.IEncoders;
import yarp.DVector;

class Example4 {

    public static void main(String[] args) {
        System.loadLibrary("yarp_java");
        Network yarp = new Network();

        // set up a description of the device we want
        // in this case, a remote motor control board
        Property prop = new Property();
        prop.put("device","remote_controlboard");
        prop.put("local", "/example/motor/client");
        prop.put("remote", "/icubSim/head");  // this is for the icubSim
        // or fake it with "yarpdev --device fakeMotionControl --name /icubSim/head"
        System.out.println("Property is " + prop);

        // create the device
        PolyDriver dev = new PolyDriver(prop);

        // get the "IPositionControl" interface
        IPositionControl pos = dev.viewIPositionControl();
        IEncoders enc = dev.viewIEncoders();

        float at = 0;
        DVector v = new DVector(enc.getAxes());
        for (int i=0; i<10; i++) {
            pos.positionMove(0,at);
            System.out.println("Set axis 0 to " + at + " degrees");
            Time.delay(1);
            enc.getEncoders(v);
            System.out.println("Encoder at " + v.get(0));
            at += 5;
        }

        // shut things down
        dev.close();
    }
}
