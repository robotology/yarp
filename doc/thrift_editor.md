Thrift IDL in YARP: editing structures remotely                 {#thrift_editor}
===============================================

[TOC]

Thrift offers a convenient way to define structures and services.
Structures are bundles of data, and services are bundles of methods.
Structures fit nicely into streaming communication, while services are intended
for RPC communication.
But sometimes we want to offer a set of services around a particular structure.
One recurring desire is a way to manipulate or read just part of a structure.
This is convenient if that structure represents configuration data for a
program.

With YARP, every Thrift structure has a corresponding `Editor` service that can
be used to manipulate or read parts of it.
For example, suppose we have the following structure in `settings.thrift`

```{.cpp}
struct Settings
{
       1: i32 id;
       2: string name;
       3: list<i32> ints;
}
```

When compiled, that will give is a `Settings` class that can read or write from
a port.
That class will read the structure in full.
For example, here's a test program that will read the structure from a port and
show its value:

```{.cpp}
#include <yarp/os/all.h>
#include "Settings.h"
int main()
{
  yarp::os::Network yarp;   // bring up the yarp network
  yarp::os::Port port;      // we'll want a port
  Settings settings;        // here are the settings we'll be controlling
  port.setReader(settings); // read automatically into settings
  port.setCallbackLock();   // allow automatic reads to be locked
  port.open("/settings");   // open port!
  while(true) {
    port.lockCallback();    // lock reads while we show settings
    printf("Settings %s\n", settings.toString().c_str());
    port.unlockCallback();  // unlock reads
    yarp::os::Time::delay(1);         // wait a little bit
  }
  return 0;
}
```

(If you want to quickly compile this program, just put it in a file called
`<something>.cpp` in an empty directory, then add `settings.thrift`, then run
`yarp cmake && cmake . && make && ./yarpy`, specifying
`-DYARP_DIR=<path_to_yarp>` to cmake if needed.)

Once that is running, from another terminal we can write to the settings:

```
echo "1 sam (1 2 3)" | yarp write /write /settings
```

And we see the resulting change of the entire structure:

```
yarp: Port /settings active at tcp://192.168.1.2:10002
Settings 0 "" ()
Settings 0 "" ()
Settings 1 sam (1 2 3)
...
```

Now, suppose we want to offer a way to change parts of the structure
individually.
We can make the following two-line modification:

```{.cpp}
int main()
{
  yarp::os::Network yarp;
  yarp::os::Port port;
  Settings settings;
  Settings::Editor editor(settings);  // add an editor for settings
  port.setReader(editor);             // read via the editor
  port.setCallbackLock();
  port.open("/settings");
  while(true) {
    port.lockCallback();
    printf("Settings %s\n", settings.toString().c_str());
    port.unlockCallback();
    yarp::os::Time::delay(1);
  }
  return 0;
}
```

Now, we can send a new kind of message to the port:

```
echo 'set name "sam"' | yarp write /write /settings
```

(It is now also possible to use `yarp rpc /settings` to send commands and get
status feedback).
Now we can see that just the part of the structure we care about will change:

```
yarp: Port /settings active at tcp://192.168.1.2:10002
Settings 0 "" ()
Settings 0 "" ()
Settings 0 sam ()
...
```

The editor also lets us capture events generated just before and just after a
field is changed.
For example, if we want to do something just before or just after the `name`
field is changed, we could do this:

```{.cpp}
class MySettings : public Settings::Editor
{
public:
    bool will_set_name() override
    {
        printf("About to set the name, it is currently '%s'\n", state().name.c_str());
        return true;
    }

    bool did_set_name() override
    {
        printf("Just set the name, it is now '%s'\n", state().name.c_str());
        return true;
    }
};

int main()
{
  yarp::os::Network yarp;
  yarp::os::Port port;
  Settings settings;
  MySettings editor;     // switch to using our custom editor
  editor.edit(settings); // connect it to our settings
  // ... everything else is the same ...
}
```

Now if we send a message to change the `name` field, we'll see:

```
yarp: Port /settings active at tcp://192.168.1.2:10002
Settings 0 "" ()
Settings 0 "" ()
About to set the name, it is currently ''
Just set the name, it is now 'sam'
Settings 0 sam ()
...
```
