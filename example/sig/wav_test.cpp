/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/sig/all.h>

int main(int argc, char *argv[]) {
    yarp::sig::Sound sound;
    yarp::sig::file::read(sound,"in.wav");
    yarp::sig::file::write(sound,"out.wav");
    return 0;
}
