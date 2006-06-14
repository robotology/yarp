// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>

#include <yarp/os/Time.h>

using namespace yarp::os;

int main() {
    printf("Hello...\n");
    Time::delay(1);
    printf("...world\n");
    return 0;
}
