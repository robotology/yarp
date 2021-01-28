#!/usr/bin/python3

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import yarp
import unittest

class VectorTest(unittest.TestCase):

    def test_vector_copy_costructor(self):
        vecSize = 10
        vec = yarp.Vector(vecSize)
        self.assertEqual(vec.size(), vecSize)
        vecCheck = yarp.Vector(vec)
        self.assertEqual(vec.size(), vecCheck.size())

        # Check copy constructor from a Vector
        # returned by a function
        mat = yarp.Matrix(3,3)
        mat.eye()
        vecTest = yarp.Vector(mat.getRow(0))
        self.assertEqual(vecTest.size(), 3)
        # Initializer list constructor
        vec2 = yarp.Vector([1.0, 2.0, 3.0])
        self.assertEqual(vec2.size(), 3)
        self.assertEqual(vec2.get(0), 1.0)
        self.assertEqual(vec2.get(1), 2.0)
        self.assertEqual(vec2.get(2), 3.0)


if __name__ == '__main__':
    unittest.main()
