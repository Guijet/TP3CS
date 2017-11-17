#include "Communication.h"

int main() {
	WSADATA wsaData; SOCKADDR_IN sin; SOCKET ClientSocket, ListenSocket = INVALID_SOCKET;
	char msg[512], recvbuf[512]; std::string quitWord = "QUIT"; std::string mot;

	std::cout << "[Veuillez entrer '" + quitWord + "' pour quitter le chat]" << std::endl;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_family = AF_INET; sin.sin_port = htons(8080);
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult); return 1;
	}
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError()); WSACleanup(); return 1;
	}
	iResult = bind(ListenSocket, (SOCKADDR *)&sin, sizeof(sin));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError()); closesocket(ListenSocket); WSACleanup(); return 1;
	}
	ClientSocket = INVALID_SOCKET; std::cout << "Attendre une connexion d'un client ..... \n";
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen failed: %d\n", WSAGetLastError()); closesocket(ListenSocket); WSACleanup(); return 1;
	}
	ClientSocket = accept(ListenSocket, NULL, NULL); std::cout << "Un client vient de se connecter ..... \n";
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept failed: %d\n", WSAGetLastError()); closesocket(ListenSocket); WSACleanup(); return 1;
	}

    Info::threads = new Threading(mot, iResult);
	
	Handler::threadRecv = CreateThread(NULL, 0, Handler::recv, &ClientSocket, 0, 0);
	Handler::threadSend = CreateThread(NULL, 0, Handler::send, &ClientSocket, 0, 0);
	WaitForSingleObject(Handler::threadRecv, INFINITE); CloseHandle(Handler::threadRecv);
	WaitForSingleObject(Handler::threadSend, INFINITE); CloseHandle(Handler::threadSend);

	//closesocket(ListenSocket); WSACleanup();
	return 0;
}