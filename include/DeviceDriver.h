/*
 * DeviceDriver.h
 *
 *  Created on: Feb 17, 2016
 *      Author: thiago.b
 */

#pragma once 
#include <termios.h>
#include <string>

namespace ptc {

class DeviceDriver {
public:
	DeviceDriver(const std::string& devicePath);
	void receiveByte(std::string& buffer);
	void sendByte(const std::string& message);
	virtual ~DeviceDriver();
private:
	int tty_fd;
	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;
};

} /* namespace ptc */

/* DEVICEDRIVER_H_ */
