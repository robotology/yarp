/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

using System;

// I don't really know C#, just bluffing from Java

namespace HelloNameSpace
{
    public class TestString
    {
        static void Main(string[] args)
        {
            Network.init();
            BufferedPortBottle p = new BufferedPortBottle();
            if (!p.open("/csharp")) System.Environment.Exit(1);
            p.close();
            Network.fini();
        }
    }
}
