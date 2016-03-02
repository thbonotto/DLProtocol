/*
 * DatagramInterface.h
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#pragma once 

namespace ptc {

class DatagramInterface {
public:
	DatagramInterface(char *dev);
	virtual ~DatagramInterface();
	const int& getFileDescriptor() const{
		return this->fd;
	};
private:
	int fd;
};

} /* namespace ptc */

/* DATAGRAMINTERFACE_H_ */
