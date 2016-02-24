/*
 * DatagramInterface.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */

#include "DatagramInterface.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>


namespace ptc {
//tun_alloc()
DatagramInterface::DatagramInterface(char *dev) {

	struct ifreq ifr;
	int fd, err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		perror("");
		throw "Cannot open device";
	}

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
	if (*dev)
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	err = ioctl(fd, TUNSETIFF, (void *) &ifr);
	if (err < 0) {
		close(fd);
		perror("");
		throw err;
	}
	strcpy(dev, ifr.ifr_name);
	this->fd = fd;

}

DatagramInterface::~DatagramInterface() {
	// TODO Auto-generated destructor stub
}

} /* namespace ptc */
