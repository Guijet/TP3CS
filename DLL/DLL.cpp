// Travail1.cpp : définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"
#include "DLL.h"
#include "string.h"

bool Verification::containsSpecialCharacter(string toCheck)
{
	return toCheck.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,") != string::npos;
}

bool Verification::containsPourcentage(string toCheck)
{
	return toCheck.find_first_of("%") != string::npos;
}

extern "C"
{
	DLL_API void Rotation(char* messageReceived, int chiffre)
	{
		string crypted = "", nextChar = "", message(messageReceived);
		for each (auto lettre in message)
		{
			if (isalpha(lettre))
			{
				if (lettre == 'z' && chiffre > 0)
					nextChar = 97 + chiffre - 1;
				else if (lettre == 'Z' && chiffre > 0)
					nextChar = 'A' + chiffre - 1;
				else if (lettre == 'a' && chiffre < 0)
					nextChar = (char)(((int)'z') - chiffre);
				else if (lettre == 'A' && chiffre < 0)
					nextChar = (char)(((int)'Z') - chiffre - 1);
				else
				{
					if (!isalpha((((int)lettre) + chiffre - 1)))
						nextChar = (char)(((int)(chiffre < 0 ? 'z' : 'a')) + chiffre);
					else
						nextChar = (char)(((int)lettre) + chiffre);
				}
			}
			else
				nextChar = lettre;
			crypted.append(nextChar);
		}
		strcpy(messageReceived, crypted.c_str());
	}

} // Rotation

extern "C"
{
	DLL_API void encodeMessage(int position, char* messageReceived, bool isChiffrer)
	{
		string message = messageReceived;
		string cutPart, stayedPart;
		int myPositon;
		string encodedMessage = "";

		if (position <= message.length())
			myPositon = position;
		else
			myPositon = position % message.length();

		if (isChiffrer) {
			cutPart = message.substr(message.length() - myPositon);
			stayedPart = message.substr(0, message.length() - cutPart.length());
			encodedMessage = (cutPart + stayedPart).c_str();
		}
		else {
			cutPart = message.substr(0, myPositon);
			stayedPart = message.substr(myPositon, message.length() - cutPart.length());
			encodedMessage = (stayedPart + cutPart).c_str();
		}
		strcpy(messageReceived, encodedMessage.c_str());
	}

} // Circulaire

extern "C"
{
	DLL_API void AlgoTrois(char* messageReceived)
	{
		string message(messageReceived), toReturn = "", nextChar = "";
		int asciiValue = 0;

		// Pour décoder
		if (Verification::containsPourcentage(message))
		{
			for each (auto lettre in message)
			{
				if (lettre != '%')
					nextChar += lettre;
				else
				{
					asciiValue = stoi(nextChar);
					nextChar = (char)asciiValue + 96;
					toReturn.append(nextChar);
					nextChar = "";
				}
			}
		}
		// Pour encoder (DONE)
		else
		{
			for each (auto lettre in message)
			{
				nextChar = lettre;
				if (isalpha(lettre))
				{
					asciiValue = lettre - 'a' + 1;
					toReturn.append(to_string(asciiValue) + "%");
				}
			}
		}
		strcpy(messageReceived, toReturn.c_str());
	}
} // Algo 3