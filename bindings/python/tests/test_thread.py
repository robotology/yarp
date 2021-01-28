#!/usr/bin/python3

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import yarp
import unittest
import time
import threading

yarp.Network.init()
yarp.Network.setLocalMode(True)

foo = 0
lock = threading.RLock()

class MyThread(yarp.Thread):
    def run(self):
        global foo
        time.sleep(1)
        with lock:
            foo = 1

class ThreadTest(unittest.TestCase):
    def test_thread(self):
        global foo
        x = MyThread()

        with lock:
            self.assertEqual(foo, 0)

        x.start()

        with lock:
            self.assertEqual(foo, 0)

        x.join()

        with lock:
            self.assertEqual(foo, 1)

if __name__ == '__main__':
    unittest.main()
