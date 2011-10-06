/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "safe_manager.h"


SafeManager::SafeManager():action(MNOTHING)
{
}

SafeManager::~SafeManager()
{

}

bool SafeManager::prepare(Manager* lazy, 
						 yarp::os::Property* pConfig, ApplicationEvent* event)
{

	eventReceiver = event;
	m_pConfig = pConfig;

	if(pConfig->find("watchdog").asString() == "yes")
		enableWatchDog();
	else
		disableWatchod();

	if(pConfig->find("auto_dependency").asString() == "yes")
		enableAutoDependency();
	else
		disableAutoDependency();

	if(pConfig->find("auto_connect").asString() == "yes")
		enableAutoConnect();
	else
		disableAutoConnect();

	// making manager from lazy manager
	KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();
	ApplicaitonPContainer apps =  lazy_kb->getApplications();
	for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
		getKnowledgeBase()->addApplication((*itr));

	return true;
}



bool SafeManager::threadInit()
{
	return true;
}

void SafeManager::threadRelease()
{
}

void SafeManager::run()
{
	waitSemaphore();

	switch(action){ 
	case MRUN:{
			for(unsigned int i=0; i<modIds.size(); i++)
				Manager::run(modIds[i], true);
			break;
		}
	case MSTOP:{
			for(unsigned int i=0; i<modIds.size(); i++)
				Manager::stop(modIds[i], true);
			break;
		}
	case MKILL:{
			for(unsigned int i=0; i<modIds.size(); i++)
				Manager::kill(modIds[i], true);
			break;
		}
	case MCONNECT:{
			for(unsigned int i=0; i<conIds.size(); i++)
				if(Manager::connect(conIds[i]) && eventReceiver)
					eventReceiver->onConConnect(conIds[i]);
			break;
		}
	case MDISCONNECT:{
			for(unsigned int i=0; i<conIds.size(); i++)
				if(Manager::disconnect(conIds[i]) && eventReceiver)
					eventReceiver->onConDisconnect(conIds[i]);
			break;
		}

	case MREFRESH:{
			for(unsigned int i=0; i<modIds.size(); i++)
			{
				if(Manager::running(modIds[i]))
				{
					if(eventReceiver) eventReceiver->onModStart(modIds[i]);
				}
				else
				{
					if(eventReceiver) eventReceiver->onModStop(modIds[i]);
				}
			}

			for(unsigned int i=0; i<conIds.size(); i++)
			{
				if(Manager::connected(conIds[i]))
				{
					if(eventReceiver) eventReceiver->onConConnect(conIds[i]);
				}
				else
				{
					if(eventReceiver) eventReceiver->onConDisconnect(conIds[i]);
				}
			}
			for(unsigned int i=0; i<resIds.size(); i++)
			{
				if(Manager::exist(resIds[i]))
				{
					if(eventReceiver) eventReceiver->onResAvailable(resIds[i]);
				}
				else
				{
					if(eventReceiver) eventReceiver->onResUnAvailable(resIds[i]);
				}
			}
			break;
		}

	default:
		break;
	};

	postSemaphore();

}

void SafeManager::safeRun(std::vector<int>& MIDs)
{
	modIds = MIDs;
	action = MRUN;
	yarp::os::Thread::start();
}

void SafeManager::safeStop(std::vector<int>& MIDs)
{
	modIds = MIDs;
	action = MSTOP;
	yarp::os::Thread::start();
}

void SafeManager::safeKill(std::vector<int>& MIDs)
{
	modIds = MIDs;
	action = MKILL;
	yarp::os::Thread::start();

}


void SafeManager::safeConnect(std::vector<int>& CIDs)
{
	conIds = CIDs;
	action = MCONNECT;
	yarp::os::Thread::start();
}


void SafeManager::safeDisconnect(std::vector<int>& CIDs)
{
	conIds = CIDs;
	action = MDISCONNECT;
	yarp::os::Thread::start();
}


void SafeManager::safeRefresh(std::vector<int>& MIDs, 
					 std::vector<int>& CIDs, 
					 std::vector<int>& RIDs)
{
	modIds = MIDs;
	conIds = CIDs;
	resIds = RIDs;
	action = MREFRESH;
	yarp::os::Thread::start();
}

void SafeManager::onExecutableStart(void* which)
{
	Executable* exe = static_cast<Executable*>(which);
	if(eventReceiver && exe)
		eventReceiver->onModStart(exe->getID());
}

void SafeManager::onExecutableStop(void* which)
{
	Executable* exe = static_cast<Executable*>(which);
	if(eventReceiver && exe)
		eventReceiver->onModStop(exe->getID());

}

void SafeManager::onExecutableDied(void* which)
{
	Executable* exe = static_cast<Executable*>(which);
	if(eventReceiver && exe)
		eventReceiver->onModStop(exe->getID());
}


void SafeManager::onExecutableFailed(void* which)
{	
	ErrorLogger* logger  = ErrorLogger::Instance();	
	Executable* exe = static_cast<Executable*>(which);
	if(exe)
	{
		if(m_pConfig->find("module_failure").asString() == "prompt")
		{
			ostringstream err;
			err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"]";
			logger->addError(err);
			if(eventReceiver && exe)
				eventReceiver->onModStop(exe->getID());
		}

		if(m_pConfig->find("module_failure").asString() == "recover")
		{
			ostringstream err;
			err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (restarting...)";
			logger->addError(err);
			exe->start();
		 }

		if(m_pConfig->find("module_failure").asString() == "terminate")
		{
			ostringstream err;
			err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (terminating...)";
			logger->addError(err);
			Manager::stop();
		}
	}

	if(eventReceiver)
			eventReceiver->onError();
}


void SafeManager::onCnnStablished(void* which)
{

}


void SafeManager::onCnnFailed(void* which)
{
	ErrorLogger* logger  = ErrorLogger::Instance();	
	Connection* cnn = static_cast<Connection*>(which);
	if(cnn)
	{
		if( m_pConfig->find("connection_failure").asString() == "prompt")
		{
			ostringstream err;
			err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to();
			logger->addError(err);
		}

		if(m_pConfig->find("connection_failure").asString() == "terminate")
		{
			ostringstream err;
			err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<" (terminating...)";
			logger->addError(err);
			Manager::stop();
		}
	}

	if(eventReceiver)
		eventReceiver->onError();
}


