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
				cout<<"Please Enter the text segment file name: ";
				cin>>filename;
				if( filename.find(".bin")==-1 )
				{
					filename+=".bin";
				}
				char* charfilename = (char *)alloca(filename.size() + 1);
				memcpy(charfilename, filename.c_str(), filename.size() + 1);
				int dashes=80; while(dashes--) cout<<'-';

				string filename2;
				cout<<"Please Enter the data segment file name: ";
				cin>>filename2;
				if( filename2.find(".bin")==-1 )
				{
					filename2+=".bin";
				}
				char* charfilename2 = (char *)alloca(filename2.size() + 1);
				memcpy(charfilename2, filename2.c_str(), filename2.size() + 1);
				dashes=80; while(dashes--) cout<<'-';
				
				DisSim x( charfilename , charfilename2);
				break;
		   }
	case 3:{
				DisSim x( argv[1] , argv[2]);
				break;
		   }
	default:{
				cout<<"Error with input! ";
			}
	}

	system ("pause");
}