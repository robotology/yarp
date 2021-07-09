/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <cmath>

#include <stdlib.h>
#include <time.h>

using namespace yarp::os;

// Port latency, basic test.
// Send a sequence of message to a port. Test the
// time it takes for the message to be received.
// Compute average.
//
// Time only makes sense if server and client run
// on the same machine.
//
// Lorenzo Natale May 2008
//
// Added paralell port code August 2008.

// Parameters:
// --name: give a name to the port
// --server: act as a server
// --client: act as a client
// --period: if server set the periodicity of the messages [ms]
// --nframes: if client specifies how many message are received
// before closing (default: waits forever)

// our own data type
class TestData: public yarp::os::Portable
{
private:
    double datum;
    unsigned char *payload;
    unsigned int payloadSize;

public:
    TestData()
    {
        payloadSize=0;
        payload=0;
    }

    ~TestData()
    {
        if (!payload)
            delete [] payload;
    }

    virtual bool read(ConnectionReader& connection)
    {
        unsigned int incoming;
        bool ok = connection.expectBlock((char*)&datum, sizeof(double));
        ok=ok&&connection.expectBlock((char*)&incoming, sizeof(int));

        //this resize only if needed
        resize(incoming);

        if (payloadSize>0)
            ok=ok&&connection.expectBlock((char *)payload, payloadSize);

        return ok;
    }

    virtual bool write(ConnectionWriter& connection)
    {
        connection.appendBlock((char*)&datum, sizeof(double));
        connection.appendBlock((char*)&payloadSize, sizeof(int));

        if (payloadSize>0)
            connection.appendBlock((char *)payload, payloadSize);

        return !connection.isError();
    }

    double inline get()
    { return datum; }

    void inline set(double v)
    {  datum=v; }

    void inline resize(unsigned int b)
    {
        if (b==payloadSize)
            return; //nothing to do

        if (payload!=0)
            delete [] payload;

        payloadSize=b;

        if (payloadSize==0)
            payload=0;

        payload=new unsigned char [payloadSize];
        srand (static_cast<unsigned int> (time(0)));
        for(unsigned int k=0;k<payloadSize;k++)
        {
            unsigned int rnd=rand();
            payload[k]=rnd; //paranoid
        }

    }

    unsigned int getPayloadSize()
    {
        return payloadSize;
    }
};

#ifdef USE_PARALLEL_PORT
#include <ppEventDebugger.h>
#endif

#include <string>

#include <vector>

struct Report
{
    Report(double t, unsigned int pl)
    {
        dt=t;
        payload=pl;
    }

    double dt;
    unsigned int payload;
};

class Reader : public BufferedPort<TestData> {
public:
    BufferedPort<TestData> outPort;

    double delay;
    double delaySq;
    int count;
    int wait;

    int nframes;

    std::vector<Report> latencies;

public:
    Reader()
    {
        delay=0;
        delaySq=0;
        count=0;
        // wait some messages before counting
        wait=0;
    }

    virtual void onRead(TestData &datum)
    {
        if (count==nframes)
            return;


        double now=Time::now();
        double origT=datum.get();
        unsigned int pl=datum.getPayloadSize();


#ifdef USE_PARALLEL_PORT
        static ppEventDebugger pp(0x378);
        pp.set();
        pp.reset();
#endif

        TestData &nd=outPort.prepare();
        nd.set(origT);
        nd.resize(pl);
        outPort.write(true);

        if (wait<=0)
            {

                double dT=(now-origT)*1000;
                delay+=dT;
                delaySq+=(dT*dT);
                count++;

                latencies.push_back(Report(dT, pl));
            }
        else
            wait--;
    }
};

class serverThread: public PeriodicThread
{
    BufferedPort<TestData> port;
    unsigned int payload;

public:
    serverThread():PeriodicThread(0.100)
    {}

    void setPayload(unsigned int p)
    {
        payload=p;
    }

    void open(double period, const std::string &name)
    {
        std::string portName;
        portName="/profiling/server/";
        portName+=name;
        portName+="/port:o";
        port.open(portName.c_str());

        PeriodicThread::setPeriod(period);
        PeriodicThread::start();
    }

    void run()
    {

        //  printf("Sending frame %d\n", k);
        TestData &datum=port.prepare();
        datum.resize(payload);
        double time=Time::now();
        datum.set(time);

#ifdef USE_PARALLEL_PORT
        static ppEventDebugger pp(0x378);
        pp.set();
#endif
        port.write(true);
#ifdef USE_PARALLEL_PORT
        pp.reset();
#endif
    }

    bool releaseThread()
    {
        port.close();
    }
};

int server(double server_wait, const std::string &name, unsigned int payload)
{
    serverThread thread;
    thread.setPayload(payload);
    thread.open(server_wait, name);

    bool forever=true;
    while(forever)
        Time::delay(2);

    return 0;
}

int client(int nframes, std::string &name)
{
    Reader reader;
    reader.nframes=nframes;

    bool forever=false;
    if (nframes==-1)
        forever=true;
    else
        reader.latencies.reserve(nframes);

    std::string portName;
    portName="/profiling/client/";
    portName+=name;
    portName+="/port:i";

    reader.useCallback();
    reader.open(portName.c_str());

    portName="/profiling/client/";
    portName+=name;
    portName+="/port:o";

    reader.outPort.open(portName.c_str());

    while( (reader.count<nframes) || forever)
        {
            //give the CPU some time
            Time::delay(0.5);
        }

    reader.close();

    std::vector<Report>::iterator it=reader.latencies.begin();
    FILE *of=fopen("timing.txt", "w");
    while(it!=reader.latencies.end())
        {
            fprintf(of, "%u %lf\n", (*it).payload, (*it).dt);
            it++;
        }
    fclose(of);

    double averageLatency=reader.delay/reader.count;
    double stdLatency=(1.0/(reader.count-1))*(reader.delaySq-reader.count*averageLatency*averageLatency);

    stdLatency=sqrt(stdLatency);

    fprintf(stderr, "Received: %d average latency %.3lf +/- %.5lf [ms]\n",
            reader.count, averageLatency, stdLatency);
    return 0;
}

int main(int argc, char **argv) {
    Network yarp;
    Property p;
    p.fromCommand(argc, argv);

    std::string name;
    if (p.check("name"))
        name=p.find("name").asString().c_str();
    else
        name="default";

    if (p.check("server"))
        {

            int payload=p.check("payload", Value(1)).asInt32();

            printf("Setting payload to %d[bytes]\n", payload);

            int dataSize=sizeof(int)+sizeof(double);
            if (payload>dataSize)
                payload-=dataSize;
            else
                {
                    fprintf(stderr, "Error: payload must be > %d", dataSize);
                    return -1;
                }

            return server(p.find("period").asFloat64()/1000.0, name, payload);
        }
    else if (p.check("client"))
        return client(p.find("nframes").asInt32(), name);
}
