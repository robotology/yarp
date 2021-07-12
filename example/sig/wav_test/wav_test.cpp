/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
