all: UDPClient UDPServer ListenerServer
UDPClient: UDPClient.c
	gcc UDPClient.c -o Client
UDPServer: UDPServer.c
	gcc UDPServer.c HandleUDPClient.c -o Server
ListenerServer: ListenerServer.c
	gcc ListenerServer.c HandleListener.c -o ListenerServer