
#include "tfModule.h"
#include <yarp/os/Nodes.h>
using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

tfModule::tfModule()
{
    rosMsgCounter   = 0;
    period          = PERIOD;     //ms
    rosNode         = 0;
}

tfModule::~tfModule()
{
    close();
}

bool tfModule::configure(ResourceFinder &rf)
{
    Time::turboBoost();
    ConstString configFile;
    int         rate;
    Port        startport;
    Bottle      cmd, response;
    string      localName, nodeName;
    double      start_time;
    Bottle      rosConf;

    rosConf = rf.findGroup("ROS");
    if (rosConf.isNull() || !rosConf.check("nodeName") || !rosConf.check("useSubscriber") || !rosConf.check("usePublisher"))
    {
            yError("wrong ros initialization parameter.. check your ini");
            return false;
    }

    nodeName            = rosConf.find("nodeName").asString();
    useSubscriber       = rosConf.find("useSubscriber").asBool();
    usePublisher       = rosConf.find("usePublisher").asBool();
    localName           = string(RPCPORTNAME);
    start_time          = yarp::os::Time::now();
    rate                = rf.check("rate", Value(20)).asInt(); //set the thread rate
    rosNode             = new yarp::os::Node(nodeName);

    yInfo("tfPublisher thread rate: %d ms.", rate);

    rpcPort.open((localName + "/rpc").c_str());
    attach(rpcPort);

    if (usePublisher && !rosPublisherPort_tf.topic(ROSTOPICNAM))
    {
        yError() << " Unable to publish data on " << ROSTOPICNAM << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    if (useSubscriber && !rosSubscriberPort_tf.topic(ROSTOPICNAM))
    {
        yError() << " Unable to subscribe to " << ROSTOPICNAM << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    if(rosConf.check("tfList"))
    {
        Bottle  list;
        string  name;
        int i;
        list = rosConf.findGroup("tfList");
        for(i = 1; i < list.size(); i++)//the 0 slot is for the group name
        {
            name = list.get(i).asString();
            if(rosConf.check(name))
            {
                Bottle tfCmd, reply, tfPar;
                tfPar = rosConf.findGroup(name); 
                tfCmd.addString("create_fixed_frame");
                tfCmd.append(tfPar);
                createFixedFrameCmd(tfCmd, reply);
                yInfo("%s",reply.get(0).asString().c_str());
            }
        }
    }

    return true;
}

bool tfModule::interruptModule()
{
    rpcPort.interrupt();
    rosPublisherPort_tf.interrupt();
    rosSubscriberPort_tf.interrupt();
    return true;
}

bool tfModule::respond(const Bottle& command, Bottle& reply)
{
    reply.clear();
    reply.addVocab(yarp::os::Vocab::encode("many"));
    string  request;
    request = command.get(0).asString();
    if ( request == "help")
    {
        return helpCmd(reply);
    }
    else if ( request == "create_fixed_frame")
    {

        if (command.size() != 10)
        {
            reply.addString("invalid params");
            return true;
        }

        if (rosHasFrame(command.get(2).asString(), command.get(3).asString()))
        {
            reply.addString("someone else is already injecting this frame");
            return true;
        }
        return createFixedFrameCmd(command, reply);
    }
    else if ( request == "delete_fixed_frame")
    {
        return deleteFixedFrameCmd(command, reply);
    }
    else if (request == "list")
    {
        return listCmd( reply );
    }
    else if (request == "get_frame")
    {
        return getFrameCmd(command.get(1).asString(), reply);
    }
    else if (request == "get_tf_chain")
    {
        if (!useSubscriber)
        {
            yError() << "Subscriber not active. activate it and restart";
            return false;
        }
    }
    reply.addString("Unknown command.");
    return true;
}

bool tfModule::helpCmd( Bottle& reply )
{
    reply.addString("Available commands are:");
    reply.addString("  "); reply.addString("  ");
    reply.addString("'create_fixed_frame'   -   usage: create_fixed_frame <frameName> <parentName> <childName> <x> <y> <z> <roll> <pitch> <yaw>");
    reply.addString("  ");
    reply.addString("'delete_fixed_frame'   -   usage: 1. delete_fixed_frame <frameName> 2. delete_fixed_frame <parentName> <childName>");
    reply.addString("  ");
    reply.addString("'list' output all current frames name ");
    reply.addString("  ");
    reply.addString("'get_frame' output specified frame's data  -  usage: get_frame <frameName> ");
    reply.addString("  ");
    reply.addString("'get_tf_chain' output tf summatory between two link  -  usage: get_tf_chain <parent> <child> ");
    return true;
}

bool tfModule::createFixedFrameCmd(const Bottle& command, Bottle& reply)
{
    tf        temp;
    double    x, y, z, roll, pitch, yaw;

    temp.name           = command.get(1).asString();
    temp.type           = tf::fixed;
    temp.parent_frame   = command.get(2).asString();
    temp.child_frame    = command.get(3).asString();
    x                   = command.get(4).asDouble();
    y                   = command.get(5).asDouble();
    z                   = command.get(6).asDouble();
    roll                = command.get(7).asDouble();
    pitch               = command.get(8).asDouble();
    yaw                 = command.get(9).asDouble();

    temp.transFromVec(x, y, z);
    temp.rotFromRPY(TORAD( roll ), TORAD( pitch ), TORAD( yaw ) );

    if (insertFixedFrame(temp))
    {
        reply.addString("frame " + temp.name + " succesfully created");
        return true;
    }
    else
    {
        reply.addString(log);
        return false;
    }
}

bool tfModule::insertFixedFrame(tf& tf_frame, bool external)
{
    vector<tf>& tfvec = external ? extTfVector : tfVector;
    std::pair<TF_MAP_PAIR> mapitem;
    for (size_t i = 0; i < tfvec.size(); i++)
    {
        if (tfvec[i].name == tf_frame.name)
        {
            log = "already exists!";
            return false;
        }

        if (
            tfvec[i].parent_frame == tf_frame.parent_frame && tfvec[i].child_frame == tf_frame.child_frame ||
            tfvec[i].parent_frame == tf_frame.child_frame && tfvec[i].child_frame == tf_frame.parent_frame
            )
        {
            log = "tf with the same hierarchy already exists!";
            return false;
        }
    }

    tf_frame.lifeTime = DEFAULTTFLIFETIME;
    tfvec.push_back(tf_frame);
    
    // finish down here for tree structure implementation..
    //mapitem.first     = tfvec.rbegin()->parent_frame + tfvec.rbegin()->child_frame;
    //mapitem.second    = &tfvec[tfvec.size()-1];
    //tfMap.insert(mapitem);
    
    return true;

}

bool tfModule::deleteFixedFrameCmd(const Bottle& command, Bottle& reply)
{
    bool    ret;
    int     cmdsize;
    cmdsize = int(command.size());
    switch ( cmdsize )
    {
        case 2:
            ret = deleteFrame( command.get(1).asString() );
            break;
        case 3:
            ret = deleteFrame( command.get(1).asString(), command.get(2).asString() );
            break;
        default:
            reply.addString("invalid params");
            return false;
    }

    if ( ret )
    {
        reply.addString("ok");
        return true;
    }
    else
    {
        reply.addString("not found");
        return false;
    }

}

bool tfModule::listCmd( Bottle& reply )
{
    string  tfCount;

    tfCount = to_string(tfVector.size());

    reply.addString( "there are currently "+ tfCount + " frame/s" );

    for (size_t i = 0; i < tfVector.size(); i++)
    {
            reply.addString( tfVector[i].name + ", " );
    }

    reply.addString("  "); reply.addString("ros Tf:"); reply.addString("  ");

    for (size_t i = 0; i < rosTf.size(); i++)
    {
        reply.addString(rosTf[i].header.frame_id + ", " + rosTf[i].child_frame_id);
    }


    return true;
}

bool tfModule::getFrameCmd(const string& name, Bottle& reply)
{
    for (size_t i = 0; i < tfVector.size(); i++)
    {
        if (tfVector[i].name == name)
        {
            replyFrameInfo( tfVector[i], reply );
        }
    }
    return true;
}

void tfModule::replyFrameInfo(const tf& frame, Bottle& reply)
{
    Vector rpyVec;
    string sep;
    sep = ", ";
    rpyVec = frame.getRPYRot();
    reply.addString( "Name: " + frame.name );
    reply.addString( "Parent: " + frame.parent_frame );
    reply.addString( "Child: " + frame.child_frame );
    reply.addString( "Traslation(X - Y - Z): " + TEXT( frame.tX ) + sep + TEXT( frame.tY ) + sep + TEXT( frame.tZ ) );
    reply.addString(
                    "Rotation(R - P - Y in degrees): " +
                    TEXT( TODEG( rpyVec[0] ) ) + sep +
                    TEXT( TODEG( rpyVec[1] ) ) + sep +
                    TEXT( TODEG( rpyVec[2] ) )
                   );
}

bool tfModule::deleteFrame( const string& name )
{
    bool ret;
    ret  = false;

    for (size_t i = 0; i < tfVector.size(); i++)
    {
        if ( tfVector[i].name == name )
        {
            tfVector.erase( tfVector.begin() + i );
            ret = true;
        }
    }
    return ret;
}

bool tfModule::deleteFrame( const string& parent, const string& child, bool external )
{
    bool ret;
    ret  = false;
    vector<tf>& tfvec = external ? extTfVector : tfVector;
    for (size_t i = 0; i < tfvec.size(); i++)
    {
        if (
            tfvec[i].parent_frame == parent && tfvec[i].child_frame == child ||
            tfvec[i].parent_frame == child && tfvec[i].child_frame == parent
           )
        {
            tfVector.erase( tfVector.begin() + i );
            ret = true;
        }
    }
    return ret;
}

bool tfModule::close()
{
    rpcPort.close();
    rosPublisherPort_tf.close();
    rosSubscriberPort_tf.close();
    if (rosNode)
    {
        delete  rosNode;
        rosNode = 0;
    }

    return true;
}

double tfModule::getPeriod()
{
    return period;
}

bool tfModule::rosHasFrame( string parent, string child )
{
    for (size_t i = 0; i < rosTf.size(); i++)
    {
        if (
            rosTf[i].header.frame_id == parent && rosTf[i].child_frame_id == child ||
            rosTf[i].header.frame_id == child && rosTf[i].child_frame_id == parent
           )
        {
            return true;
        }
    }

    return false;
}

void tfModule::importTf()
{
    for (size_t i = 0; i < rosTf.size(); i++)
    {
        string key;
        key = rosTf[i].header.frame_id + rosTf[i].child_frame_id;
        if (tfMap.count(key))
        {
            tf& currTf   = *tfMap.find(key)->second;
            currTf.tX    = rosTf[i].transform.translation.x;
            currTf.tY    = rosTf[i].transform.translation.y;
            currTf.tZ    = rosTf[i].transform.translation.z;
            currTf.rX    = rosTf[i].transform.rotation.x;
            currTf.rY    = rosTf[i].transform.rotation.y;
            currTf.rZ    = rosTf[i].transform.rotation.z;
            currTf.rW    = rosTf[i].transform.rotation.w;
        }
        else
        {
            tf newtf;
            newtf.name          = ROSNAMEPREFIX + key;
            newtf.parent_frame  = rosTf[i].header.frame_id;
            newtf.child_frame   = rosTf[i].child_frame_id;
            newtf.tX            = rosTf[i].transform.translation.x;
            newtf.tY            = rosTf[i].transform.translation.y;
            newtf.tZ            = rosTf[i].transform.translation.z;
            newtf.rX            = rosTf[i].transform.rotation.x;
            newtf.rY            = rosTf[i].transform.rotation.y;
            newtf.rZ            = rosTf[i].transform.rotation.z;
            newtf.rW            = rosTf[i].transform.rotation.w;
            insertFixedFrame(newtf, true);
        }
        
        // finish down here for tree structure implementation..
        //map<string, tf*>::iterator iter;
        //for (iter = tfMap.begin(); iter != tfMap.end(); iter++)
        //{
        //    tf& currTf = *(iter->second);
        //    double now = clock.now();
        //    if (now - currTf.birth > currTf.lifeTime)
        //    {
        //        deleteFrame(currTf.parent_frame, currTf.child_frame, true);
        //    }
        //}

    }
}

bool tfModule::updateModule()
{
    tf_tfMessage*                     rosInData  = rosSubscriberPort_tf.read( false );
    tf_tfMessage&                     rosOutData = rosPublisherPort_tf.prepare();
    geometry_msgs_TransformStamped    transform;
    unsigned int                      tfVecSize;

    if( rosInData != 0 )
    {
        rosTf = rosInData->transforms;
        importTf();
    }
    else
    {
        if (!extTfVector.empty())
        {
            extTfVector.clear();
        }
    }

    tfVecSize = tfVector.size();

    if (rosOutData.transforms.size() != tfVecSize)
    {
        rosOutData.transforms.resize(tfVecSize);
    }

    for( size_t i = 0; i < tfVecSize; i++ )
    {

        transform.child_frame_id            = tfVector[i].child_frame;
        transform.header.frame_id           = tfVector[i].parent_frame;
        transform.header.seq                = rosMsgCounter;
        transform.transform.rotation.x      = tfVector[i].rX;
        transform.transform.rotation.y      = tfVector[i].rY;
        transform.transform.rotation.z      = tfVector[i].rZ;
        transform.transform.rotation.w      = tfVector[i].rW;
        transform.transform.translation.x   = tfVector[i].tX;
        transform.transform.translation.y   = tfVector[i].tY;
        transform.transform.translation.z   = tfVector[i].tZ;

        rosOutData.transforms[i]            = transform;
    }
    rosPublisherPort_tf.write();
    rosMsgCounter++;
    return true;
}
