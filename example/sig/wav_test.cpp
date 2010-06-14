// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/sig/all.h>

int main(int argc, char *argv[]) {
    yarp::sig::Sound sound;
    yarp::sig::file::read(sound,"in.wav");
    yarp::sig::file::write(sound,"out.wav");
    return 0;
}
