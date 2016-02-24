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
#include <termios.h>


namespace ptc {

DeviceDriver::DeviceDriver(std::string device, std::string as, int baud) {
	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;
	int tty_fd;

	unsigned char c = 'D';
	tcgetattr(STDOUT_FILENO, &old_stdio);

	memset(&stdio, 0, sizeof(stdio));
	stdio.c_iflag = 0;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag = 0;
	stdio.c_cc[VMIN] = 1;
	stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);     // make the reads non-blocking

	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL; // 8n1, see termios.h for more information
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;

	tty_fd = open(device.c_str(), O_RDWR | O_NONBLOCK);
	cfsetospeed(&tio, B9600);            // 115200 baud
	cfsetispeed(&tio, B9600);            // 115200 baud

	tcsetattr(tty_fd, TCSANOW, &tio);
	while (c != 'q') {
		if (read(tty_fd, &c, 1) > 0)
			write(STDOUT_FILENO, &c, 1); // if new data is available on the serial port, print it out
		if (read(STDIN_FILENO, &c, 1) > 0)
			write(tty_fd, &c, 1); // if new data is available on the console, send it to the serial port
	}
	sleep(5);
	close(tty_fd);
	tcsetattr(STDOUT_FILENO, TCSANOW, &old_stdio);

//	return EXIT_SUCCESS;

}

void DeviceDriver::sendMessage(std::string& mensagem){

}
DeviceDriver::~DeviceDriver() {
	// TODO Auto-generated destructor stub
}

} /* namespace ptc */
