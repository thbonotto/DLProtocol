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
	Queue<std::pair<char*, size_t>>* buffer = protocolo->getOutputBuffer();
	std::pair<char*, size_t> frame;
	while (true) {
		frame = buffer->pop();
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
	size_t nBytes;
	while (true) {
		try {
			std::cout << "\rProdutora lendo" << std::endl;
			nBytes = interface->receiveDatagram(buffer, 255);
			char* message = (char*) malloc(nBytes);
			std::cout << "\rMessage: " << message << "tamanho :"
					<< std::to_string(nBytes) << std::endl;
			interface->receiveDatagram(message, nBytes);
			protocolo->sendMessage(message, nBytes);
			nBytes = 0;

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
		if (argc < 2) {
			std::cout << "Uso: ./DLProtocol interfaceIp destinationIp"
					<< std::endl;
			exit(-1);
		}
		DeviceDriver uart { argv[1] };
		DatagramInterface interface { argv[2], argv[3], argv[4] };
		DataLinkProtocol protocolo { &uart };
		std::thread sender(sendThread, &protocolo);
		std::thread receptor(receiveThread, &protocolo);
		if (argc == 2) {
			for (int i = 0;; i++) {
				std::string mes { "Mensagem~" + std::to_string(i) };
				//std::string mes2{"Mensagem2"};
				protocolo.sendMessage((char*) mes.c_str(), mes.size());
				//protocolo.sendMessage((char*)mes2.c_str(),mes2.size());
				sleep(5);
			}
		}
		std::thread toBuffer(bufferThread, &protocolo, &interface);
		std::thread fromBuffer(unbufferThread, &protocolo, &interface);
		std::string mes { "Mensagem~" };
		//std::string mes2{"Mensagem2"};
		if (strcasecmp(argv[2], "dlp0") == 0)
			protocolo.sendMessage((char*) mes.c_str(), mes.size());
		sender.join();
		receptor.join();
		toBuffer.join();
		fromBuffer.join();
	} catch (const char* e) {
		std::cout << e << std::endl;
	}
	return 0;
}
