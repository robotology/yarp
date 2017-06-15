#include <yarp/dev/IJoypadController.h>
#include <yarp/os/LogStream.h>
using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
#define MESSAGE_PREFIX "IJoypadController:"
#define myInfo() yInfo() << MESSAGE_PREFIX
#define myError() yError() << MESSAGE_PREFIX
#define myDebug() yDebug() << MESSAGE_PREFIX
#define myWarning() yWarning() << MESSAGE_PREFIX
#define buttActionGroupName "BUTTON_EXECUTE"

bool yarp::dev::IJoypadController::executeAction(int action_id)
{
    if (m_actions.find(action_id) != m_actions.end())
    {
        myInfo() << "executing script" << action_id << ":" << m_actions[action_id];
        system(m_actions[action_id].c_str());
    }
    else
    {
        myWarning() << "no scripts associated to button" << action_id;
        return false;
    }
    return true;
}

bool yarp::dev::IJoypadController::parseActions(const yarp::os::Searchable& cfg, int* count)
{
    int  dummy;
    int  i;
    int& actCount = count ? *count : dummy;
    if(!cfg.check(buttActionGroupName))
    {
        myInfo() << "no actions found in the configuration file (no" << buttActionGroupName << "group found)";
        actCount = 0;
        return true;
    }
    Bottle& actionsGroup = cfg.findGroup(buttActionGroupName);

    if(!actionsGroup.size())
    {
        myError() << "no action found under" << buttActionGroupName << "group";
        actCount = 0;
        return false;
    }

    for(i = 1; i < actionsGroup.size(); i++)
    {
        if(!actionsGroup.get(i).isList())
        {
            yDebug() << "error parsing cfg";
            return false;
        }

        Bottle& keyvalue         = *actionsGroup.get(i).asList();
        yDebug() << keyvalue.toString();
        unsigned int buttonCount;
        if(!this->getButtonCount(buttonCount))
        {
            myError() << "unable to get button count while parsing the actions";
            actCount = 0;
            return false;
        }
        if(!keyvalue.get(0).isInt()                 ||
            keyvalue.get(0).asInt() < 0             ||
            keyvalue.get(0).asInt() > buttonCount-1 ||
           !keyvalue.get(1).isString())
        {
            myError() << "Button's actions parameters must be in the format 'unsigned int string' and the button id must be in range";
            actCount = 0;
            return false;
        }
        myInfo() << "assigning actions" << keyvalue.get(1).asString() << "to button" << keyvalue.get(0).asInt();
        m_actions[keyvalue.get(0).asInt()] = keyvalue.get(1).asString();
    }

    actCount = i;
    myInfo() << actCount << "action parsed succesfully";
    return true;
}
