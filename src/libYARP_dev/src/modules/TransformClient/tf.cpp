
#include "tf.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

tf::tf()
{
    //tfport  = 0;
    type    = fixed;
    tX = tY = tZ = rX = rY = rZ = rW = 0;
}

tf::tf
(
    const string& inName,
    const string& parent,
    const string& child,
    double        inTX,
    double        inTY,
    double        inTZ,
    double        inRX,
    double        inRY,
    double        inRZ,
    double        inRW
)
{
    name            = inName;
    parent_frame    = parent;
    child_frame     = child;
    tX              = inTX;
    tY              = inTY;
    tZ              = inTZ;
    rX              = inRX;
    rY              = inRY;
    rZ              = inRZ;
    rW              = inRW;
}

tf::~tf()
{
    /*if (tfport)
    {
        delete  tfport;
        tfport  = 0;
    }*/
}

//void tf::read()
//{
//    if (tfport)
//    {
//        Bottle* bot;
//        bot     = tfport->read(false);
//        if (bot)
//        {
//            parent_frame = bot->get(0).asString();
//            child_frame  = bot->get(1).asString();
//            tX           = bot->get(2).asDouble();
//            tY           = bot->get(3).asDouble();
//            tZ           = bot->get(4).asDouble();
//            rX           = bot->get(5).asDouble();
//            rY           = bot->get(6).asDouble();
//            rZ           = bot->get(7).asDouble();
//            rW           = bot->get(8).asDouble();
//        }
//    }
//}
void tf::transFromVec(double X, double Y, double Z)
{
    tX = X;
    tY = Y;
    tZ = Z;
}
void tf::rotFromRPY(double R, double P, double Y)
{
    double    rot[3] = {R, P, Y};
    size_t    i;
    Vector    rotV, rotQ;
    Matrix    rotM;
    i         = 3;
    rotV      = Vector(i, rot);
    //rotM      = rpy2dcm(rotV);
    //rotQ      = dcm2quat(rotM);
    rW        = rotQ[X_AX];
    rX        = rotQ[Y_AX];
    rY        = rotQ[Z_AX];
    rZ        = rotQ[W_AX];
    return;
}
Vector tf::getRPYRot() const
{
    Vector rotQ, rotV;
    size_t i;
    Matrix rotM;
    double rot[4] = { rW, rX, rY, rZ };
    
    i    = 4;
    rotQ = Vector( i, rot );
    //rotM = quat2dcm( rotQ );
    //rotV = dcm2rpy( rotM );
    
    return rotV;
}

