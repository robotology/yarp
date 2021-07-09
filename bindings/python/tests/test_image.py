#!/usr/bin/python3

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp
import unittest

yarp.Network.init()
yarp.Network.setLocalMode(True)


class ImageTest(unittest.TestCase):

    def test_send_and_recv_setExt(self):
        port_in = yarp.Port()
        in_port_name = '/test/img:i'
        self.assertTrue(port_in.open(in_port_name))
        port_out = yarp.Port()
        port_out.enableBackgroundWrite(True);
        out_port_name = '/test/img:o'
        self.assertTrue(port_out.open(out_port_name))
        self.assertTrue(yarp.Network.connect(port_out.getName(), port_in.getName()))
        yarp.delay(0.5)
        height = 240
        width = 320
        yarp_img_out = yarp.ImageMono()
        yarp_img_out.resize(width, height)
        yarp_img_in = yarp.ImageMono()
        self.assertEqual(240, yarp_img_out.height())
        self.assertEqual(320, yarp_img_out.width())
        yarp_vector = yarp.Vector()
        yarp_vector.resize(320*240, 0)
        self.assertTrue(240*320, yarp_vector.size())
        yarp_img_out.setExternal(yarp_vector, width, height)
        self.assertTrue(port_out.write(yarp_img_out))
        yarp.delay(0.5)
        self.assertTrue(port_in.read(yarp_img_in))
        self.assertEqual(240, yarp_img_in.height())
        self.assertEqual(320, yarp_img_in.width())

        port_out.close()
        port_in.close()


if __name__ == '__main__':
    unittest.main()
