/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

service TestMultipleContainers
{
     bool testList1(1: list<i32> a);
     bool testList2(1: list<i32> a, 2: list<i32> b);

     bool testMap1(1: map<i32,string> a);
     bool testMap2(1: map<i32,string> a, 2: map<i32,string> b);

     bool testSet1(1: set<i32> a);
     bool testSet2(1: set<i32> a, 2: set<i32> b);
}
