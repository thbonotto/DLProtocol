/*
 * DataLinkProtocol.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: thiago.b
 */
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "DataLinkProtocol.h"
extern "C" {
#include "crc.h"
}
namespace ptc {
const char PDU=0x00;
const char ACK=0x01;
const char NACK=0x02;
DataLinkProtocol::DataLinkProtocol(DeviceDriver* deviceDriver,
		DatagramInterface* interface) :
		mDeviceDriver ( deviceDriver ), mDatagramInterface ( interface ), lastAck(
				1), lastReceived(0) {
	protocolo = 0;
	// TODO Auto-generated constructor stub
	if (deviceDriver == nullptr)
		throw "DeviceDriver is Nullpointer";
	//if (notifyCB == nullptr)
	//	throw "Notifify Call Back is Nullpointer";
}

DataLinkProtocol::~DataLinkProtocol() {
	// TODO Auto-generated destructor stub
}
void DataLinkProtocol::resendLastFrame() {
	if (lastFrameSent.first != nullptr)
		mDeviceDriver->sendByte(lastFrameSent.first, lastFrameSent.second);
}
void DataLinkProtocol::validateAndStoreFrame(char * frame, size_t frameSize) {
	uint8_t sequence;
	uint8_t tipo;
	uint8_t protocolo;
	char* frameWithCRC = frame;
	char frameCRC[2];
	frameCRC[0] = frameWithCRC[frameSize - 2];
	frameCRC[1] = frameWithCRC[frameSize - 1];
	unsigned short calculatedCRC = crcSlow((unsigned char*) frameWithCRC,
			frameSize - 2);
	char * calcCRC;
	calcCRC = (char *) &calculatedCRC;
	printf("\rRecebido Frame do tipo: \"\\0x%x\" \r\n",
			(unsigned char) frameWithCRC[1]);
	printf("\rCRC calculado: \\0x%x\\0x%x \r\n", (unsigned char) calcCRC[0],
			(unsigned char) calcCRC[1]);
	printf("\rCRC recebido: \\0x%x\\0x%x \r\n", (unsigned char) frameCRC[0],
			(unsigned char) frameCRC[1]);
	/**
	 * CRC is fine.
	 * Checking now if it's a ACK or NACK frame
	 */
	sequence = frameWithCRC[0];
	tipo = frameWithCRC[1];
	protocolo = frameWithCRC[2];
	char* payload = &frameWithCRC[3];
	/**
	 * Less 1 from seq, less 1 from tipo, less 1 from protocol and less 2 from crc.
	 */
	size_t payloadSize = frameSize - 5;
	if (calcCRC[0] != frameCRC[0] || calcCRC[1] != frameCRC[1]) {
		std::cout << "\rWrong CRC, discarting Frame" << std::endl;
		std::string nack("Nack");
		std::pair<char*, size_t> nackMsg = prepareMessage(sequence, NACK,
				protocolo, (char*) nack.c_str(), nack.size());
		mDeviceDriver->sendByte(nackMsg.first, nackMsg.second);
	} else {
		if (tipo == ACK) {
			/**
			 * Verifying if it's receiving last send frame ack.
			 */
			if (sequence == lastAck) {
				lastReceived = sequence;
				printf("\rAtualizando ultimo ack recebido: \\0x%x \r\n",
						sequence);
			}

			else {
				// Duplicate ACK - retransmission
				printf(
						"\rSeq number diferente do ultimo enviado, retransmitindo ultimo frame: \\0x%x \r\n",
						sequence);
				resendLastFrame();
			}
			return;
		} else if (tipo == NACK) {
			/**
			 * Verifying if it's receiving last send frame nack.
			 * Else ignore.
			 */
			if (sequence == lastAck) {
				printf(
						"\r recebido NACL com Seq number igual do ultimo enviado, retransmitindo ultimo frame: \\0x%x \r\n",
						sequence);
				resendLastFrame();
				return;
			}
		} else if (tipo == PDU) {
			/**
			 * Ok, it's a payload message that must be delivered to up layer.
			 * And send ack.
			 */
			std::string ack("ack");
			std::cout << "\rPreparando para enviar ack n: "
					<< std::to_string(sequence) << std::endl;
			std::pair<char*, size_t> ackMsg = prepareMessage(sequence, ACK,
					protocolo, (char*) ack.c_str(), ack.size());
			mDeviceDriver->sendByte(ackMsg.first, ackMsg.second);
			std::cout << "\rEnviado ack n: " << std::to_string(sequence)
					<< "tipo : " << std::to_string(ackMsg.first[2])
					<< std::endl;
			printf(
					"\rEntregando frame n: \\0x%x Do tipo: \\0x%x   Para o protocolo: \\0x%x \r\n",
					sequence, tipo, protocolo);
			std::cout << "Payload: \"" << std::string(payload, payloadSize)
					<< "\"" << std::endl;
//		std::pair<char*, size_t> frameOK { frame, frameSize };
			std::pair<char*, size_t> frameOK { payload, payloadSize };
			//mInputBuffer.push(frameOK);
			try {
				mDatagramInterface->sendDatagram(payload, payloadSize);
				std::cout << "\rFrame entregue "
										<< std::to_string(sequence) << std::endl;
			} catch (...) {
				std::cout << "\rErro ao entregar frame "
						<< std::to_string(sequence) << std::endl;
			}
			lastReceived = sequence;
		}
	}

}
void DataLinkProtocol::receiveThread() {
	enum Event {
		OCIOSO, INICIADO, RECEPCAO, ESCAPE
	};
	Event estado = OCIOSO;
	char buffer;
	char message[1500];
	memset(message, 0, 1500);
	char* finalMessage;
	int i = 0;
	while (true) {
		try {
			mDeviceDriver->receiveByte(&buffer, 1);
			switch (estado) {
			case OCIOSO:
				if (buffer == 0x7E)
					estado = RECEPCAO;
				break;
			case INICIADO:
				if (buffer == 0x7D)
					estado = ESCAPE;
				else {
					message[i] = buffer;
					i++;
					estado = RECEPCAO;
				}
				break;
			case RECEPCAO:
				if (buffer == 0x7D)
					estado = ESCAPE;
				else if (buffer == 0x7E) {
					finalMessage = (char*) malloc(i);
					memcpy(finalMessage, message, i);
					validateAndStoreFrame(finalMessage, i);
					estado = OCIOSO;
					i = 0;
					buffer = 0;
				} else {
					message[i] = buffer;
					i++;
					estado = RECEPCAO;
				}
				break;
			case ESCAPE:
				message[i] = (buffer ^ 0x20);
				i++;
				estado = RECEPCAO;
				break;
			}
		} catch (...) {
			//Timeout occours sleep to avoid rc
			sleep(1);
			//	buffer = 0;
			//i = 0;
		}
	}
}

/**
 * Check if there is any sentinel byte.
 * Make needed operations, calculates CRC.
 * Insert in transmission buffer.
 */
std::pair<char*, size_t> DataLinkProtocol::prepareMessage(uint8_t sequence,
		uint8_t tipo, uint8_t protocol, char* message, size_t messageSize) {
	char buffer[3];
	buffer[0] = sequence;
	buffer[1] = tipo;
	buffer[2] = protocol;
	/**
	 * Add 3 bytes for sequence, tipo e protocol.
	 * Plus payload size, less /0 of string
	 */
	size_t buff2Size = 3 + messageSize;
	char * buffer2 = (char *) malloc(buff2Size);
	memcpy(buffer2, buffer, 3);
	memcpy(&buffer2[3], message, messageSize);
	size_t crcSize = buff2Size + 2;
	char * crc = (char *) malloc(crcSize);
	memcpy(crc, buffer2, buff2Size);
	unsigned short rescrc = crcSlow((unsigned char*) buffer2, buff2Size);
	memcpy(&crc[buff2Size], &rescrc, 2);
	size_t tamanhoMensagem = crcSize;

	char * buffer3 = (char*) malloc(256);
	buffer3[0] = 0x7E;
	for (size_t i = 0, j = 1; i < crcSize; i++, j++) {
		if (crc[i] == 0x7D) {
			buffer3[j] = 0x7D;
			buffer3[++j] = (0x7D ^ 0x20);
			tamanhoMensagem++;
		} else if (crc[i] == 0x7E) {
			buffer3[j] = 0x7D;
			buffer3[++j] = (0x7E ^ 0x20);
			tamanhoMensagem++;
		} else
			buffer3[j] = crc[i];
	}
	tamanhoMensagem++;
	buffer3[tamanhoMensagem++] = 0x7E;
	std::pair<char*, size_t> retorno { buffer3, tamanhoMensagem };
	return retorno;
}
void DataLinkProtocol::sendThread() {
	std::pair<char*, size_t> message, frame;
	message = mOutputBuffer.pop();
	lastAck = (lastAck == 0) ? 1 : 0;
	frame = prepareMessage(lastAck, PDU, protocolo, message.first,
			message.second);
	mDeviceDriver->sendByte(frame.first, frame.second);
	lastFrameSent = frame;
	lastAck = frame.first[1];
	std::cout << "\rAtualizando valor do ultimo seq enviado: "
			<< std::to_string(lastAck) << std::endl;
	sleep(1);
	while (lastAck != lastReceived) {
		sleep(10);
		mDeviceDriver->sendByte(frame.first, frame.second);
		std::cout << "\rSeq last send: " << std::to_string(lastAck)
				<< std::endl;
		std::cout << "\rSeq last ack: " << std::to_string(lastReceived)
				<< std::endl;

	}
}
void DataLinkProtocol::sendMessage(char* message, size_t messageSize) {
	std::pair<char*, size_t> msg { message, messageSize };
	mOutputBuffer.push(msg);
}

} /* namespace ptc */
