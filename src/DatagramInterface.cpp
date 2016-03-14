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
#include <arpa/inet.h>
#include <mutex>

namespace ptc {
//tun_alloc()
std::mutex interfaceMutex;
DatagramInterface::DatagramInterface(char *dev, char * ip, char * dst) {

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
	dev = (char*) malloc(sizeof(ifr.ifr_name));
	strcpy(dev, ifr.ifr_name);
	this->fd = fd;

	// esta outra função configura a interface tun: define os endereços IP do enlace ponto-a-ponto,
	// a máscara de rede /30 e ativa a interface. Ela faz o mesmo que o comando:
	// ifconfig nome_tun IP_da_interface dstaddr IP_da_outra_ponta_do_enlace
	//
	// esta função deve ser usada assim, supondo que a interface tun se chame tun0, e os endereços IP
	// do enlace sejam 10.0.0.1 e 10.0.0.2:
	//
	// if (set_ip("tun0", "10.0.0.1", "10.0.0.2") < 0) {
	//   perror("so configurar a interface tun");
	//   return 0;
	// }
	//
	// Maiores detalhes sobre as chamadas de sistemas utilizadas: ver "man netdevice", ou
	// http://man7.org/linux/man-pages/man7/netdevice.7.html

	// struct ifreq ifr;
	struct sockaddr_in *addr;
	int ok;
	int sd;

	// cria um socket para configurar a interface
	// esse socket não tem nada de especial ...
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	//  if (sd < 0) return sd;

	// zera todos os bytes da struct ifreq
	// essa struct contém os atributos a serem configurados na interface
	bzero(&ifr, sizeof(ifr));

	// usa o ponteiro addr para referenciar o campo de endereço da struct ifreq
	// isso facilita o preenchimento dos atributos desse campo
	addr = (struct sockaddr_in*) &(ifr.ifr_ifru.ifru_addr);

	// preenche o campo endereço com o enderço IP da interface
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_family = AF_INET;
	addr->sin_port = 0;

	// escreve o nome da interface na struct ifreq. Isso é necessário
	// para o kernel saber que interface é alvo da operação
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	// executa uma operação de configuração de endereço IP de interface
	ok = ioctl(sd, SIOCSIFADDR, &ifr);
	//if (ok < 0) return ok;

	// preenche o campo endereço com o endereço IP da outra ponta do enlace
	addr->sin_addr.s_addr = inet_addr(dst);

	// executa uma operação de configuração de endereço IP de destino da interface
	ok = ioctl(sd, SIOCSIFDSTADDR, &ifr);
	//if (ok < 0) return ok;

	// preenche o campo endereço com a máscara de rede da interface
	addr->sin_addr.s_addr = inet_addr("255.255.255.252");

	// executa uma operação de configuração de máscara de rede da interface
	ok = ioctl(sd, SIOCSIFNETMASK, &ifr);
	//  if (ok < 0) return ok;

	// lê as flags da interface
	ok = ioctl(sd, SIOCGIFFLAGS, &ifr);
	// if (ok < 0) return ok;

	// acrescenta flags UP (ativa) e RUNNING (pronta para uso)
	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

	// executa uma operação de configuração flags da interface
	ioctl(sd, SIOCSIFFLAGS, &ifr);

}

DatagramInterface::~DatagramInterface() {
	// TODO Auto-generated destructor stub
}
size_t DatagramInterface::receiveDatagram(char* buffer, size_t tamanho) {
	//std::lock_guard<std::mutex> lock(interfaceMutex);
	size_t nBytes=0;
	struct timeval timeout; // para especificar o timeout
	timeout.tv_sec = 2; //timeout de 2 segundos
	timeout.tv_usec = 0;
	fd_set espera;
	//FD_ZERO(&espera);
	//FD_SET(this->fd, &espera);
	//if (select(this->fd, &espera, NULL, NULL, &timeout) == 0) { // timeout !!
	//	throw "\rTimeout";
	//} else {
		nBytes = read(this->fd, buffer, tamanho);
//		write( STDOUT_FILENO, buffer, tamanho);
	//}
	return nBytes;
}
void DatagramInterface::sendDatagram(const char* datagram, size_t tamanho) {
	std::lock_guard<std::mutex> lock(interfaceMutex);
	size_t wroteBytes = write(this->fd, datagram, tamanho); // if new data is available on the console, send it to the serial port
	if(wroteBytes != tamanho)
		throw "Impossível entregar datagram";
}

} /* namespace ptc */
