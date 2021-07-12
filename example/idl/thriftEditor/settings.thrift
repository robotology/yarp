/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct Point {
       1: i32 x;
       2: i32 y;
}

struct Settings {
       1: i32 id;
       2: string name;
       3: Point pt;
       4: list<i32> ints;
}
