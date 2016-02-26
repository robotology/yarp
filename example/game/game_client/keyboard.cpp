/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include <yarp/os/all.h>
using namespace yarp::os;
using namespace yarp;

#include "simio.h"
#include "keyboard.h"

static String dir = "right";
static String msg = "";
static int mode = 0;
static Semaphore mutex(1);

String getPreparation() {
    mutex.wait();
    String result = msg;
    mutex.post();
    return result;
}

String getCommand() {
    String cmd = "";

    if (mode==-1) {
        while (kbhit()) {
            waitkey();
        }
        mode = 0;
        return cmd;
    }

    int key = waitkey();
    switch (key) {
    case 'q':
        if (mode==0) {
            cmd = "quit";
        }
        break;
    case 224:
        if (mode==0) mode=2;
        else mode = 0;
        break;
    case 27:
        if (mode==0) mode++;
        else mode = 0;
        break;
    case 91:
        if (mode==1) mode++;
        else mode = 0;
        break;
    case 77:
    case 67:
        if (mode==2) {
            dir = "right";
            cmd = "go right";
        }
        mode = 0;
        break;
    case 75:
    case 68:
        if (mode==2) {
            dir = "left";
            cmd = "go left";
        }
        mode = 0;
        break;
    case 65:
    case 72:
        if (mode==2) {
            dir = "up";
            cmd = "go up";
        }
        mode = 0;
        break;
    case 80:
    case 66:
        if (mode==2) {
            dir = "down";
            cmd = "go down";
        }
        mode = 0;
        break;
    case 32:
        if (mode==0) {
            if (msg.length()==0) {
                cmd = "fire ";
                cmd += dir;
            } else {
                msg += " ";
            }
        }
        mode = 0;
        break;
    case 10:
    case 13:
        cmd = "say ";
        cmd += msg;
        msg = "";
        mode = 0;
        break;
    case 127:
    case 8:
        if (mode==0) {
            msg = msg.substr(0,msg.length()-1);
            mode = 0;
        }
        break;
    default:
        //cprintf("KEY is %d\n", key);
        if (mode==0) {
            mutex.wait();
            if (key>=32 && key<=126) {
                msg += ((char)key);
            }
            mutex.post();
        }
        mode = 0;
        break;
    }

    if (cmd!="") {
        mode = -1;
    }

    return cmd;
}




