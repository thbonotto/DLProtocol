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
	const char PDU=0x00;
	const char ACK=0x01;
	const char NACK=0x02;
	void receiveThread();
	DataLinkProtocol(DeviceDriver* deviceDriver, NotifyCallback* notifyCB );
	virtual ~DataLinkProtocol();
	void sendThread();
	void sendMessage(char* message,size_t messageSize);
private:
	void resendLastFrame();
	std::pair<char*,size_t> prepareMessage(uint8_t sequence, uint8_t tipo, uint8_t protocol,  char* message,size_t messageSize);
	void updateLastAck(uint8_t lastAck);
	void validateAndStoreFrame(char * frame, size_t frameSize);
	Queue<std::pair<char*,size_t>> mInputBuffer;
	Queue<std::pair<char*,size_t>> mOutputBuffer;
	DeviceDriver * mDeviceDriver;
	NotifyCallback * mNotify_cb;
	volatile uint8_t lastAck;
	volatile uint8_t lastReceived;
	uint8_t protocolo;
	std::pair<char*,size_t> lastFrameSent;
};

} /* namespace ptc */

/* DATALINKPROTOCOL_H_ */
