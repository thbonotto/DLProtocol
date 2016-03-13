/*
 * DatagramInterface.h
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#pragma once 
#include <stddef.h>
namespace ptc {

class DatagramInterface {
public:
	DatagramInterface(char *dev, char * ip, char * dst);
	size_t receiveDatagram(char* buffer, size_t tamanho);
	void sendDatagram(const char* datagram, size_t tamanho);
	virtual ~DatagramInterface();
	const int& getFileDescriptor() const{
		return this->fd;
	};
private:
	int fd;
};

} /* namespace ptc */

/* DATAGRAMINTERFACE_H_ */
