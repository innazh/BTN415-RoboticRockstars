#include "PktDef.h"
#include "MySocket.h"

bool ExeComplete;

int main(int argc, char* argv) {
	ExeComplete = false;

	MySocket first(SocketType::CLIENT, ip, port, ConnectionType::TCP, 100);
	MySocket second(CLIENT, ip, port, TCP, 100);

}