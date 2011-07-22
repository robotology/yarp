// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "PortCtrlMod.h"

PortCtrlMod::PortCtrlMod(bool useCallback){
	_isInterfaceSet = false;
	_useCallback = useCallback;
}

double PortCtrlMod::getPeriod(){
	return 0.01;
}

bool PortCtrlMod::updateModule(){
	if(_isInterfaceSet)
		return _interfaceDriver->updateInterface();
	return true;
}

bool PortCtrlMod::respond(const Bottle& command, Bottle& reply){
	if(command.get(0).asString()=="quit"){
		return false;
	}else if(_isInterfaceSet){
		return _interfaceDriver->shellRespond(command,reply);
	} else return true;
}

void PortCtrlMod::onRead(Bottle &command){
	if(_isInterfaceSet)
		_interfaceDriver->onRead(command);
}

bool PortCtrlMod::configure(ResourceFinder &rf){
	_sendingPort = NULL;
	_receivingPort = NULL;
	attachTerminal();
	return true;
}

bool PortCtrlMod::interruptModule(){
	return true;
}

bool PortCtrlMod::close(){
	if(_isInterfaceSet){
		if(_receivingPort!=NULL) _receivingPort->close();
		if(_sendingPort!=NULL) _sendingPort->close();
		if(_sendingPort!=NULL) return _interfaceDriver->close();
		else return false;
	}else return true;
}

void PortCtrlMod::setInterfaceDriver(GenericYarpDriver *interfaceDriver){
	_isInterfaceSet = true;
	_interfaceDriver = interfaceDriver;
	if(_sendingPort!=NULL) _sendingPort = _interfaceDriver->getSendPort();
	if(_receivingPort!=NULL) _receivingPort = _interfaceDriver->getReceivePort();
	if(_useCallback && _receivingPort!=NULL) _receivingPort->useCallback(*this);
}