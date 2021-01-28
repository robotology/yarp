#!/usr/bin/python3

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import yarp
import unittest


yarp.Network.init()
yarp.Network.setLocalMode(True)


class BufferedPortTest(unittest.TestCase):

    def test_open_and_connect(self):
        p = yarp.BufferedPortBottle()
        p2 = yarp.BufferedPortBottle()
        self.assertTrue(p.open("/python/out"))
        self.assertTrue(p2.open("/python/in"))
        self.assertTrue(yarp.Network.connect(p.getName(),p2.getName()))
        p.close()
        p2.close()

    def test_open_and_connect_comm(self):
        p = yarp.BufferedPortBottle()
        p2 = yarp.BufferedPortBottle()
        bt_out = p.prepare()
        bt_out.addInt32(10)
        self.assertTrue(p.open("/python/out"))
        self.assertTrue(p2.open("/python/in"))
        self.assertTrue(yarp.Network.connect(p.getName(),p2.getName()))
        yarp.delay(0.5)
        p.write()
        yarp.delay(0.5)
        bt_in=p2.read()
        self.assertEqual(1, bt_in.size())
        self.assertEqual(10, bt_in.get(0).asInt32())
        p.close()
        p2.close()

if __name__ == '__main__':
    unittest.main()
