/*
 *  Yarp Modules Manager
 *  Copyright: Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "manager.h"
#include "yarpbroker.h"
#include "localbroker.h"
#include "xmlapploader.h"
#include "singleapploader.h"


#define RSK_TIMEOUT			5		// Run, Stop and kill timeout in seconds
#define BROKER_YARPRUN		"yarprun"


/**
 * Class Manager
 */

Manager::Manager(bool withWatchDog) : MEvent() 
{
	logger  = ErrorLogger::Instance();
	bWithWatchDog = withWatchDog;
	bAutoDependancy = false;
	bAutoDependancy = true;
	strDefBroker = BROKER_YARPRUN;	
	knowledge.createFrom(NULL, NULL);
}

Manager::Manager(const char* szModPath,
			const char* szAppPath, bool withWatchDog)
{
	logger  = ErrorLogger::Instance();
	bWithWatchDog = withWatchDog;
	bAutoDependancy = false;
	bAutoDependancy = true;
	strDefBroker = BROKER_YARPRUN;	

	XmlModLoader modload(szModPath, NULL);
	XmlModLoader* pModLoad = &modload;
	if(!modload.init())
		pModLoad = NULL;

	XmlAppLoader appload(szAppPath, NULL);
	XmlAppLoader* pAppLoad = &appload;
	if(!appload.init())
		pAppLoad = NULL;
	
	knowledge.createFrom(pModLoad, pAppLoad);
}


Manager::~Manager()
{
	clearExecutables();	
}


bool Manager::addApplication(const char* szFileName)
{
	XmlAppLoader appload(szFileName);
	if(!appload.init())
		return false; 
	Application* application = appload.getNextApplication();
	if(!application)
		return false;
	return knowledge.addApplication(application);
}


bool Manager::addApplications(const char* szPath)
{
	XmlAppLoader appload(szPath, NULL);
	if(!appload.init())
		return false; 
	Application* application;
	while((application = appload.getNextApplication()))
		knowledge.addApplication(application);
	return true;
}


bool Manager::addModule(const char* szFileName)
{
	XmlModLoader modload(szFileName);
	if(!modload.init())
		return false; 
	Module* module = modload.getNextModule();
	if(!module)
		return false;
	return knowledge.addModule(module);
}


bool Manager::addModules(const char* szPath)
{
	XmlModLoader modload(szPath, NULL);
	if(!modload.init())
		return false; 
	Module* module;
	while((module = modload.getNextModule()))
		knowledge.addModule(module);
	return true;
}



bool Manager::loadApplication(const char* szAppName)
{
	__CHECK_NULLPTR(szAppName);
	
	if(allRunning())
	{
		logger->addError("Please stop current running application first.");
		return false;
	}
	strAppName = szAppName;
	if(!knowledge.reasolveDependency(szAppName, bAutoDependancy))
		return false;
	
	return prepare();
}

bool Manager::prepare(void)
{
	clearExecutables();
	connections.clear();
	modules.clear();
	resources.clear();

	connections = knowledge.getSelConnection();
	modules = knowledge.getSelModules();
	resources = knowledge.getSelResources();
	
	/**
	 *  TODO: we need to initialize a module with a local broker if the
	 *  host property is empty. 
	 *  Thus we can use specified broker for remote execution and a local 
	 *  broker for local execution.
	 *
	 *  Resources should also be added to the relevant executable. up to now
	 *  all of them will be handled by manager. 
	 */ 
	ModulePIterator itr;
	for(itr=modules.begin(); itr!=modules.end(); itr++)
	{

		string strCurrentBroker;		 
		if(compareString((*itr)->getBroker(), BROKER_YARPRUN))
			strCurrentBroker = BROKER_YARPRUN;
		else
			strCurrentBroker = strDefBroker;

		Broker* broker = NULL;
		if(strCurrentBroker == string(BROKER_YARPRUN))
			broker = new YarpBroker;
		//else if( for other brokers )
		//...

		/**
		 * using default broker if it is still NULL
		 */
		if(!broker)
		{
			ostringstream war;
			war<<"Broker "<<strCurrentBroker<<" does not exist! (using default broker)";
			logger->addWarning(war);
			broker = new YarpBroker;
			strCurrentBroker = BROKER_YARPRUN;
		}

		Executable* exe = new Executable(broker, (MEvent*)this, 
										bWithWatchDog);
		exe->setCommand((*itr)->getName());
		exe->setParam((*itr)->getParam());
		exe->setHost((*itr)->getHost());
		exe->setStdio((*itr)->getStdio());
		exe->setWorkDir((*itr)->getWorkDir());
		if(strCurrentBroker == string(BROKER_YARPRUN))
		{
			string env = string("YARP_PORT_PREFIX=") + 
							string((*itr)->getPrefix());
			exe->setEnv(env.c_str());
		}
		
		/**
		 * Adding connections to their owners 
		 */
		CnnIterator cnn;
		for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
			if((*cnn).owner() == (*itr))
			{
				exe->addConnection(*cnn);
				//connections.erase(cnn);
			}

		runnables.push_back(exe);
	}

	return true;
}


bool Manager::checkDependency(void)
{
	/**
	 * checking for port resources availability
	 * TODO:later it should change to use proper broker for resource cheking.
	 * 		up to now, we use only yraprun for checking port resources		
	 */
	bool ret = true;
	ResourcePIterator itrRes; 
	YarpBroker resChecker; 
	for(itrRes=resources.begin(); itrRes!=resources.end(); itrRes++)
	{
		ResYarpPort* res = (*itrRes); 
		if(!resChecker.exists(res->getPort()))
		{
			ret = false;
			ostringstream err;
			err<<"Port "<<res->getPort()<<" is not available!";
			logger->addError(err);
		}
	}

	return ret;
}


bool Manager::run(unsigned int id)
{
	if(runnables.empty())
	{
		logger->addError("Application is not loaded.");
		return false;
	}
	
	if(id>=runnables.size())
	{
		logger->addError("Module id is out of range.");
		return false;
	}

	runnables[id]->disableAutoConnect();
	runnables[id]->start(); 

	/* waiting for running */
	int retry = 0;
	while(!running(id) && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);
	
	if(!running(id))
	{
		ostringstream msg;
		msg<<"Failed to run "<<runnables[id]->getCommand();
		msg<<" on "<<runnables[id]->getHost();
		msg<<". (State: "<<runnables[id]->state();
		msg<<", paramete: "<<runnables[id]->getParam()<<")";
		logger->addError(msg);
		return false;
	}

	return true;

}

bool Manager::run()
{
	if(runnables.empty())
	{
		logger->addError("Application is not loaded.");
		return false;
	}

	if(!checkDependency())
	{
		logger->addError("Some of external ports dependency are not satisfied.");
		return false;
	}

	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
	{
		if(bAutoConnect)
			(*itr)->enableAutoConnect();
		else
			(*itr)->disableAutoConnect();
		(*itr)->start();
	}

	/* waiting for running */
	int retry = 0;
	while(!allRunning() && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);
	
	if(!allRunning())
	{
		ExecutablePIterator itr;
		for(itr=runnables.begin(); itr!=runnables.end(); itr++)
			if((*itr)->state() != RUNNING)
			{
				ostringstream msg;
				msg<<"Failed to run "<<(*itr)->getCommand();
				msg<<" on "<<(*itr)->getHost();
				msg<<". (State: "<<(*itr)->state();
				msg<<", paramete: "<<(*itr)->getParam()<<")";
				logger->addError(msg);
			}
		kill();
		return false;
	}

	/* connecting extra ports*/
	if(bAutoConnect)
		if(!connectExtraPorts())
		{
			logger->addError("Failed to stablish some of connections.");
			return false;
		}

	return true;
}

bool Manager::stop(unsigned int id)
{
	if(runnables.empty())
	{
		logger->addError("Application is not loaded.");
		return false;
	}
	
	if(id>=runnables.size())
	{
		logger->addError("Module id is out of range.");
		return false;
	}

	runnables[id]->stop(); 

	/* waiting for stopping */
	int retry = 0;
	while(running(id) && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);
	
	if(running(id))
	{
		ostringstream msg;
		msg<<"Failed to stop "<<runnables[id]->getCommand();
		msg<<" on "<<runnables[id]->getHost();
		msg<<". (State: "<<runnables[id]->state();
		msg<<", paramete: "<<runnables[id]->getParam()<<")";
		logger->addError(msg);
		return false;
	}

	return true;

}


bool Manager::stop()
{
	if(runnables.empty())
		return true;

	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
		(*itr)->stop();

	/* waiting for stoping */
	int retry = 0;
	while(!allStopped() && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);
	
	if(!allStopped())
	{
		ExecutablePIterator itr;
		for(itr=runnables.begin(); itr!=runnables.end(); itr++)
			if( ((*itr)->state() != SUSPENDED) && 
				((*itr)->state() != DEAD))
			{
				ostringstream msg;
				msg<<"Failed to stop "<<(*itr)->getCommand();
				msg<<" on "<<(*itr)->getHost();
				msg<<". (State: "<<(*itr)->state();
				msg<<", paramete: "<<(*itr)->getParam()<<")";
				logger->addError(msg);
			}
		return false;
	}
	
	return true;
}

bool Manager::kill(unsigned int id)
{
	if(runnables.empty())
	{
		logger->addError("Application is not loaded.");
		return false;
	}
	
	if(id>=runnables.size())
	{
		logger->addError("Module id is out of range.");
		return false;
	}

	runnables[id]->kill(); 

	/* waiting for killing */
	int retry = 0;
	while(running(id) && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);
	
	if(running(id))
	{
		ostringstream msg;
		msg<<"Failed to kill "<<runnables[id]->getCommand();
		msg<<" on "<<runnables[id]->getHost();
		msg<<". (State: "<<runnables[id]->state();
		msg<<", paramete: "<<runnables[id]->getParam()<<")";
		logger->addError(msg);
		return false;
	}

	return true;
}


bool Manager::kill()
{
	if(runnables.empty())
		return true;

	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
		(*itr)->kill();

	/* waiting for stoping */
	int retry = 0;
	while(!allStopped() && (retry++ < (RSK_TIMEOUT*10)) ) 
		yarp::os::Time::delay(0.1);

	if(!allStopped())
	{
		ExecutablePIterator itr;
		for(itr=runnables.begin(); itr!=runnables.end(); itr++)
			if( ((*itr)->state() != SUSPENDED) && 
				((*itr)->state() != DEAD))
			{
				ostringstream msg;
				msg<<"Failed to kill "<<(*itr)->getCommand();
				msg<<" on "<<(*itr)->getHost();
				msg<<". (State: "<<(*itr)->state();
				msg<<", paramete: "<<(*itr)->getParam()<<")";
				logger->addError(msg);
			}
		return false;
	}

	return true;
}


void Manager::clearExecutables(void)
{
	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
	{
		if((*itr)->getBroker())
			delete (*itr)->getBroker();
		delete (*itr);
	}
	runnables.clear();
}

bool Manager::connect(unsigned int id)
{
	if(id>=connections.size())
	{
		logger->addError("Connection id is out of range.");
		return false;
	}

	YarpBroker connector; 
	connector.init();

	if( !connector.connect(connections[id].from(), 
							connections[id].to(),
					carrierToStr(connections[id].carrier())) )
	{
		logger->addError(connector.error());
		//cout<<connector.error()<<endl;
		return false;
	}	
	
	return true;
}

bool Manager::connect(void)
{
	YarpBroker connector; 
	connector.init();
	CnnIterator cnn;
	for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
		if( !connector.connect((*cnn).from(), (*cnn).to(),
						carrierToStr((*cnn).carrier())) )
			{
				logger->addError(connector.error());
				//cout<<connector.error()<<endl;
				return false;
			}	
	return true;
}

bool Manager::disconnect(unsigned int id)
{
	if(id>=connections.size())
	{
		logger->addError("Connection id is out of range.");
		return false;
	}

	YarpBroker connector; 
	connector.init();

	if( !connector.disconnect(connections[id].from(), 
							connections[id].to()) )
	{
		logger->addError(connector.error());
		//cout<<connector.error()<<endl;
		return false;
	}	
	
	return true;
}

bool Manager::disconnect(void)
{
	YarpBroker connector; 
	connector.init();
	CnnIterator cnn;
	for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
		if( !connector.disconnect((*cnn).from(), (*cnn).to()) )
			{
				logger->addError(connector.error());
				//cout<<connector.error()<<endl;
				return false;
			}	
	return true;
}


bool Manager::connected(unsigned int id)
{
	if(id>=connections.size())
	{
		logger->addError("Connection id is out of range.");
		return false;
	}

	YarpBroker connector; 
	connector.init();
	return connector.connected(connections[id].from(), 
							connections[id].to());
}


bool Manager::connected(void)
{
	YarpBroker connector; 
	connector.init();
	CnnIterator cnn;
	bool bConnected = true;
	for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
		if( !connector.connected((*cnn).from(), (*cnn).to()) )
			bConnected = false;
	return bConnected;
}


bool Manager::connectExtraPorts(void)
{
	YarpBroker connector; 
	connector.init();
	CnnIterator cnn;
	for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
		if(!(*cnn).owner() )
		{
			if( !connector.connect((*cnn).from(), (*cnn).to(),
						carrierToStr((*cnn).carrier())) )
			{
				logger->addError(connector.error());
				//cout<<connector.error()<<endl;
				return false;
			}	
		}
	return true;
}

bool Manager::running(unsigned int id)
{
	if(id>=runnables.size())
	{
		logger->addError("Module id is out of range.");
		return false;
	}

	if(runnables[id]->state() != RUNNING)
			return false;
	return true;
}


bool Manager::allRunning(void)
{
	if(!runnables.size())
		return false;
	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
		if((*itr)->state() != RUNNING)
			return false;
	return true;
}


bool Manager::allStopped(void)
{
	if(!runnables.size())
		return true; 
	ExecutablePIterator itr;
	for(itr=runnables.begin(); itr!=runnables.end(); itr++)
		if( ((*itr)->state() != SUSPENDED) && 
			((*itr)->state() != DEAD))
			return false;
	return true;
}


void Manager::onExecutableStart(void* which) {}
void Manager::onExecutableStop(void* which)  {}
void Manager::onCnnStablished(void* which) {}
void Manager::onExecutableDied(void* which) {}
void Manager::onCnnFailed(void* which) {}


/*
bool Manager::loadModule(const char* szModule, const char* szHost)
{
	__CHECK_NULLPTR(szModule);
	strAppName = szModule;

	SingleAppLoader appLoader(szModule, szHost);
	if(!appLoader.init())
	{
		logger->addError("Error initializing SingleAppLoader.");
		return false;
	}
	
	if(!createKnowledgeBase(appLoader))
	{
		logger->addError("Cannot create knowledge base");
		return false;
	}
	
	return prepare();

}
*/


