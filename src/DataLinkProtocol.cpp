/*
 * DataLinkProtocol.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#include "DataLinkProtocol.h"
extern "C"
{
#include "crc.h"
}
namespace ptc {

DataLinkProtocol::DataLinkProtocol(DeviceDriver* deviceDriver,
		NotifyCallback* notifyCB) :
		mDeviceDriver { deviceDriver }, mNotify_cb { notifyCB } {
	// TODO Auto-generated constructor stub
	if (deviceDriver == nullptr)
		throw "DeviceDriver is Nullpointer";
	//if (notifyCB == nullptr)
	//	throw "Notifify Call Back is Nullpointer";
}

DataLinkProtocol::~DataLinkProtocol() {
	// TODO Auto-generated destructor stub
}

void DataLinkProtocol::makeCRC(std::string& message,
		std::string& messageAndCRC) {
	crc tmp;
	messageAndCRC=message;
	crcInit();
	unsigned char* const messagem=(unsigned char* const)message.c_str();
	tmp = crcSlow(messagem,message.size());
	char* final =(char*) messageAndCRC.c_str();
	final[messageAndCRC.size()]=tmp;

}

/**
 * Check if there is any sentinel byte.
 * Make needed operations, calculates CRC.
 * Insert in transmission buffer.
 */
void DataLinkProtocol::prepareAndSendMessage(std::string& message) {
	size_t tamanhoMensagem = message.size();
	std::string mensagemPronta;
	char * buffer = (char*) message.c_str();
	char * buffer2 = (char*) mensagemPronta.c_str();
	buffer2[0] = 0x7E;
	for (size_t i = 0, j = 0; i < message.size() - 1; i++, j++) {
		if (buffer[i] == 0x7D) {
			buffer2[j] = 0x7D;
			buffer2[++j] = (0x7D ^ 0x20);
			tamanhoMensagem++;
		} else if (buffer[i] == 0x7E) {
			buffer2[j] = 0x7D;
			buffer2[++j] = (0x7E ^ 0x20);
			tamanhoMensagem++;
		} else
			buffer2[j] = buffer[i];
	}
	std::string afterSentinel;
	this->makeCRC(message, afterSentinel);
	buffer2[tamanhoMensagem] = 0x7D;

	mDeviceDriver->sendByte(std::string{buffer2,255});
	//this->mOutputBuffer.push(afterSentinel);

}

} /* namespace ptc */
