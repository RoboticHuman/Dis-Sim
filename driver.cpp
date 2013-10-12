#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "DisSim.h"

using namespace std;

int main(int argc, char *argv[])
{	
	switch( argc )
	{
	case 2:{
				DisSim x( argv[1] , "outfile.txt" );
				break;
		   }
	case 3:{
				DisSim x( argv[1] , argv[2] );
				break;
		   }
	default:{
				cout<<"Error with input file path! ";
			}
	}

	system ("pause");
}