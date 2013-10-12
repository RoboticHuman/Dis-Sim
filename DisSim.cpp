#include "DisSim.h"
#include "stdlib.h"
#include <string>
#include <sstream>

void DisSim::emitError(char *s)
{
	cout<< s;
	exit(0);
}

char* DisSim::decodeR(unsigned int instWord)
{
	unsigned int rd, rs, rt, func, shamt, imm;
	std::ostringstream strstream;
	std::string str;
	unsigned int adress;
	func = instWord & 0x3F;
	shamt = (instWord>>6) & 0x1f;
	rd    = (instWord>>11) & 0x1f;
	rt    = (instWord>>16) & 0x1f;
	rs    = (instWord>>21) & 0x1f;
	switch(func)
	{
	case 0x20: // needs exception handling
		{
		strstream << "add\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x21:
		{
		strstream << "addu\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x22:
		{
		strstream << "sub\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x24:
		{
		strstream << "and\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x25:
		{
		strstream << "or\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x26:
		{
		strstream << "xor\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x2A:
		{
		strstream << "slt\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x2:
		{
        strstream << "srl\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", " << dec << shamt;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x0:
		{
		strstream << "sll\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", " << dec << shamt;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0xC:
		{
			return "SYSCALL";
		}
		break;
	default:
		{
			return "Unkown R-Format Instruction";
		}
	}
}

char* DisSim::decodeInst(unsigned int instWord)
{
	unsigned int opcode;
	opcode = instWord >> 26;
	if(0 == (opcode))
	{
		current_Instr_Address += 4;
		return( decodeR(instWord) );
	}
	else if(0!= opcode && 2!=opcode && 3!=opcode && 16!=opcode && 17!=opcode && 18!=opcode && 19!=opcode)
	{
		current_Instr_Address += 4;
		return ( decodeI(instWord) );
	}
	else if(opcode==2 || opcode==3)
	{
		current_Instr_Address += 4;
		return ( decodeJ(instWord) );
	}
	else
	{
		std::ostringstream strstream;
		strstream << "Opcode: " << opcode << "is referring to an unknown instruction\n";
		std:: string str = strstream.str();
		char * error = (char *)alloca(str.size() + 1);
		memcpy(error, str.c_str(), str.size() + 1);
		return error;
	}
}

DisSim::DisSim(char * in , char * out)
{
	regs[0]=0;

	current_Instr_Address = 0x00400000;	
	memory_Address = 0x10010000;

	inFile.open(in);
	outFile.open(out);

	if(inFile.fail())
		emitError("Cannot access input file\n");
	if(outFile.fail())
		emitError("Cannot access output file\n");
	if(!inFile.fail() && !outFile.fail())
	{
		unsigned int instWord=0;
		while(inFile.read ((char *)&instWord, 4))
			outFile<<decodeInst(instWord)<<endl;
	}

	// mapping the register numbers to names
	char* tempr[32] = { "zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"};
	for( int i=0 ; i<32 ; i++ )
		regNames.insert(pair<int,char *>(i,tempr[i]));
}