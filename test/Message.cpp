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
#include "DeviceDriver.h"
#include "Notifier.h"
#include "DataLinkProtocol.h"

using namespace ptc;
void receiveThread(DataLinkProtocol* protocolo){
	protocolo->receiveThread();
}
void sendThread(DataLinkProtocol* protocolo){
	while(true){
		protocolo->sendThread();
	}
}

int main(int argc, char** argv) {
	DeviceDriver uart {argv[1]};
	DataLinkProtocol protocolo{&uart,nullptr};
	std::string mes{"Mensagem~"};
	std::string mes2{"Mensagem2"};
//	protocolo.mOutputBuffer.push(Frame);

	std::thread sender(sendThread, &protocolo);
	std::thread receptor(receiveThread, &protocolo);
	std::string buffer;
	std::pair<char*,size_t> frame;
	protocolo.sendMessage((char*)mes2.c_str(),mes.size());
	protocolo.sendMessage((char*)mes2.c_str(),mes2.size());
	//while(true){
		//sleep(10);
	//	protocolo.sendMessage((char*)mes2.c_str(),mes.size());
	//	sleep(5);
	//	protocolo.sendMessage((char*)mes2.c_str(),mes2.size());
//	}
//	sender.join();
//	receptor.join();
	sleep(10);
	return 0;
}
