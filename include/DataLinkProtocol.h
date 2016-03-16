/*
 * DataLinkProtocol.h
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#pragma once 

#include "DeviceDriver.h"
#include "Queue.h"
#include "DatagramInterface.h"
#include <string>
#include <vector>

namespace ptc {

class DataLinkProtocol {
public:
	void receiveThread();
	DataLinkProtocol(DeviceDriver* deviceDriver, DatagramInterface* interface);
	virtual ~DataLinkProtocol();
	void sendThread();
	void sendMessage(char* message,size_t messageSize);
/*	Queue<std::pair<char*,size_t>> * getInputBuffer(){
		return &mInputBuffer;
	}*/
	Queue<std::pair<char*,size_t>> * getOutputBuffer(){
		return &mOutputBuffer;
	}
private:
	void resendLastFrame();
	std::pair<char*,size_t> prepareMessage(uint8_t sequence, uint8_t tipo, uint8_t protocol,  char* message,size_t messageSize);
	void updateLastAck(uint8_t lastAck);
	void validateAndStoreFrame(char * frame, size_t frameSize);
//	Queue<std::pair<char*,size_t>> mInputBuffer;
	Queue<std::pair<char*,size_t>> mOutputBuffer;
	DeviceDriver * mDeviceDriver;
	DatagramInterface * mDatagramInterface;
	volatile uint8_t lastAck;
	volatile uint8_t lastReceived;
	uint8_t protocolo;
	std::pair<char*,size_t> lastFrameSent;
};

} /* namespace ptc */

/* DATALINKPROTOCOL_H_ */
