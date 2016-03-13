/*
 * DeviceDriver.cpp
 *
 *  Created on: Feb 17, 2016
 *      Author: thiago.b
 */

#include "DeviceDriver.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <mutex>

namespace ptc {

std::mutex deviceMutex;

DeviceDriver::DeviceDriver(const std::string& devicePath) {
	tcgetattr(STDOUT_FILENO, &this->old_stdio);

	memset(&this->stdio, 0, sizeof(this->stdio));
	this->stdio.c_iflag = 0;
	this->stdio.c_oflag = 0;
	this->stdio.c_cflag = 0;
	this->stdio.c_lflag = 0;
	this->stdio.c_cc[VMIN] = 1;
	this->stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &this->stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &this->stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);     // make the reads non-blocking

	memset(&this->tio, 0, sizeof(this->tio));
	this->tio.c_iflag = 0;
	this->tio.c_oflag = 0;
	this->tio.c_cflag = CS8 | CREAD | CLOCAL; // 8n1, see termios.h for more information
	this->tio.c_lflag = 0;
	this->tio.c_cc[VMIN] = 1;
	this->tio.c_cc[VTIME] = 5;

	this->tty_fd = open(devicePath.c_str(), O_RDWR | O_NONBLOCK);
	cfsetospeed(&this->tio, B9600);            // 115200 baud
	cfsetispeed(&this->tio, B9600);            // 115200 baud

	tcsetattr(this->tty_fd, TCSANOW, &this->tio);
}
void DeviceDriver::receiveByte(char* buffer, size_t tamanho){
	std::lock_guard<std::mutex> lock(deviceMutex);
	if (read(this->tty_fd, buffer, tamanho) > 0)
		write(STDOUT_FILENO, buffer, tamanho); // if new data is available on the serial port, print it out
	else throw "No data available";
}
void DeviceDriver::sendByte(const char* mensagem, size_t tamanho){
	std::lock_guard<std::mutex> lock(deviceMutex);
			write(this->tty_fd, mensagem, tamanho); // if new data is available on the console, send it to the serial port
}
DeviceDriver::~DeviceDriver() {
	close(this->tty_fd);
	tcsetattr(STDOUT_FILENO, TCSANOW, &this->old_stdio);
}

} /* namespace ptc */
