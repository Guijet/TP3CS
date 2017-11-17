#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "../DLL/DLL.h"

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma once

// COMMUNICATION CLASS
class Communication
{
private:
	// Attributes
	SOCKET * ClientSocket;
	int iResult;

protected:
	// Mutateurs
	void setiResult(int iResult) { this->iResult = iResult; }

	// Attributes
	const std::string quitWord = "QUIT";
	char msg[512];
	string mot;
public:
	// Constructeur et méthodes
	Communication();
	void setTextColor(int textColor, int backColor);
	virtual void terminate() = 0;

	// Mutateurs publics.
	void setSocket(SOCKET * socket) { this->ClientSocket = socket; }
	void setMot(std::string mot2) { this->mot = mot2; }

	// File cheker methods
	bool isFile(char *msg);
	bool file_exists(const char *fileName);
	std::string getFileNameReceived(char *msg);
	std::string getFileContent(char *msg);
	void createFile(std::string contenu, std::string name);
	bool isFileSending(std::string message);
	std::string getFileNameSending(std::string message);

	// Accesseurs
	SOCKET * getSocket() const { return this->ClientSocket; }
	int getiResult() const { return this->iResult; }
	string getMot() const { return this->mot; }
};

// RECV CLASS
class Recv : public Communication {
private:
	// Attributes
	char recvbuf[512];
public:
	// Constructor and methods
	Recv();
	int receiveMessage();
	void terminate();
};

// SEND CLASS
class Send : public Communication {
public:
	// Constructor and methods
	Send(std::string & mot2, int iResult);
	void sendFile();
	void sendMessage();
	int sendStuff();
	void terminate();
};

// THREADING CLASS
class Threading {
private:
	Send * toolSend;
	Recv * toolReceive;
public:
	Threading(std::string mot, int iResult);

	// Getters
	Send * getToolSend() const { return this->toolSend; }
	Recv * getToolReceive() const { return this->toolReceive; }
};

// INFO CLASS
static class Info {
public:
	static Threading * threads;
};

// HANDLER CLASS
static class Handler {
public:
	static HANDLE threadRecv, threadSend;
	static DWORD WINAPI send(void * pParam);
	static DWORD WINAPI recv(void * pParam);
};