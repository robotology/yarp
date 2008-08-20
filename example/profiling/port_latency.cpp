// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>

#include <math.h>

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
public:
    virtual bool read(ConnectionReader& connection)
    {
        bool ok = connection.expectBlock((char*)&datum, sizeof(double));
        return ok;
    }

    virtual bool write(ConnectionWriter& connection)
    {
        connection.appendBlock((char*)&datum, sizeof(double));
        return !connection.isError();
    }

    double inline get()
    { return datum; }
    
    void inline set(double v)
    {  datum=v; }
};

#include <ppEventDebugger.h>
#include <string>

#include <vector>

class Reader : public BufferedPort<TestData> {
public:
    BufferedPort<TestData> outPort;

    double delay;
    double delaySq;
    int count;
    int wait;

    int nframes;

    std::vector<double> latencies;

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

        static ppEventDebugger pp;
        static bool init=false;

        double now=Time::now();
        double t=datum.get();

        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        pp.set();
        pp.reset();

        TestData &nd=outPort.prepare();
        nd.set(t);
        outPort.write();

        if (wait<=0)
            {

                double dT=(now-t)*1000;
                delay+=dT;
                delaySq+=(dT*dT);
                count++;

                latencies.push_back(dT);
            }
        else
            wait--;
    }
};

class serverThread: public RateThread
{
    BufferedPort<TestData> port;

public:
    serverThread():RateThread(100)
    {}

    void open(double period, const std::string &name)
    {
        std::string portName;
        portName="/profiling/server/";
        portName+=name;
        portName+="/port:o";
        port.open(portName.c_str());

        RateThread::setRate(int (period*1000.0+0.5));
        RateThread::start();
    }

    void run()
    {
        static ppEventDebugger pp;
        static bool init=false;
        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        //  printf("Sending frame %d\n", k);
        TestData &datum=port.prepare();
        double time=Time::now();
        datum.set(time);
        pp.set();
        port.write();
        pp.reset();
    }

    bool releaseThread()
    {
        port.close();
    }
}; 

int server(double server_wait, const std::string &name)
{
    serverThread thread;
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
        reader.latencies.resize(nframes);

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

    std::vector<double>::iterator it=reader.latencies.begin();
    while(it!=reader.latencies.end())
        {
            printf("%lf\n", *it);
            it++;
        }

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
        return server(p.find("period").asDouble()/1000.0, name);
    else if (p.check("client"))
        return client(p.find("nframes").asInt(), name);
}






