/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>

#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

int main(int argc, char* argv[])
{
    std::string filename;
    if (argc < 2) {
        yInfo() << "Syntax: send_sound_file <audiofile>";
        return 1;
    }

    filename = argv[1];

    yarp::sig::Sound audioFile;
    bool ret = yarp::sig::file::read(audioFile, filename.c_str());
    if (ret == false) {
        yError() << "Unable to open file" << filename.c_str();
        return 1;
    }

    yarp::os::Network yarp;

    yarp::os::Port p;
    p.open("/sound:o");
    yInfo() << "Port opened..";
    yarp::os::Time::delay(3);

    yInfo() << "Sending audio data..";
    p.write(audioFile);
    yarp::os::Time::delay(1);
    yInfo() << "..data sent!";

    return 0;
}
