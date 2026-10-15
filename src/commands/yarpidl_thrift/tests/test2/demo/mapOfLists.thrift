/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct MapOfLists
{
    1: map<string, list<double>> vectors;
    2: map<string, map<string, list<double>>> nestedVectors;
    3: map<string, list<map<string, list<double>>>> mapOfVectors;
}
