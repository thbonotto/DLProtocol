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

	this->tty_fd = open(devicePath.c_str(), O_RDWR );
	cfsetospeed(&this->tio, B9600);            // 115200 baud
	cfsetispeed(&this->tio, B9600);            // 115200 baud

	tcsetattr(this->tty_fd, TCSANOW, &this->tio);
}
void DeviceDriver::receiveByte(char* buffer, size_t tamanho){
	//  std::lock_guard<std::mutex> lock(deviceMutex);
	  struct timeval timeout; // para especificar o timeout
	  timeout.tv_sec = 2; //timeout de 2 segundos
	  timeout.tv_usec = 0;

	  fd_set espera;
	  //FD_ZERO(&espera);
	  //FD_SET(this->tty_fd, &espera);

	  //if (select(this->tty_fd, &espera, NULL, NULL, &timeout) == 0) { // timeout !!
		//  throw "\rTimeout";
	//  } else {
		  read(this->tty_fd, buffer, tamanho);
		//  write( STDOUT_FILENO, buffer, tamanho);
	//  }
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
