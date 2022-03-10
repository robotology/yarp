/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/os/LogStream.h>
#include <signal.h>

#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/std_msgs/Int8MultiArray.h>

using yarp::companion::impl::Companion;

bool interrupt = false;
void signal_callback_handler(int signum)
{
    interrupt = true;
}

int print_help()
{
    yCInfo(COMPANION, "Available options:");
    yCInfo(COMPANION, "'--publish <topic> <size_bytes> <period_s>' to test a topic publisher");
    yCInfo(COMPANION, "'--subscribe <topic> to test a topic subscriber");
    return 1;
}

int Companion::cmdTopicTest(int argc, char *argv[])
{
    if (argc == 0)
    {
        //0 = yarp topic-test
        return print_help();
    }

    if (argc == 1 && std::string(argv[0]) == "--publish")
    {
        yCError(COMPANION) << "Invalid/missing topic name";
        return print_help();
    }
    else if (argc == 2 && std::string(argv[0]) == "--publish")
    {
        yCError(COMPANION) << "Invalid params";
        return print_help();
    }
    else if (argc == 3 && std::string(argv[0]) == "--publish")
    {
        yCError(COMPANION) << "Invalid params";
        return print_help();
    }
    else if (argc == 4 && std::string(argv[0]) == "--publish")
    {
        //parse command line
        std::string topicname = std::string(argv[1]);
        if (topicname.empty())
        {
            yCError(COMPANION) << "Invalid/missing topic name\n";
            return print_help();
        }
        size_t blobsize = std::atoi(argv[2]);
        if (blobsize == 0)
        {
            yCError(COMPANION) << "Invalid/missing payload size";
            return print_help();
        }
        double period = std::atof(argv[3]);
        if (period == 0)
        {
            yCError(COMPANION) << "Invalid period";
            return print_help();
        }

        //open the node
        yarp::os::Node* m_node = new yarp::os::Node("/testnode");
        if (m_node == nullptr)
        {
            yCError(COMPANION) << " opening " << "/testnode" << " Node, check your yarp-ROS network configuration\n";
            return 1;
        }

        //open the topics
        yarp::os::Publisher<yarp::rosmsg::nav_msgs::Odometry>  rosPublisherPort_odom;
        yarp::os::Publisher<yarp::rosmsg::std_msgs::Int8MultiArray>  rosPublisherPort_arr;
        std::string rosPublisherPort_odom_name = topicname + "_odom";
        std::string rosPublisherPort_arr_name = topicname + "_array";

        yCInfo(COMPANION) << " opening topics\n";
        if (!rosPublisherPort_odom.topic(rosPublisherPort_odom_name))
        {
            yCError(COMPANION) << " opening " << topicname << " Topic, check your yarp-ROS network configuration\n";
            return 1;
        }
        if (!rosPublisherPort_arr.topic(rosPublisherPort_arr_name))
        {
            yCError(COMPANION) << " opening " << topicname << " Topic, check your yarp-ROS network configuration\n";
            return 1;
        }
        yCInfo(COMPANION) << "Topics opened for publishing\n";

        size_t count=0;
        std::vector<int8_t> blob;
        blob.resize(blobsize);
        yCInfo(COMPANION) << "Publisher will use a payload of "<< blobsize << " bytes, period" << period << "s (" << (double)(blobsize)*8.0/period/1000000 << " Mb/s)";

        //publisher loop
        signal(SIGINT, signal_callback_handler);
        while (!interrupt)
        {
            yarp::rosmsg::nav_msgs::Odometry& rosData = rosPublisherPort_odom.prepare();
            {
                rosData.header.seq = count++;
                rosData.header.stamp = 0;
                rosData.header.frame_id = "a_frame";
                rosData.child_frame_id = "b_frame";

                rosData.pose.pose.position.x = 0.0;
                rosData.pose.pose.position.y = 0.0;
                rosData.pose.pose.position.z = 0.0;
                rosData.twist.twist.linear.x = 1;
                rosData.twist.twist.linear.y = 2;
                rosData.twist.twist.linear.z = 3;
                rosData.twist.twist.angular.x = 0;
                rosData.twist.twist.angular.y = 0;
                rosData.twist.twist.angular.z = 0;
            }

            yarp::rosmsg::std_msgs::Int8MultiArray& rosData2 = rosPublisherPort_arr.prepare();
            {
                rosData2.data = blob;
            }

            rosPublisherPort_odom.write();
            rosPublisherPort_arr.write();

            yarp::os::Time::delay(period);
        }

        //cleanup
        yCDebug(COMPANION) << "yarp topic-test completed";
        rosPublisherPort_odom.close();
        rosPublisherPort_arr.close();
        delete m_node;
        return 0;
    }

    else if (argc == 1 && std::string(argv[0]) == "--subscribe")
    {
        yCError(COMPANION) << "Missing topic name";
        return print_help();
    }

    else if (argc == 2 && std::string(argv[0]) == "--subscribe")
    {
        //parse command line
        std::string topicname = std::string(argv[1]);
        if (topicname.empty())
        {
            yCError(COMPANION) << "Invalid/missing topic name";
            return print_help();
        }

        //open the node
        yarp::os::Node* m_node = new yarp::os::Node("/testnode");
        if (m_node == nullptr)
        {
            yCError(COMPANION) << " opening " << "/testnode" << " Node, check your yarp-ROS network configuration\n";
            return 1;
        }

        //open the topics
        yarp::os::Subscriber<yarp::rosmsg::nav_msgs::Odometry>  rosPublisherPort_odom;
        yarp::os::Subscriber<yarp::rosmsg::std_msgs::Int8MultiArray>  rosPublisherPort_arr;
        std::string rosPublisherPort_odom_name = topicname + "_odom";
        std::string rosPublisherPort_arr_name = topicname + "_array";

        yCInfo(COMPANION) << " opening topics\n";
        if (!rosPublisherPort_odom.topic(rosPublisherPort_odom_name))
        {
            yCError(COMPANION) << " opening " << topicname << " Topic, check your yarp-ROS network configuration\n";
            return 1;
        }
        if (!rosPublisherPort_arr.topic(rosPublisherPort_arr_name))
        {
            yCError(COMPANION) << " opening " << topicname << " Topic, check your yarp-ROS network configuration\n";
            return 1;
        }
        yCInfo(COMPANION) << "Topics opened for subscribing\n";

        //subscriber loop
        signal(SIGINT, signal_callback_handler);
        while (!interrupt)
        {
            yarp::rosmsg::nav_msgs::Odometry* data1 = rosPublisherPort_odom.read(false);
            if (data1)
            {
                yCInfo(COMPANION) << "Data received on topic " << rosPublisherPort_odom_name;
            }
            yarp::rosmsg::std_msgs::Int8MultiArray* data2 = rosPublisherPort_arr.read(false);
            if (data2)
            {
                yCInfo(COMPANION) << "Data received on topic "<< rosPublisherPort_arr_name;
            }
            yarp::os::Time::delay(0.1);
        }

        //cleanup
        yCDebug(COMPANION) << "yarp topic-test completed";
        rosPublisherPort_odom.close();
        rosPublisherPort_arr.close();
        delete m_node;
        return 0;
    }

    yCInfo(COMPANION) << "Invalid command line";
    return print_help();
}
