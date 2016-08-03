/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/NetType.h>
#include <stdio.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define MAX_PACKET 100

class DgramTest : public DgramTwoWayStream {
public:
    ManagedBytes store[MAX_PACKET];
    int cursor;
    int readCursor;

    DgramTest() {
        cursor = 0;
        readCursor = 0;
    }

    virtual void onMonitorInput() {
        //printf("Waiting for input %d %d\n", readCursor, cursor);
        removeMonitor();
        if (readCursor<cursor) {
            setMonitor(store[readCursor].bytes());
            //printf("Gave input of size %d\n", getMonitor().length());
            readCursor++;
        }
    }

    virtual void onMonitorOutput() {
        if (cursor<MAX_PACKET) {
            store[cursor] = ManagedBytes(getMonitor(),false);
            store[cursor].copy();
        } else {
            printf("message too big\n");
            exit(1);
        }
        cursor++;
        removeMonitor();
    }

    int size() {
        return cursor;
    }

    void clear() {
        readCursor = 0;
        cursor = 0;
    }

    Bytes get(int i) {
        return store[i].bytes();
    }

    void copyMonitor(DgramTest& alt) {
        readCursor = 0;
        for (int i=0; i<alt.cursor; i++) {
            store[cursor] = ManagedBytes(alt.get(i),false);
            store[cursor].copy();
            cursor++;
        }
        removeMonitor();
    }

    void corrupt(int index, int offset = 0) {
        if (index<MAX_PACKET) {
            if (offset<(int)store[index].length()) {
                store[index].get()[offset] ^= 255;
            } else {
                printf("cannot corrupt nonexistent byte\n");
            }
        } else {
            printf("cannot corrupt nonexistent dgram\n");
        }
    }

    void corruptSwap(int index, int altIndex) {
        if (index<MAX_PACKET && altIndex<MAX_PACKET) {
            ManagedBytes tmp(store[index].bytes(),false);
            tmp.copy();
            store[index] = ManagedBytes(store[altIndex].bytes(),false);
            store[index].copy();
            store[altIndex] = ManagedBytes(tmp.bytes(),false);
            store[altIndex].copy();
        } else {
            printf("cannot corrupt nonexistent dgram\n");
        }
    }

    void corruptDrop(int index) {
        if (index<MAX_PACKET) {
            for (int i=index; i<cursor-1; i++) {
                store[i] = ManagedBytes(store[i+1].bytes(),false);
                store[i].copy();
            }
            cursor--;
        } else {
            printf("cannot corrupt nonexistent dgram\n");
        }
    }
};


class DgramTwoWayStreamTest : public UnitTest {
public:
    virtual ConstString getName() { return "DgramTwoWayStreamTest"; }

    void checkNormal() {
        report(0, "checking that dgrams are output sensibly");

        DgramTest out;

        int sz = 100;
        out.openMonitor(sz,sz);

        ManagedBytes msg(200);
        for (size_t i=0; i<msg.length(); i++) {
            msg.get()[i] = i%128;
        }
        out.beginPacket();
        out.write(msg.bytes());
        out.flush();
        out.endPacket();
        printf("created %d packets\n", out.size());
        checkEqual(3,out.size(),"right number of packets");


        ////////////////////////////////////////////////////////////////////
        // Send a multi-dgram message, see if it gets through

        report(0, "checking that dgrams can be reassembled into messages");
        DgramTest in;
        in.openMonitor(sz,sz);
        ManagedBytes recv(200);
        for (size_t i=0; i<recv.length(); i++) {
            recv.get()[i] = 0;
        }
        in.copyMonitor(out);
        in.beginPacket();
        in.readFull(recv.bytes());
        in.endPacket();
        bool mismatch = false;
        for (size_t i=0; i<recv.length(); i++) {
            if (recv.get()[i]!=msg.get()[i]) {
                printf("Mismatch, at least as early as byte %d\n", (int)i);
                mismatch = true;
                break;
            }
        }
        checkFalse(mismatch,"received what is sent");


        ////////////////////////////////////////////////////////////////////
        // Send three messages, see if all get through
        // (just testing the receiver side)

        report(0, "checking reassembly for multiple messages");
        in.clear();
        in.copyMonitor(out);
        in.copyMonitor(out);
        in.copyMonitor(out);
        mismatch = false;
        for (int k=0; k<3; k++) {
            for (size_t i=0; i<recv.length(); i++) {
                recv.get()[i] = 0;
            }
            in.beginPacket();
            in.readFull(recv.bytes());
            in.endPacket();
            for (size_t i=0; i<recv.length(); i++) {
                if (recv.get()[i]!=msg.get()[i]) {
                    printf("Mismatch, at least as early as byte %d\n", (int)i);
                    mismatch = true;
                    break;
                }
            }
        }
        checkFalse(mismatch,"multiple messages ok");


        ////////////////////////////////////////////////////////////////////
        // Send three messages, corrupt in different ways

        for (int problem=0; problem<3; problem++) {

            in.clear();
            in.copyMonitor(out);
            in.copyMonitor(out);
            in.copyMonitor(out);

            switch (problem) {
            case 0:
                report(0, "reassembly for 3 messages, middle one corrupted");
                // corrupt 10th byte of 4th dgram
                in.corrupt(4,10);
                break;
            case 1:
                report(0, "order switched in middle message");
                // swap 4th and 5th dgram
                in.corruptSwap(4,5);
                break;
            case 2:
                report(0, "drop dgram in middle message");
                in.corruptDrop(4);
                break;
            };

            mismatch = false;
            bool goodRead[4];
            int length[4];
            for (int k=0; k<4; k++) {
                //printf("Iteration %d\n", k);
                for (size_t i=0; i<recv.length(); i++) {
                    recv.get()[i] = 0;
                }
                in.beginPacket();
                int len = in.readFull(recv.bytes());
                in.endPacket();
                mismatch = false;
                for (size_t i=0; i<recv.length(); i++) {
                    if (recv.get()[i]!=msg.get()[i]) {
                        //printf("Mismatch, at least as early as byte %d\n", i);
                        mismatch = true;
                        break;
                    }
                }
                goodRead[k] = !mismatch;
                length[k] = len;
            }
            if (problem!=2) {
                checkTrue(goodRead[0],"first read should be good");
                checkTrue(!goodRead[1],"second read should be broken");
                checkTrue(!goodRead[2],"third read should be broken");
                checkTrue(goodRead[3],"fourth read should be good again");
                checkEqual(length[0],recv.length(),"first length should be full");
                checkEqual(length[1],-1,"second should be error");
                checkEqual(length[2],-1,"third should be error");
                checkEqual(length[3],recv.length(),"fourth length should be full");
            } else {
                checkTrue(goodRead[0],"first read should be good");
                checkTrue(!goodRead[1],"second read should be broken");
                checkTrue(goodRead[2],"third read should be good");
                checkTrue(!goodRead[3],"fourth read is nothing");
            }
        }
    }

    virtual void runTests() {
        checkNormal();
    }
};

static DgramTwoWayStreamTest theDgramTwoWayStreamTest;

UnitTest& getDgramTwoWayStreamTest() {
    return theDgramTwoWayStreamTest;
}
