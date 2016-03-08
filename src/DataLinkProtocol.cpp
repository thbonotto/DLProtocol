/*
 * DataLinkProtocol.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */
#include <cstdint>
#include <cstring>

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
void DataLinkProtocol::prepareAndSendMessage(uint8_t sequence, uint8_t tipo, uint8_t protocol, std::string& message) {
	char buffer[3];
	buffer[0]=sequence;
	buffer[1]=tipo;
	buffer[2]=protocol;
	/**
	 * Add 3 bytes for squence, tipo e protocol.
	 * Plus payload size, less /0 of string
	 */
	size_t buff2Size = 3 + message.size() -1;
	char * buffer2 = (char *) malloc(buff2Size);
	strcpy(buffer2,buffer);
	strcpy(&buffer2[4],message.c_str());
	crcInit();
	size_t crcSize= buff2Size+2;
	char * crc = (char *) malloc(crcSize);
	memcpy(crc,buffer2,buff2Size);
	unsigned short rescrc;
	rescrc=crcSlow((unsigned char*)buffer2,buff2Size);
	memcpy(&crc[buff2Size-1],&rescrc,2);

	std::string afterCRC;
	size_t tamanhoMensagem = crcSize;
	std::string mensagemPronta(255,' ');
	char * buffer3 = (char*) mensagemPronta.c_str();
	buffer3[0] = 0x7E;
	for (size_t i = 0, j = 1; i < crcSize; i++, j++) {
//		if (buffer[i] == 0x7D) {
//			buffer2[j] = 0x7D;
//			buffer2[++j] = (0x7D ^ 0x20);
//			tamanhoMensagem++;
//		} else if (buffer[i] == 0x7E) {
//			buffer2[j] = 0x7D;
//			buffer2[++j] = (0x7E ^ 0x20);
//			tamanhoMensagem++;
//		} else
			buffer3[j] = crc[i];
			tamanhoMensagem++;
	}
//	std::string afterSentinel{buffer2,tamanhoMensagem};
	//std::string afterSentinel{buffer2,tamanhoMensagem};
//	this->makeCRC(afterSentinel, afterSentinel);
	buffer3[tamanhoMensagem] = 0x7E;

	mDeviceDriver->sendByte(std::string{crc,crcSize+5});
	//this->mOutputBuffer.push(afterSentinel);

}

} /* namespace ptc */
