/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "executable.h"

Executable::Executable(Broker* _broker, 
					MEvent* _event, 
					bool _bWatchDog) : RateThread(DEF_PERIOD)
{
	bAutoConnect = true;
	broker = _broker;
	event = _event;
	bWatchDog = _bWatchDog;
	setState(SUSPENDED);
	logger  = ErrorLogger::Instance();
}

Executable::~Executable()
{
	
}

bool Executable::threadInit()
{
	__CHECK_NULLPTR(broker);
	__CHECK_NULLPTR(event);
	bool ret = broker->init(strCommand.c_str(),
						strParam.c_str(),
						strHost.c_str(),
						strStdio.c_str(),
						strWorkdir.c_str(),
						strEnv.c_str());
	if(!ret)
	{
		ostringstream msg;
		msg<<"cannot initialize broker. : ";
		if(broker->error())
			msg<<broker->error();
		logger->addError(msg);
		setState(DEAD);
		return false;
	}
	return true;
}


void Executable::afterStart(bool s)
{
}


void Executable::run()
{
	switch(getState()) {

		case SUSPENDED:
		{
			RateThread::stop();
			break;
		}
		
		case READY:
		{	
			//cout<<strCommand<<": READY"<<endl;
			if(checkPriorityPorts())
			{
				if(runModule())
					setState(CONNECTING);
				else
					setState(DEAD);
			}
			break;
		}

		case CONNECTING:
		{
			//cout<<strCommand<<": CONNECTING"<<endl;
			if(checkNormalPorts())
			{
				if(connectAllPorts())
				{
					setState(RUNNING);
					event->onExecutableStart(this);
				}
				else
					setState(DYING);
			}
			break;
		}

		case RUNNING:
		{
			//cout<<strCommand<<": RUNNING"<<endl;
			if(bWatchDog)
			{
				RateThread::setRate(WDOG_PERIOD);
				watchModule();
			}
			else
				RateThread::stop();
			break;
		}

		case DYING:
		{
			//cout<<strCommand<<": DYING"<<endl;
			if(stopModule())
			{
				event->onExecutableStop(this);
				setState(SUSPENDED);
				RateThread::stop();
			}
			else
				setState(DEAD);
			break;
		}

		default: //DEAD case
		{
			//cout<<strCommand<<": DEAD"<<endl;
			Executable::kill();
			break;
		}		
	};
}


void Executable::threadRelease()
{
	
}


bool Executable::start(void)
{
	if((getState() == RUNNING))	
		return true;
		
	if((getState() != SUSPENDED) &&
	   (getState() != DEAD))
	{
		ostringstream msg;
		msg<<strCommand<<" can be executed only from suspended or dead.";
		logger->addWarning(msg);
		return true;
	}
	
	RateThread::setRate(DEF_PERIOD);
	setState(READY);
	RateThread::start();
	return true;
}


void Executable::stop(void)
{
	if((getState() == SUSPENDED) ||
	   (getState() == DEAD) ||
	   (getState() == DYING))
	{
		ostringstream msg;
		msg<<strCommand<<" is dying or already suspended.";
		logger->addWarning(msg);		
		return;
	}
	
	if((getState() == RUNNING))
	{
		setState(DYING);
		if(!RateThread::isRunning())
		{
			RateThread::setRate(DEF_PERIOD);
			RateThread::start();
		}
		return;
	}

	ostringstream msg;
	msg<<strCommand<<" is not in running state. trying to kill it.";
	logger->addWarning(msg);		
	Executable::kill();
}


void Executable::kill(void)
{
	if(RateThread::isRunning())
		RateThread::stop();
	broker->kill();
	setState(DEAD);
	event->onExecutableDied(this);	
}


void Executable::setState(RSTATE st)
{
	safeState.wait();
	status = st;
	safeState.post();	
}


RSTATE Executable::getState(void)
{
	RSTATE currentState;
	safeState.wait();
	if((status == RUNNING) && !broker->running())
		currentState = DEAD;
	else
		currentState = status;
	safeState.post();
	return currentState;
}


/**
 * checks if all the priority-required ports exist.  
 */
bool Executable::checkPriorityPorts(void)
{
	if(!bAutoConnect)
		return true;

	CnnIterator itr;
	for(itr=connections.begin(); itr!=connections.end(); itr++)
		if((*itr).withPriority() 
			&& !broker->exists((*itr).from()))
			return false;	
	return true;
}


/**
 * checks if all the required ports exist.  
 */
bool Executable::checkNormalPorts(void)
{
	if(!bAutoConnect)
		return true;

	CnnIterator itr;
	for(itr=connections.begin(); itr!=connections.end(); itr++)
	{
		if(!broker->exists((*itr).to()) || 
			!broker->exists((*itr).from()))
			return false;	
	}
	return true;
}


bool Executable::connectAllPorts(void)
{
	if(!bAutoConnect)
		return true;

	CnnIterator itr;
	for(itr=connections.begin(); itr!=connections.end(); itr++)
	{
		if( !broker->connect((*itr).from(), (*itr).to(), 
						carrierToStr((*itr).carrier())) )
		{
			ostringstream msg;
			msg<<"cannot connect "<<(*itr).from() <<" to "<<(*itr).to();
			if(broker->error())
				msg<<" : "<<broker->error();
			logger->addError(msg);
			return false;
		}
		else
			event->onCnnStablished(&(*itr));
	}
	return true;	
}

bool Executable::runModule(void)
{
	if(!broker->run())
	{
		ostringstream msg;
		msg<<"cannot run "<<strCommand<<" on "<<strHost;
		if(broker->error())
			msg<<" : "<<broker->error();
		logger->addError(msg);
		return false;
	}
	return true;
}

bool Executable::stopModule(void)
{
	if(!broker->stop())
	{
		ostringstream msg;
		msg<<"cannot stop "<<strCommand<<" on "<<strHost;
		if(broker->error())
			msg<<" : "<<broker->error();
		logger->addError(msg);
		return false;
	}
	return true;
}


void Executable::watchModule(void)
{
	if(!broker->running())
	{
		setState(DEAD);
		return;
	}

	if(bAutoConnect)
	{
		CnnIterator itr;
		for(itr=connections.begin(); itr!=connections.end(); itr++)
			if( !broker->connected((*itr).from(), (*itr).to()) ) 
				event->onCnnFailed(&(*itr));	
	}
}


