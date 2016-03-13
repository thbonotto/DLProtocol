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
	const char PDU=0x00;
	const char ACK=0x01;
	const char NACK=0x02;
	void receiveThread();
	DataLinkProtocol(DeviceDriver* deviceDriver);
	virtual ~DataLinkProtocol();
	void sendThread();
	void sendMessage(char* message,size_t messageSize);
	Queue<std::pair<char*,size_t>> * getInputBuffer(){
		return &mInputBuffer;
	}
	Queue<std::pair<char*,size_t>> * getOutputBuffer(){
		return &mOutputBuffer;
	}
private:
	void resendLastFrame();
	std::pair<char*,size_t> prepareMessage(uint8_t sequence, uint8_t tipo, uint8_t protocol,  char* message,size_t messageSize);
	void updateLastAck(uint8_t lastAck);
	void validateAndStoreFrame(char * frame, size_t frameSize);
	Queue<std::pair<char*,size_t>> mInputBuffer;
	Queue<std::pair<char*,size_t>> mOutputBuffer;
	DeviceDriver * mDeviceDriver;
	volatile uint8_t lastAck;
	volatile uint8_t lastReceived;
	uint8_t protocolo;
	std::pair<char*,size_t> lastFrameSent;
};

} /* namespace ptc */

/* DATALINKPROTOCOL_H_ */
