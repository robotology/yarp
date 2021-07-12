/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

using namespace yarp::os;
using namespace std;
using yarp::companion::impl::Companion;


//------------------------------------------------------------------------------------------------------------------------------

enum client_return_code_t { CLIENT_END_TEST = 0, CLIENT_CARRIER_ERROR = 1};
enum server_return_code_t { SERVER_END_TEST = 0, SERVER_QUIT = 1,SERVER_ERROR =2 };

server_return_code_t server(double server_wait, bool verbose = false);
client_return_code_t client(int nframes, int payload_size, string proto, double pause, bool no_reply, string logfilename = "log_", bool verbose = false);

//------------------------------------------------------------------------------------------------------------------------------

int Companion::cmdLatencyTest(int argc, char* argv[])
{
    Property p;
    p.fromCommand(argc, argv, false);
    string proto="tcp";

    if (p.check("help") || argc==0)
    {
        yCInfo(COMPANION, "This is yarp latency-test");
        yCInfo(COMPANION, "Syntax for the server:");
        yCInfo(COMPANION, "yarp latency-test --server [--details]");
        yCInfo(COMPANION, "--server_wait <time>");
        yCInfo(COMPANION, " ");
        yCInfo(COMPANION, "Syntax for the client:");
        yCInfo(COMPANION, "yarp latency-test --client [--details]");
        yCInfo(COMPANION, "--nframes <X> --payload_size <Y> --client_wait <Z> [--protocol <protocol>] [--no-reply] [--logfile <filename_prefix>]");
        yCInfo(COMPANION, "--nframes <X> --multitest --client_wait <Z> [--protocol <protocol>] [--no-reply] [--logfile <filename_prefix>]");
        yCInfo(COMPANION, "--nframes <X> --multitest \"(1 400000)\" --client_wait <Z> [--protocol <protocol>] [--no-reply] [--logfile <filename_prefix>]");
        yCInfo(COMPANION, "--nframes <X> --customtest \"(1 40 100 10000)\" --client_wait <Z> [--protocol <protocol>] [--no-reply] [--logfile <filename_prefix>]");
        yCInfo(COMPANION, "Default value for filename_prefix: log_");
        return -1;
    }

    bool verbose = false;
    if (p.check("details")) verbose=true;

    if (p.check("server"))
    {
        double server_wait = p.find("server_wait").asFloat64();
        size_t servercounter=0;
        while(1)
        {
            int returncode = server(server_wait, verbose);
            if      (returncode == SERVER_END_TEST) { yCInfo(COMPANION, "Test %zu complete", servercounter++);}
            else if (returncode == SERVER_QUIT)     { yCInfo(COMPANION, "Test %zu complete, quitting", servercounter++); break;}
            else if (returncode == SERVER_ERROR)    { yCError(COMPANION, "Test %zu error", servercounter++); }
        }
    }
    else if (p.check("client"))
    {
        if (p.check("nframes") == false)
        {  yCError(COMPANION) << "Missing mandatory parameter nframes. See available options with yarp latency-test";  return -1; }
        int frames = p.find("nframes").asInt32();

        double client_wait = 0;
        if (p.check("client_wait")) client_wait = p.find("client_wait").asFloat64();

        string logfilename = "log_";
        if (p.check("logfile")) logfilename = p.find("logfile").asString();

        if (p.check("protocol")) proto = p.find("protocol").asString();

        bool no_reply = p.check("no-reply");

        if (p.check("payload_size") && !p.check ("multitest") && !p.check ("customtest"))
        {
            int payload = p.find("payload_size").asInt32();
            return client(frames, payload, proto, client_wait, no_reply, logfilename, verbose);
        }
        else if (!p.check("payload_size") && p.check("multitest") && !p.check ("customtest"))
        {
            Bottle* b =p.find("multitest").asList();
            double min = 1;
            double max = 800'000'000;
            if (b && b->size()==2)
            {
                min = b->get(0).asFloat64();
                max = b->get(1).asFloat64();
                //yCInfo(COMPANION) << min << max;
            }
            //prepare the test array
            //the generated sequence is similar to 1 2 4 8 10 20 40 80 100...
            std::vector<int> psizes;
            {
                int val =1;
                for (size_t pot=1; pot <10; pot++)
                {
                    for (size_t id=0 ; id<4; id++)
                    {
                        //yCInfo(COMPANION) << val;
                        if (val>=min && val<=max) psizes.push_back(val);
                        val*=2;
                    }
                    val=val-val%(int(pow(10,pot)));
                }
            }
            //execute the tests
            for (size_t i = 0; i < psizes.size(); i++)
            {
               client(frames, psizes[i], proto, client_wait, no_reply, logfilename, verbose);
            }
            return 0;
        }
        else if (!p.check("payload_size") && !p.check("multitest") && p.check ("customtest"))
        {
            Bottle* b =p.find("customtest").asList();
            if (b)
            {
                for (size_t i = 0; i < b->size(); i++)
                {
                    if (b->get(i).isInt32())
                    {
                        client(frames, b->get(i).asInt32(), proto, client_wait, no_reply, logfilename, verbose);
                    }
                    else
                    {
                        yCError(COMPANION) << "Syntax error. Invalid value for customtest. See available options with yarp latency-test";
                    }
                }
            }
        }
        else
        {
            yCError(COMPANION) << "Syntax error. See available options with yarp latency-test";
            return -1;
        }

    }
    else
    {
        yCError(COMPANION) << "Missing option. Use --help";
        return -1;
    }
    return -1;
}

//------------------------------------------------------------------------------------------------------------------------------

server_return_code_t server(double server_wait, bool verbose)
{
    //Open the port for connection with the client
    Port port;
    port.open("/latencyTest/server");
    yCInfo(COMPANION,"I am the server, now listening to the client for a `start` command\n");

    //Wait a start command from the client
    Bottle startbot;
    port.read(startbot);
    if (startbot.get(0).asString() != "start")
    {
        yCError(COMPANION) << "Invalid command received from the client";
        return SERVER_ERROR;
    }

    //Creates a payload bottle, consisting of a string with the size requested by the client
    int payload_reqsize = startbot.get(1).asInt32();
    bool use_reply = (startbot.get(2).asInt32() == 1);

    char* buf = new char[payload_reqsize];
    for (int elem = 0; elem < payload_reqsize - 1; elem++)
    {
        buf[elem] = 112;
    }
    buf[payload_reqsize - 1] = '\0';

    Bottle payloadbottle;
    payloadbottle.addString(buf);
    yCInfo(COMPANION,"Generated a string of %zu bytes, as requested by the client (%d)", payloadbottle.get(0).asString().size() + 1, payload_reqsize);

    if (use_reply) {
        yCInfo(COMPANION,"Replying on the same connection, as requested by the client");
    } else {
        yCInfo(COMPANION,"Using a separate connection for replies, as requested by the client");
    }

    size_t serverframecounter=0;
    while(true)
    {
        //Reads the frame sent by the client and checks if command `stop` or `quit` is received. Otherwise:
        //Adds the payload, the time required to add the payload, the frame number. Finally, it sends it back to client.
        Bottle b;
        port.read(b, use_reply);
        if (verbose)
        {
            yCInfo(COMPANION, "Received: \"%s\"", b.toString().c_str());
        }
        if      (b.get(0).asString() == "stop") break;
        else if (b.get(0).asString() == "quit") return SERVER_QUIT;
        double tt1 = yarp::os::Time::now();
        b.append(payloadbottle);
        double tt2 = yarp::os::Time::now();
        b.addFloat64(tt2-tt1);
        b.addInt32(serverframecounter);

        if (use_reply) {
            port.reply(b);
        } else {
            port.write(b);
        }

        //verbose prints
        if (verbose)
        {
            yCInfo(COMPANION, "Replying: \"%s\"", b.toString().c_str());
            yCInfo(COMPANION, "This time was required to append the payload: %f", tt2 - tt1);
            yCInfo(COMPANION, "Sending the frame number: %zu", serverframecounter);
        }

        //Give the CPU some idle time
        if (server_wait > 0) Time::delay(server_wait);
        serverframecounter++;
    }

    //The test is complete
    port.interrupt();
    port.close();
    return SERVER_END_TEST;
}

//------------------------------------------------------------------------------------------------------------------------------

client_return_code_t client(int nframes, int payload_size, string proto, double client_wait, bool no_reply, string logfilename, bool verbose)
{
    //the structure where to save the data
    struct stats
    {
        double latency=0;
        double copytime=0;
    };
    std::vector<stats> test_data;
    test_data.resize(nframes);

    //opens a local port and connects bidirectionally with the server
    Port port;
    port.open("/latencyTest/client");

    auto* carrier = yarp::os::Carriers::getCarrierTemplate(proto);
    if (!carrier) {
        return CLIENT_CARRIER_ERROR;
    }

    bool use_reply = true;
    if (!carrier->supportReply() || no_reply) {
        use_reply = false;
    }

    while (!Network::connect("/latencyTest/client", "/latencyTest/server", proto))
    {
        yCInfo(COMPANION, "Waiting for connection..\n");
        Time::delay(0.5);
    }

    if (!use_reply) {
        while(!Network::connect("/latencyTest/server","/latencyTest/client", proto))
        {
            yCInfo(COMPANION, "Waiting for connection..\n");
            Time::delay(0.5);
        }
    }

    //Send to the server a command 'start', followed by the requested size of the payload (in bytes)"
    Bottle startbot;
    startbot.addString("start");
    startbot.addInt32(payload_size);
    startbot.addInt32(use_reply ? 1 : 0);
    port.write(startbot);

    //Performs the test, by sending request to the server. The duration of the test depends on the number of requested frames.
    int clientframecounter = 0;
    double latency_max = 0;
    double latency_min = std::numeric_limits<double>::infinity();
    while(clientframecounter <nframes)
    {
        //sends the frame to server. The frame is composed by
        //0 clientframecounter a counter id
        //1 clientframetime the current time
        Bottle datum;
        Bottle reply;
        double clientframetime = Time::now();
        datum.addInt32(clientframecounter);
        datum.addFloat64(clientframetime);

        if (use_reply) {
            port.write(datum, reply);
        } else  {
            port.write(datum);
            port.read(reply);
        }

        //receives back from the server the frame just sent, with the additional stuff appended by the server.
        //So the final content is:
        //0 the clientframecounter
        //1 the clientframetime
        //2 a blob of data (string)
        //3 the copytime computed by the server
        //4 the serverframecounter
        double finaltime=Time::now();

        int recT =reply.get(0).asInt32();
        YARP_UNUSED(recT);
        double time = reply.get(1).asFloat64();
        std::string recstringpayload = reply.get(2).asString();
        double copytime = reply.get(3).asFloat64();
        double latency_ms = (finaltime - time - copytime) * 1000;
        test_data[clientframecounter].latency = latency_ms;
        test_data[clientframecounter].copytime = copytime;
        if (latency_ms>latency_max) latency_max = latency_ms;
        if (latency_ms<latency_min) latency_min = latency_ms;

        if (verbose)
        {
            //These prints are ok for debug, but they will slow down the tests.
            yCInfo(COMPANION, "Received a payload of %zu bytes", recstringpayload.size());
            yCInfo(COMPANION, "latency for frame %d is: %lf ms\n", clientframecounter, (finaltime -time)*1000);
        }

        //Give the CPU some idle time
        if (client_wait >0) {yarp::os::Time::delay(client_wait);}

        clientframecounter++;
    }

    //Send to the server a command 'stop'. The test is complete.
    //The server will now restart, waiting for a new client connection.
    Bottle stopbot;
    stopbot.addString("stop");
    port.write(stopbot);

    //close the port
    port.interrupt();
    Network::disconnect("/latencyTest/client", "/latencyTest/server");
    if (!use_reply) {
        Network::disconnect("/latencyTest/server","/latencyTest/client", proto);
    }
    Time::delay(0.5);
    port.close();

    //prints stats to screen
    double latency_mean = 0;
    double latency_stdev = 0;
    for (int i = 0; i < nframes; i++)
    {
        latency_mean+=test_data[i].latency;
    }
    latency_mean/=nframes;
    for (int i = 0; i < nframes; i++)
    {
        latency_stdev+=pow(test_data[i].latency-latency_mean,2);
    }
    latency_stdev=sqrt(latency_stdev/nframes);
    yCInfo(COMPANION, "Processed %d frames of %d bytes, average latency %.3lf[ms], max %.3lf[ms], min %.3lf[ms], stdev %.3lf[ms]\n", clientframecounter, payload_size, latency_mean, latency_max, latency_min, latency_stdev);

    //save the stats to a logfile
    std::fstream fs;
    std::string filename = logfilename;
    filename += std::to_string(payload_size);
    filename += ".txt";
    fs.open(filename, std::fstream::out );
    for (int i = 0; i < nframes; i++)
    {
        fs << test_data[i].latency << " " << test_data[i].copytime << std::endl;
    }
    fs.close();
    yCInfo(COMPANION, "Test complete. Data saved to file: %s", filename.c_str());

    return CLIENT_END_TEST;
}
