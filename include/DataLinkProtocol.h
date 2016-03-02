/*
 * DataLinkProtocol.h
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#pragma once 

#include "DeviceDriver.h"
#include "Queue.h"
#include <string>
#include <vector>

namespace ptc {

class NotifyCallback {
  public:
   virtual void operator()() = 0;
};

class DataLinkProtocol {
public:
	DataLinkProtocol(DeviceDriver* deviceDriver, NotifyCallback* notifyCB );
	void prepareAndSendMessage(std::string& message);
	virtual ~DataLinkProtocol();
private:
	void makeCRC(std::string& message, std::string& messageAndCRC);
	void checkCRC(std::string& message, std::string& messageAndCRC);
	void sendFrameThread();
	void receiveFrameThread();
	DeviceDriver * mDeviceDriver;
	NotifyCallback * mNotify_cb;
	Queue<std::string> mInputBuffer;
	Queue<std::string> mOutputBuffer;
};

} /* namespace ptc */

/* DATALINKPROTOCOL_H_ */
