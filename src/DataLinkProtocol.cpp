/*
 * DataLinkProtocol.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#include "DataLinkProtocol.h"
#include "crc.h"

namespace ptc {

DataLinkProtocol::DataLinkProtocol(DeviceDriver* deviceDriver, NotifyCallback* notifyCB)
	: mDeviceDriver{deviceDriver}, mNotify_cb{notifyCB} {
	// TODO Auto-generated constructor stub
	if(deviceDriver==nullptr)
		throw "DeviceDriver is Nullpointer";
	if(notifyCB==nullptr)
		throw "Notifify Call Back is Nullpointer";
}

DataLinkProtocol::~DataLinkProtocol() {
	// TODO Auto-generated destructor stub
}

void DataLinkProtocol::makeCRC(std::string& message, std::string& messageAndCRC){

}

/**
 * Check if there is any sentinel byte.
 * Make needed operations, calculates CRC.
 * Insert in transmission buffer.
 */
void DataLinkProtocol::prepareAndSendMessage(std::string& message){

	std::string afterSentinel;
	this->makeCRC(message,afterSentinel);
	this->mOutputBuffer.push(afterSentinel);

}

} /* namespace ptc */
