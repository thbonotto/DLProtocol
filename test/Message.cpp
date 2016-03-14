/**
 * Message.cpp
 *
 *  Created on: Feb 17, 2016
 *      Author: thiago.b
 *
 * Adaptation from https://en.wikibooks.org/wiki/Serial_Programming/Serial_Linux#termios
 *
 */
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "DeviceDriver.h"
#include "DataLinkProtocol.h"
#include "DatagramInterface.h"

using namespace ptc;
/**
 * Read from protocol output buffer and write to interface
 */
void unbufferThread(DataLinkProtocol* protocolo, DatagramInterface* interface) {
	std::cout << "\rConsumidora disparada." << std::endl;
	Queue<std::pair<char*, size_t>>* buffer = protocolo->getOutputBuffer(); // it's inoputbuffer actually
	std::pair<char*, size_t> frame;
	while (true) {
		frame = buffer->pop();
		std::cout << "\rEnviado frame para interface. \r\nPayload: "
				<< std::string { frame.first, frame.second } << std::endl;
		interface->sendDatagram(frame.first, frame.second);
	}
}
/**
 * receive datagram fom TUN interface and sendMessage;
 */
void bufferThread(DataLinkProtocol* protocolo, DatagramInterface* interface) {
	std::cout << "\rProdutora disparada." << std::endl;
	char buffer[256];
	std::pair<char*, size_t> frame;
	size_t nBytes = 255;
	while (true) {
		try {
			std::cout << "\rProdutora lendo" << std::endl;
			//nBytes = interface->receiveDatagram(buffer, 255);
			char* message = (char*) malloc(nBytes);
			//std::cout << "\rMessage: " << message << "tamanho :"
			//	<< std::to_string(nBytes) << std::endl;
			interface->receiveDatagram(message, 84);
			protocolo->sendMessage(message, nBytes);
			//nBytes = 0;

		} catch (const char* e) {
			std::cout << e << std::endl;
			//	sleep(5);
		}
	}
}
void receiveThread(DataLinkProtocol* protocolo) {
	protocolo->receiveThread();
}
void sendThread(DataLinkProtocol* protocolo) {
	while (true) {
		protocolo->sendThread();
	}
}

int main(int argc, char** argv) {
	try {
		if (argc < 4) {
			std::cout
					<< "Uso: ./DLProtocol transmissorDevice interfaceName interfaceIp destinationIp"
					<< std::endl
					<< "  Ex: ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2"
					<< std::endl
					<< "Para efetuar um teste entre duas interfaces: "
					<< std::endl
					<< " Use: ./DLProtocol interfaceName interfaceIp destinationIp teste"
					<< std::endl
					<< "  Ex: ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2 teste"
					<< std::endl
					<< "Atenção: Seu usuário deve ser CAP_NET_ADMIN caso contrário execute como sudo."
					<< std::endl
					<< "  Ex: sudo ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2 teste"
					<< std::endl;
			exit(-1);
		}
		DeviceDriver uart { argv[1] };
		DatagramInterface interface { argv[2], argv[3], argv[4] };
		DataLinkProtocol protocolo { &uart, &interface };
		std::thread sender(sendThread, &protocolo);
		std::thread receptor(receiveThread, &protocolo);
		if (argc > 5) {
			for (int i = 0;; i++) {
				std::string mes { "Mensagem da interface: "
						+ std::string(argv[2]) + " Número: " + std::to_string(i) };
				protocolo.sendMessage((char*) mes.c_str(), mes.size());
				sleep(i);
			}
		} else {
			std::pair<char*, size_t> frame;
			size_t nBytes = 255;
			char buffer[256];
			while (true) {
				try {
					std::cout << "\rProdutora lendo" << std::endl;
					nBytes = interface.receiveDatagram(buffer, 255);
					char* message = (char*) malloc(nBytes);
					std::cout << "\rMessage: " << message << "tamanho :"
							<< std::to_string(nBytes) << std::endl;
					interface.receiveDatagram(message, nBytes);
					protocolo.sendMessage(message, nBytes);
				} catch (const char* e) {
					std::cout << "\r" << e << std::endl;
				}
			}
		}
		//std::thread toBuffer(bufferThread, &protocolo, &interface);
		//std::thread fromBuffer(unbufferThread, &protocolo, &interface);
		sender.join();
		receptor.join();
		//	toBuffer.join();
		//	fromBuffer.join();
	} catch (const char* e) {
		std::cout << e << std::endl;
	} catch (const int& e) {
		std::cout << "Exceção :" << std::to_string(e) << std::endl
				<< "Atenção: Seu usuário deve ser CAP_NET_ADMIN caso contrário execute como sudo."
				<< std::endl
				<< "  Ex: sudo ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2 teste"
				<< std::endl;
	}
	return 0;
}
