#include"score.h"
#include<string>
using namespace std;

void main()
{
	score x(10,3,1);						// Maximum number of players, Maximum characters per playername, true for descending false for ascending
	x.SetFilePath("datafile.txt");			// string file path
	x.UpdateScores( "HIMA" , 10000 );		// string playername , int playerscore
}
