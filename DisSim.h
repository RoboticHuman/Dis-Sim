#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <map>
//#include <string> // if needed..


using namespace std;

#ifndef DIS_SIM_H
#define DIS_SIM_H

// This is just a cooler way to write the skeleton that the dr. sent..

class DisSim
{
private:

	// Variables
	int regs[32];								// The size of each register is 32 bits (int)
	char memory[8*1024];						// each element in the memory is 8 bits (1 char) and we use 8 kilo bytes
	ifstream inFile;							// The binary code input file stream
	ofstream outFile[2];							// The Assembly code output file stream
	ofstream regTrace;
	map< int , char * > regNames;				// Mapping the register numbers with their names
	unsigned int current_Instr_Address;			// initial address = 0x00400000 and is incremented by 4 each time
	unsigned int memory_Address;
	map< int , unsigned int > Instr_Addresses;
	bool exitFlag;
	map< unsigned int , int > Labels;
	int labelCount;
	bool bLabel;

	// Functions
	void emitError( char * );			// This was on the professors skeleton.. will see what is it for
	char * decodeInst( unsigned int );	// Takes the binary line from the constructor, extracts the opcode and
										// sends the binary line to either R,I or J depending on the opcode
										// it then recieves the translation from either and returns it to the constructor
										// It also increases the value of (current_Instr_Address) by 4 each time because each line is 4 bytes
	char * decodeR( unsigned int );		
	char * decodeI( unsigned int );
	char * decodeJ( unsigned int );
	void ExecuteInst( unsigned int );
	void ExecuteR( unsigned int );
	void ExecuteI( unsigned int );
	void ExecuteJ( unsigned int );

public:
	DisSim( char * , char * );			// The constructor recieves the input and output file paths
										// Opens the files, takes instruction by instruction sends it to decodeInst
										// Recieves the assembly translation of the instruction and puts it in the out file followed by \n
	~DisSim();
};

#endif