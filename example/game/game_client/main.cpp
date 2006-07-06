#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include <yarp/os/all.h>
using namespace yarp::os;

#include "simio.h"
#include "keyboard.h"
using namespace yarp;

#define real_printf printf
#define printf cprintf


Semaphore broadcastMutex(1);
String broadcast = "";

class BroadcastHandler : public TypedReaderCallback<Bottle> {
public:
  virtual void onRead(Bottle& bot) {
    broadcastMutex.wait();
    broadcast = bot.toString().c_str();
    broadcastMutex.post();
  }
} handler;



class UpdateThread : public Thread {
public:
  Port p;
  String name;
  PortReaderBuffer<Bottle> reader;
  Semaphore mutex;

  UpdateThread() : mutex(1) {
  }

  void setName(const char *name) {
    this->name = name;
  }

  void show() {
    mutex.wait();
    Bottle send("look");
    Property prop;
    p.write(send,prop);
    mutex.post();
    clrscr();
    Bottle& map = prop.findGroup("look").findGroup("map");
    broadcastMutex.wait();
    String prep = getPreparation().c_str();
    if (prep.length()>0) {
      long int t = (long int)Time::now();
      if (t%2==0) {
	prep = prep + "_";
      }
    }
    printf("\n%s\n%s\n\n", prep.c_str(), broadcast.c_str());
    broadcastMutex.post();
    for (int i=1; i<map.size(); i++) {
      printf("  %s\n", map.get(i).asString().c_str());
    }
    printf("\n");
    Bottle& players = prop.findGroup("look").findGroup("players");
    for (int i=1; i<players.size(); i++) {
      Bottle *player = players.get(i).asList();
      if (player!=NULL) {
	Bottle& location = player->findGroup("location");
	Value& life = player->find("life");
	printf("PLAYER %s is at (%d,%d) with lifeforce %d\n", 
	       player->get(0).asString().c_str(), 
	       location.get(1).asInt(),
	       location.get(2).asInt(),
	       life.asInt());
      }
    }
  }

  void apply(const String& str) {
    Bottle send, recv;
    send.fromString(str.c_str());
    mutex.wait();
    p.write(send,recv);
    if (recv.get(0).asString()=="error") {
      printf("PROBLEM:\n");
      printf("  request: %s\n", send.toString().c_str());
      printf("  response: %s\n", recv.toString().c_str());
      refresh();
      Time::delay(2);
    }
    mutex.post();
    show();
  }

  virtual void run() {
    reader.attach(p);
    reader.useCallback(handler);

    if (name[0]!='.') {
      name = String("/player/") + name;
    }
    p.open(name.c_str());
    
    // we'll be sending messages to the game (and getting responses)
    Network::connect(p.getName(),"/game");
    
    // there are occasional messages broadcast from the game to us
    Network::connect("/game",p.getName(),"mcast");

    while (!isStopping()) {
      autorefresh();
      Time::delay(0.25);
      show();
    }
    mutex.wait();
    p.close();
    mutex.post();
  }

} update_thread;


void stop(int x) {
  clrscr();
  printf("Stopping...\n");
  autorefresh();
  update_thread.stop();
  Time::delay(0.5);
  deinitconio();
  Time::delay(0.5);
  exit(0);
}



void mainloop(const char *name) {
  update_thread.setName(name);
  update_thread.start();

  bool done = false;
  while (!done) {
    String str = getCommand();
    if (str!="") {
      if (str=="quit") {
	done = true;
	break;
      }
      update_thread.apply(str);
    }
  }

  update_thread.stop();
  deinitconio();
}




int main(int argc, char *argv[]) {
#ifndef WIN32
  signal(SIGKILL,stop);
  signal(SIGINT,stop);
  signal(SIGTERM,stop);
  signal(SIGPIPE,stop);
#endif

  Network::init();

  initconio();
  setautorefresh(1);
  clrscr();

  const char *name = "...";
  if (argc>=2) {
    name = argv[1];
  }

  mainloop(name);

  Network::fini();

  return 0;
}
