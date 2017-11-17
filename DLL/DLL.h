// Le bloc ifdef suivant est la façon standard de créer des macros qui facilitent l'exportation 
// à partir d'une DLL. Tous les fichiers contenus dans cette DLL sont compilés avec le symbole TRAVAIL1_EXPORTS
// défini sur la ligne de commande. Ce symbole ne doit pas être défini pour un projet
// qui utilisent cette DLL. De cette manière, les autres projets dont les fichiers sources comprennent ce fichier considèrent les fonctions 
// TRAVAIL1_API comme étant importées à partir d'une DLL, tandis que cette DLL considère les symboles
// définis avec cette macro comme étant exportés.
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