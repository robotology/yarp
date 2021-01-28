/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::sig::Sound sound;
    yarp::sig::file::read(sound, "in.wav");
    yarp::sig::file::write(sound, "out.wav");
    return 0;
}
