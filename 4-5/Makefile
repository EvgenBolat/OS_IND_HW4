all: UDPClient UDPServer
UDPClient: UDPClient.c
	gcc UDPClient.c -o Client
UDPServer: UDPServer.c
	gcc UDPServer.c HandleUDPClient.c -o Server