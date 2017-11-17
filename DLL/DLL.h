// Le bloc ifdef suivant est la fa�on standard de cr�er des macros qui facilitent l'exportation 
// � partir d'une DLL. Tous les fichiers contenus dans cette DLL sont compil�s avec le symbole TRAVAIL1_EXPORTS
// d�fini sur la ligne de commande. Ce symbole ne doit pas �tre d�fini pour un projet
// qui utilisent cette DLL. De cette mani�re, les autres projets dont les fichiers sources comprennent ce fichier consid�rent les fonctions 
// TRAVAIL1_API comme �tant import�es � partir d'une DLL, tandis que cette DLL consid�re les symboles
// d�finis avec cette macro comme �tant export�s.
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

extern "C" DLL_API void Rotation(char* messageReceived, int chiffre);
extern "C" DLL_API void encodeMessage(int position, char* messageReceived, bool isChiffrer);
extern "C" DLL_API void AlgoTrois(char* messageReceived);

static class Verification
{
public:
	static bool containsSpecialCharacter(string toCheck);
	static bool containsPourcentage(string toCheck);
};