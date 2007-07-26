// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_COMPANION_
#define _YARP2_COMPANION_

#include <yarp/String.h>
#include <yarp/Writable.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Vector_T.h>
#include <ace/OS_NS_stdio.h>

// ACE headers may fiddle with main 
#ifdef main
#undef main
#endif



namespace yarp {
    class Companion;
}

/**
 * Implementation of a standard set of YARP utilities.
 */
class yarp::Companion {
public:

    String version();

    /**
     * The standard main method for the YARP companion utility.
     * @param argc Argument count
     * @param argv Command line arguments
     * @return 0 on success, non-zero on failure
     */

    static int main(int argc, char *argv[]);

    /**
     * Request that an output port connect to an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param silent whether to print comments on the result
     * @return 0 on success, non-zero on failure
     */
    static int connect(const char *src, const char *dest,
                       bool silent = false);

    /**
     * Request that an output port disconnect from an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param silent whether to print comments on the result
     * @return 0 on success, non-zero on failure
     */
    static int disconnect(const char *src, const char *dest,
                          bool silent = false);

    static int disconnectInput(const char *target, const char *src,
                               bool silent = false);

    static int poll(const char *target, bool silent = false); 

    /**
     * Create a port to read Bottles and prints them to standard input.
     * It assumes the Bottles consist of an integer followed by a string.
     * The integer indicates whether the "end-of-file" has been reached.
     * The string is what gets printed.
     * @param name the name which which to register the port
     * @param src name of a port to connect from, if any
     * @param showEnvelope set to true if you want envelope information
     * shown
     * @return 0 on success, non-zero on failure
     */
    static int read(const char *name, const char *src = NULL,
                    bool showEnvelope = false);

    static int write(const char *name, int ntargets, char *targets[]);

    static int rpc(const char *connectionName, const char *targetName);

    static String slashify(const String& src) {
        if (src.length()>0) {
            if (src[0] == '/') {
                return src;
            }
        }
        return String("/") + src;
    }

    /**
     * Read a line of arbitrary length from standard input.
     */
    static String readString(bool *eof=NULL);


    static int sendMessage(const String& port, Writable& writable, 
                           bool silent = false) {
        String output;
        return sendMessage(port,writable,output,silent);
    }

    static int sendMessage(const String& port, Writable& writable, 
                           String& output,
                           bool silent);


private:

    Companion();
  
    static Companion& getInstance() {
        return instance;
    }

    static Companion instance;

    int dispatch(const char *name, int argc, char *argv[]);

    int cmdVersion(int argc, char *argv[]);

    int cmdTerminate(int argc, char *argv[]);

    int cmdName(int argc, char *argv[]);

    int cmdWhere(int argc, char *argv[]);

    int cmdConf(int argc, char *argv[]);

    int cmdHelp(int argc, char *argv[]);

    int cmdConnect(int argc, char *argv[]);

    int cmdDisconnect(int argc, char *argv[]);

    int cmdRead(int argc, char *argv[]);

    int cmdWrite(int argc, char *argv[]);

    int cmdRpc(int argc, char *argv[]);

    int cmdRegression(int argc, char *argv[]);

    int cmdServer(int argc, char *argv[]);

    int cmdCheck(int argc, char *argv[]);

    int cmdPing(int argc, char *argv[]);

    int cmdExists(int argc, char *argv[]);

    int cmdWait(int argc, char *argv[]);

    int cmdMake(int argc, char *argv[]);

    int cmdRun(int argc, char *argv[]);

    int cmdNamespace(int argc, char *argv[]);

    class Entry {
    public:
        String name;
        int (Companion::*fn)(int argc, char *argv[]);

        Entry(const char *name, int (Companion::*fn)(int argc, char *argv[])) :
            name(name),
            fn(fn)
        {}

        Entry() {
        }
    };

    ACE_Hash_Map_Manager<String,Entry,ACE_Null_Mutex> action;
    ACE_Vector<String> names;
    ACE_Vector<String> tips;

    void add(const char *name, int (Companion::*fn)(int argc, char *argv[]),
             const char *tip = NULL) {
        Entry e(name,fn);
        action.bind(String(name),e);
        // maintain a record of order of keys
        names.push_back(String(name));
        if (tip!=NULL) {
            tips.push_back(String(tip));
        } else {
            tips.push_back(String(""));
        }
    }
};

#endif
