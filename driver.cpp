#include <iostream>
#include <fstream>
#include <string>
#include "stdlib.h"
#include "DisSim.h"

using namespace std;

int main(int argc, char *argv[])
{	
	switch( argc )
	{
	case 1:{
				string filename;
				cout<<"Please Enter the file name: ";
				cin>>filename;
				char* charfilename = (char *)alloca(filename.size() + 1);
				memcpy(charfilename, filename.c_str(), filename.size() + 1);
				int dashes=80; while(dashes--) cout<<'-';
				DisSim x( charfilename );
				break;
		   }
	case 2:{
				DisSim x( argv[1] );
				break;
		   }
	default:{
				cout<<"Error with input! ";
			}
	}

	system ("pause");
}