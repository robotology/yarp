/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

using System;

// I don't really know C#, just bluffing from Java

namespace HelloNameSpace
{
    public class TestStamp
    {
        static void Main(string[] args)
        {
            Network.init();
            BufferedPortBottle p = new BufferedPortBottle();
            if (!p.open("/csharp")) System.Environment.Exit(1);
            Stamp ts = new Stamp();
            p.setEnvelope(ts);
            p.close();
            Network.fini();
        }
    }
}
