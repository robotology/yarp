/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>

using namespace yarp::os;

#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_NOT VOCAB3('n','o','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_REMOVE VOCAB2('r','m')


int main(int argc, char *argv[]) {
    if (argc<=1) {
        printf("This is a very simple database\n");
        printf("Call as: %s --name /database\n", argv[0]);
        printf("Then you can test it by running:\n");
        printf("  yarp rpc /database\n");
        printf("And typing things like:\n");
        printf("  set x 24\n");
        printf("  get x\n");
        printf("  get y\n");
        printf("  rm x\n");
        printf("  get x\n");
        printf("  set \"my favorite numbers\" (5 10 16)\n");
        printf("  get \"my favorite numbers\"\n");
    }

    Network yarp;

    Property option;
    option.fromCommand(argc,argv);

    Property state;

    Port port;
    port.open(option.check("name",Value("/database")).asString());

    while (true) {
        Bottle cmd;
        Bottle response;
        port.read(cmd,true);  // true -> will reply

        Bottle tmp;
        tmp.add(cmd.get(1));
        ConstString key = tmp.toString();

        switch (Vocab::encode(cmd.get(0).toString())) {
        case VOCAB_SET:
            state.put(key,cmd.get(2));
            break;
        case VOCAB_GET:
            break;
        case VOCAB_REMOVE:
            state.unput(key);
            break;
        }
        Value& v = state.find(key);
        response.addVocab(v.isNull()?VOCAB_NOT:VOCAB_IS);
        response.add(cmd.get(1));
        if (!v.isNull()) {
            response.add(v);
        }
        port.reply(response);
    }

    return 0;
}

