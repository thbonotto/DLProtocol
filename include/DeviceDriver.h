/*
 * DeviceDriver.h
 *
 *  Created on: Feb 17, 2016
 *      Author: thiago.b
 */

#pragma once 

#include <string>

namespace ptc {

class DeviceDriver {
public:
	DeviceDriver(std::string device, std::string as, int baud);
	void sendMessage(std::string& message);
	virtual ~DeviceDriver();
};

} /* namespace ptc */

/* DEVICEDRIVER_H_ */
