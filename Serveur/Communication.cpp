#include "Communication.h"

/*
 * Static Declarations
 */

#pragma region Static Declarations

HANDLE Handler::threadRecv;
HANDLE Handler::threadSend;
Threading * Info::threads;

#pragma endregion

/*
 * Communication Class methods.
 */

#pragma region Communication

Communication::Communication() {}

void Communication::terminate() {}

void Communication::setTextColor(int textColor, int backColor)
{
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(H, backColor * 16 + textColor);
}

bool Communication::isFile(char *msg)
{
	std::string message(msg);
	if (message.find("EnvoiFichier") != std::string::npos)
		return true;
	else
		return false;
}

bool Communication::file_exists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

std::string Communication::getFileNameReceived(char *msg)
{
	std::string message(msg);
	int index = 0;
	int position1, position2;
	for each (char character in message)
	{
		if (character == '(')
			position1 = index + 1;
		if (character == ')')
			position2 = index - 1;
		++index;
	}
	return message.substr(position1, position2 - position1 + 1);
}

std::string Communication::getFileContent(char *msg)
{
	std::string message(msg);
	int index = 0;
	int position1;
	for each (char character in message)
	{
		if (character == ')') position1 = index + 2;
		++index;
	}
	return message.substr(position1, message.length() - position1 + 1);
}

void Communication::createFile(std::string contenu, std::string name)
{
	std::ofstream file;
	file.open(name);
	file << contenu;
	file.close();
}

bool Communication::isFileSending(std::string message)
{
	return message.find("EnvoiFichier") != std::string::npos;
}

std::string Communication::getFileNameSending(std::string message)
{
	int index = 0;
	int position1, position2;
	for each (char character in message)
	{
		if (character == '(')
			position1 = index + 1;
		if (character == ')')
			position2 = index - 1;
		++index;
	}
	return message.substr(position1, position2 - position1 + 1);
}

#pragma endregion

/*
 * Receive messages Class methods.
 */

#pragma region Recv

Recv::Recv() : Communication() {}

int Recv::receiveMessage()
{
	do {
		this->setiResult(recv(*(this->getSocket()), recvbuf, 512, 0));
		if (this->getiResult() > 0)
		{
			memset(&msg, 0, sizeof(msg)); strncpy(msg, recvbuf, this->getiResult());
			// DECODER LE MESSAGE
			string msgString(msg);
			char *motEncoded = new char[msgString.length() - 1];
			strcpy(motEncoded, msgString.c_str());
			encodeMessage(7, motEncoded, true);
			this->setTextColor(2, 0); // Couleur verte

			if (string(motEncoded) == quitWord)
			{
				this->setTextColor(4, 0);
				cout << "Connection closed" << endl;
				Info::threads->getToolSend()->terminate();
				this->terminate();
				return 1;
			}

			cout << string(motEncoded) << endl;
			this->setTextColor(9, 0); // Couleur bleue

			if (this->isFile(motEncoded))
				this->createFile(this->getFileContent(motEncoded), this->getFileNameReceived(motEncoded));
		}
		else if (this->getiResult() == 0)
		{
			this->setTextColor(4, 0);
			cout << "Connection closed" << endl;
			Info::threads->getToolSend()->terminate();
			this->terminate();
			return 1;
		}
		else
		{
			this->setTextColor(4, 0);
			printf("recv failed: %d\n", WSAGetLastError());
			Info::threads->getToolSend()->terminate();
			this->terminate();
			closesocket(*(this->getSocket())); WSACleanup(); return 1;
		}
	} while (this->getMot() != this->quitWord && this->msg != this->quitWord);
	return 0;
}

void Recv::terminate()
{
	CloseHandle(Handler::threadRecv);
}

#pragma endregion

/*
 * Send messages Class methods.
 */

#pragma region Send

Send::Send(std::string & mot2, int iResult)
{
	setMot(mot2);
	setiResult(iResult);
}

void Send::sendFile()
{
	std::string filename;
	bool error = false;
	do
	{
		if (error)
		{
			std::getline(std::cin, this->mot);
			this->setTextColor(9, 0); // Couleur bleue
			if (!this->isFileSending(this->getMot()))
			{
				this->sendMessage();
				return;
			}
		}

		filename = getFileNameSending(this->getMot());
		std::string result;

		if (this->file_exists(filename.c_str()))
		{
			std::ifstream myReadFile;
			myReadFile.open(filename);
			char output[512];
			if (myReadFile.is_open()) {
				while (!myReadFile.eof()) {
					myReadFile >> output;
					result += output;
					result += " ";
				}
			}
			myReadFile.close();

			//ENCODER LE MESSAGE
			std::string finalresult = "EnvoiFichier(" + filename + ")" + " " + result;
			char *motEncoded = new char[finalresult.length() + 1];
			strcpy(motEncoded, finalresult.c_str());
			encodeMessage(7, motEncoded, false);
			this->setiResult(send(*(this->getSocket()), motEncoded, strlen(motEncoded), 0));
		}
		else
		{
			std::cout << "Le fichier entrer est introuvable." << std::endl;
			error = true;
		}
	} while (!file_exists(filename.c_str()));
}

void Send::sendMessage()
{
	//ENCODER LE MESSAGE
	char * motEncoded = new char[this->getMot().length() + 1];
	strcpy(motEncoded, this->getMot().c_str());
	encodeMessage(7, motEncoded, false);
	this->setiResult(send(*(this->getSocket()), motEncoded, strlen(motEncoded), 0));
}

int Send::sendStuff()
{
	do {
		this->setTextColor(9, 0); // Couleur bleue
		std::getline(std::cin, this->mot);

		if (this->getMot() == quitWord)
		{
			this->setTextColor(4, 0);
			std::cout << "Connection closed" << std::endl;
			Info::threads->getToolReceive()->terminate();
			this->terminate();
			return 1;
		}

		// Check if entered word is a file or not.
		this->isFileSending(this->getMot()) ? this->sendFile() : this->sendMessage();
		// CHANGE RECEIVE THREAD ENTERED WORD (mot).
		Info::threads->getToolReceive()->setMot(this->getMot());

	} while (this->getMot() != this->quitWord && this->msg != this->quitWord);
	return 0;
}

void Send::terminate()
{
	CloseHandle(Handler::threadSend);
}

#pragma endregion

/*
 * Threading constructor
 */

#pragma region Threading

Threading::Threading(std::string mot, int iResult)
{
	this->toolSend    = new Send(mot, iResult);
	this->toolReceive = new Recv();
}

#pragma endregion

/*
 * Handler static methods
 */

#pragma region Handler

DWORD WINAPI Handler::send(void * pParam)
{
	Info::threads->getToolSend()->setSocket(reinterpret_cast<SOCKET *>(pParam));
	return Info::threads->getToolSend()->sendStuff();
}

DWORD WINAPI Handler::recv(void * pParam)
{
	Info::threads->getToolReceive()->setSocket(reinterpret_cast<SOCKET *>(pParam));
	return Info::threads->getToolReceive()->receiveMessage();
}

#pragma endregion