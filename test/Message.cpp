/**
 * Message.cpp
 *
 *  Created on: Feb 17, 2016
 *      Author: thiago.b
 *
 * Adaptation from https://en.wikibooks.org/wiki/Serial_Programming/Serial_Linux#termios
 *
 */

#include "DeviceDriver.h"
#include "Notifier.h"
#include "DataLinkProtocol.h"

using namespace ptc;

int main() {
	DeviceDriver uart{"/dev/ttyUSB1","",9600};
	Notifier* reader;
	DataLinkProtocol protocol{&uart,reader};
	std::string testMsg{"Primeira Mensagem de Teste"};
	protocol.prepareAndSendMessage(testMsg);
	return 0;
}
