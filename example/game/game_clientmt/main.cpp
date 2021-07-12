/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <random>
#include <mutex>

#include <yarp/os/all.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/Time.h>
using namespace yarp::os;

const char SERVER_NAME[]="/game";
const double PLAYER_PERIOD=0.5;

const char UP[]="up";
const char DOWN[]="down";
const char RIGHT[]="right";
const char LEFT[]="left";
const char FIRE[]="fire";
const char GO[]="go";

class MyPlayer: public PeriodicThread
{
public:
    std::default_random_engine randengine;

    MyPlayer(const char *n, double period):PeriodicThread(period)
    {
        myX=0;
        myY=0;
        myLife=0;
        shooterF=0;

        strcpy(myName, n);
        printf("Registering port\n");
        port.open(myName);
    }

    ~MyPlayer()
    {
        port.close();
    }

    void doInit()
    {
        mutex.lock();

        printf("Connecting with game server\n");
        Network::connect(port.getName(), SERVER_NAME);

        myLife=6;

        randengine.seed(0);

        mutex.unlock();
    }

    // void doLoop()
    void run()
    {
        mutex.lock();

        look();
        rndMove();
        if(shooterF)
            rndShoot();

        mutex.unlock();
    }

    void doRelease()
    {
        mutex.lock();

        printf("Disconnecting\n");
        Network::disconnect(port.getName(), SERVER_NAME);

        mutex.unlock();
    }

    void look()
    {
        Bottle cmd;
        Bottle response;
        cmd.addString("look");
        port.write(cmd,response);

        // pick out map part
        world= response.findGroup("look").findGroup("map");

        Bottle &users = response.findGroup("look").findGroup("players");

        Bottle *player = users.get(1).asList();
        if (player!=0)
            {
                Bottle &location = player->findGroup("location");
                Value &life = player->find("life");
                std::string playerName = player->get(0).asString();

                myX=location.get(1).asInt32(),
                    myY=location.get(2).asInt32(),
                    myLife=life.asInt32();
            }
    }

    void rndMove()
    {
        Bottle cmd;
        Bottle response;
        cmd.addString("go");
        cmd.addString(randomDirection());

        port.write(cmd, response);
    }

    void setShooter(int sh)
    {
        shooterF=sh;
    }

    void rndShoot()
    {
        Bottle cmd;
        Bottle response;
        cmd.addString("fire");
        cmd.addString(randomDirection());

        port.write(cmd, response);
    }

    const char* randomDirection()
    {
        std::uniform_real_distribution<double> udist(0.0,1.0);
        double rnd=udist(randengine);

        if (rnd<1/4.0)
            return RIGHT;
        if ( (rnd>=1/4.0) && (rnd<=2/4.0))
            return  LEFT;
        if ( (rnd>=2/4.0) && (rnd<=3/4.0))
            return  DOWN;
        if ( (rnd>=3/4.0) && (rnd<=1.0))
            return  UP;

        return DOWN;
    }

    void getWorld(Bottle &w)
    {
        mutex.lock();

        w=world;

        mutex.unlock();
    }

    int getLife()
    {
        int ret;
        mutex.lock();
        ret=myLife;
        mutex.unlock();

        return ret;
    }

    int getX()
    {
        int ret;
        mutex.lock();
        ret=myX;
        mutex.unlock();
        return ret;
    }

    int getY()
    {
        int ret;
        mutex.lock();
        ret=myY;
        mutex.unlock();
        return ret;
    }

    int myLife;
    int myX;
    int myY;
    int shooterF;
    char myName[255];

    Bottle world;
    Port port;


    double prev;
    double now;
    std::mutex mutex;
};

int main(int argc, char **argv)
{
    if (argc!=3)
        return 0;

    Network yarp;

    MyPlayer *player = new MyPlayer(argv[1], PLAYER_PERIOD);

    if(atoi(argv[2])==0)
        player->setShooter(0);
    else
        player->setShooter(1);

    player->start();

    int count=0;
    while(true)
        {
            int plLife;
            int plX;
            int plY;
            count++;

            plLife=player->getLife();
            plX=player->getX();
            plY=player->getY();

            if ((count==500)||(plLife==0))
                {
                    fprintf(stderr, "Stopping player\n");
                    player->stop();
                    fprintf(stderr, "Waiting some time\n");
                    Time::delay(2);

                    printf("Restarting the player\n");
                    player->start();

                    count=0;
                }

            Bottle world;
            player->getWorld(world);

            int i;
            for (i=1; i<world.size(); i++)
                {
                    printf("%s\n", world.get(i).asString().c_str());
                }

            printf("X:%d Y:%d Life:%d\n", plX, plY, plLife);

            Time::delay(0.5);
        }

    player->stop();

    delete player;

    return 0;
}
