all:
	gcc -I./include ./src/crc.c -c -o crc.o
	g++ -std=c++0x -I./include crc.o ./src/DeviceDriver.cpp ./src/DataLinkProtocol.cpp ./src/DatagramInterface.cpp ./test/Message.cpp -lpthread -o DLProtocol
	
	@echo "Uso: ./DLProtocol transmissorDevice interfaceName interfaceIp destinationIp"
	@echo " Ex: ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2"
	@echo "Para efetuar um teste entre duas interfaces: "
	@echo " Use: ./DLProtocol interfaceName interfaceIp destinationIp teste"
	@echo "  Ex: ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2 teste"
	@echo "Atenção: Seu usuário deve ser CAP_NET_ADMIN caso contrário execute como sudo."
	@echo "  Ex: sudo ./DLProtocol /dev/ttyUSB0 dlp0 10.0.0.1 10.0.0.2 teste"

clean: 
	rm -f crc.o
	rm -f DLProtocol
