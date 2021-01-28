/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
