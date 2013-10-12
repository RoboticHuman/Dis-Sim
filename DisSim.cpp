#include "DisSim.h"
#include "stdlib.h"
#include <string>
#include <sstream>

using namespace std;

DisSim::DisSim(char * in , char * out)
{
	regs[0]=0;

	current_Instr_Address = 0x00400000;	
	memory_Address = 0x10010000;
	exitFlag=false;

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
		{
			Instr_Addresses.insert(pair<int, unsigned int>( current_Instr_Address , instWord ));
			outFile<<decodeInst(instWord)<<endl;
			current_Instr_Address += 4;
		}
		current_Instr_Address = 0x00400004;		// reseting the instruction address to start execution + 4
		while(!exitFlag)
		{
			ExecuteInst(Instr_Addresses.at(current_Instr_Address));
			current_Instr_Address += 4;
		}
	}

	// mapping the register numbers to names
	char* tempr[32] = { "zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"};
	for( int i=0 ; i<32 ; i++ )
		regNames.insert(pair<int,char *>(i,tempr[i]));
}
void DisSim::emitError(char *s)
{
	cout<< s;
	exit(0);
}

char* DisSim::decodeInst(unsigned int instWord)
{
	unsigned int opcode;
	opcode = instWord >> 26;
	if(0 == (opcode))
	{
		return( decodeR(instWord) );
	}
	else if(0!= opcode && 2!=opcode && 3!=opcode && 16!=opcode && 17!=opcode && 18!=opcode && 19!=opcode)
	{
		return ( decodeI(instWord) );
	}
	else if(opcode==2 || opcode==3)
	{
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

char* DisSim::decodeR( unsigned int instWord)
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
		strstream << "0x" << hex << current_Instr_Address << "add\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x21:
		{
		strstream << "0x" << hex << current_Instr_Address << "addu\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x22:
		{
		strstream << "0x" << hex << current_Instr_Address << "sub\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x24:
		{
		strstream << "0x" << hex << current_Instr_Address << "and\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x25:
		{
		strstream << "0x" << hex << current_Instr_Address << "or\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x26:
		{
		strstream << "0x" << hex << current_Instr_Address << "xor\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x2A:
		{
		strstream << "0x" << hex << current_Instr_Address << "slt\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", $" << regNames.find(rt)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x2:
		{
        strstream << "0x" << hex << current_Instr_Address << "srl\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", " << dec << shamt;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
	memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	case 0x0:
		{
		strstream << "0x" << hex << current_Instr_Address << "sll\t$" << regNames.find(rd)->second <<", $" << regNames.find(rs)->second <<", " << dec << shamt;
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
	case 0x8:
		{
		strstream << "0x" << hex << current_Instr_Address << "jr\t$" << regNames.find(rs)->second;
		str = strstream.str();
		char * operation = (char *)alloca(str.size() + 1);
		memcpy(operation, str.c_str(), str.size() + 1);
		return operation;
		}
		break;
	default:
		{
			return "Unkown R-Format Instruction";
		}
	}
}

void DisSim::ExecuteInst( unsigned int instWord)
{
	unsigned int opcode;
	opcode = instWord >> 26;
	if(0 == (opcode))
	{
		ExecuteR(instWord);
	}
	else if(0!= opcode && 2!=opcode && 3!=opcode && 16!=opcode && 17!=opcode && 18!=opcode && 19!=opcode)
	{
		ExecuteI(instWord);
	}
	else if(opcode==2 || opcode==3)
	{
		ExecuteJ(instWord);
	}
}

void DisSim::ExecuteR( unsigned int instWord)
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
	case 0x20:
		{	//add
			regs[rd] = regs[rs] + regs[rt];
		}
		break;
	case 0x21:
		{	//addu
			regs[rd] = ( (unsigned int)regs[rs] + (unsigned int)regs[rt];
		}
		break;
	case 0x22:
		{	//sub
			regs[rd] = regs[rs] - regs[rt];
		}
		break;
	case 0x24:
		{	//and
			regs[rd] = regs[rs] & regs[rt];
		}
		break;
	case 0x25:
		{	//or
			regs[rd] = regs[rs] | regs[rt];
		}
		break;
	case 0x26:
		{	//xor
			regs[rd] = regs[rs] ^ regs[rt];
		}
		break;
	case 0x2A:
		{	//slt
			regs[rd] = (regs[rs] < regs[rt]);
		}
		break;
	case 0x2:
		{	//srl
			regs[rd] = regs[rt] >> shamt;
		}
		break;
	case 0x0:
		{	//sll
			regs[rd] = regs[rt] << shamt;
		}
		break;
	case 0x8:
		{	//jr
			current_Instr_Address = regs[rs] - 4;
		}
		break;
	case 0xC:
		{	//syscall
			if(regs[2] == 1)
			{
				cout<< regs[4];
			}
			else if(regs[2] == 4)
			{
				cout<< regs[4];
			}
			else if(regs[2] == 10)
			{
				exitFlag=true;
			}
		}
	default:
		{
			//unknown format so no implementation.
		}
	}
}

char* DisSim::decodeI( unsigned int instWord)
{
	unsigned int opcode, rs, rt, imm;

	opcode	= (instWord>>26);
	rt    	= (instWord>>16)& 0x1f;
	rs    	= (instWord>>21)& 0x1f;
	imm	  	= (instWord    )& 0xffff;

	stringstream strs;

	switch(opcode)
	{
	case 4: {
				// beq
				strs<< "0x" << hex << current_Instr_Address << "\tbeq\t$" << regNames.at(rs) << ",\t" << regNames.at(rt) << ",\t0x" << hex << imm;
				break;
			}
	case 5: {
				// bne
				strs<< "0x" << hex << current_Instr_Address << "\tbne\t$" << regNames.at(rs) << ",\t" << regNames.at(rt) << ",\t0x" << hex << imm;
				break;
			}
	case 8:	{
				// addi
				strs<< "0x" << hex << current_Instr_Address << "\taddi\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t" << dec << imm;
				break;
			}
	case 9:	{
				// addiu
				strs<< "0x" << hex << current_Instr_Address << "\taddiu\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t" << dec << imm;
				break;
			}
	case 10:{
				// slti
				strs<< "0x" << hex << current_Instr_Address << "\tslti\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t" << dec << imm;
				break;
			}
	case 11:{
				// sltiu
				strs<< "0x" << hex << current_Instr_Address << "\tsltiu\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t" << dec << imm;
				break;
			}
	case 12:{
				// andi
				strs<< "0x" << hex << current_Instr_Address << "\tandi\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t0x" << hex << imm;
				break;
			}
	case 13:{
				// ori
		strs<< "0x" << hex << current_Instr_Address << "\tori\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t0x" << hex << imm;
				break;
			}
	case 14:{
				// xori
		strs<< "0x" << hex << current_Instr_Address << "\txori\t$" << regNames.at(rt) << ",\t" << regNames.at(rs) << ",\t0x" << hex << imm;
				break;
			}
	case 15:{
				// lui
				break;
			}
	case 32:{
				// lb
				break;
			}
	case 33:{
				// lh
				break;
			}
	case 35:{
				// lw
				break;
			}
	case 36:{
				// lbu
				break;
			}
	case 37:{
				// lhu
				break;
			}
	case 40:{
				// sb
				break;
			}
	case 41:{
				// sh
				break;
			}
	case 43:{
				// sw
				break;
			}
	default:{
				// Unkown I-Format Instruction
			}
	}


	
	string temp_str = strs.str();
	char* char_type = (char*) temp_str.c_str();
	return ( char_type );
}

void DisSim::ExecuteI( unsigned int instWord)
{
	
	unsigned int opcode, rs, rt, imm;

	opcode	= (instWord>>26);
	rt    	= (instWord>>16)& 0x1f;
	rs    	= (instWord>>21)& 0x1f;
	imm	  	= (instWord    )& 0xffff;

	switch(opcode)
	{
	case 4: {
				// beq
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				if( regs[rs] == regs[rt] )
					current_Instr_Address += sImm;
				break;
			}
	case 5: {
				// bne
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				if( regs[rs] == regs[rt] )
					current_Instr_Address += sImm;
				break;
			}
	case 8:	{
				// addi
				regs[rt] = regs[rs] + imm;
				break;
			}
	case 9:	{
				// addiu
				regs[rt] = (unsigned int)regs[rs] + (unsigned int) imm;
				break;
			}
	case 10:{
				// slti
				regs[rt] = (regs[rs] < imm);
				break;
			}
	case 11:{
				// sltiu
				regs[rt] = ( (unsigned int) regs[rs] < (unsigned int) imm);
				break;
			}
	case 12:{
				// andi
				regs[rt] = regs[rs] & imm ;
				break;
			}
	case 13:{
				// ori
				regs[rt] = regs[rs] | imm ;
				break;
			}
	case 14:{
				// xori
				regs[rt] = regs[rs] ^ imm ;
				break;
			}
	case 15:{
				// lui
				regs[rt] = imm << 16;
				break;
			}
	case 32:{
				// lb
				int sImm  = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				regs[rt] = memory[(regs[rs] + sImm) - memory_Address];
				break;
			}
	case 33:{
				// lh
				int sImm  = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				regs[rt] = memory[(regs[rs] + sImm) - memory_Address] << 8;
				regs[rt] |= memory[(regs[rs] + sImm + 1) - memory_Address];
				break;
			}
	case 35:{
				// lw
				int sImm  = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				regs[rt]  = memory[(regs[rs] + sImm) - memory_Address] << 24;
				regs[rt] |= memory[(regs[rs] + sImm + 1) - memory_Address] << 16;
				regs[rt] |= memory[(regs[rs] + sImm + 2) - memory_Address] << 8;
				regs[rt] |= memory[(regs[rs] + sImm + 3) - memory_Address];
				break;
			}
	case 40:{
				// sb
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				memory[(regs[rs] + sImm) - memory_Address] = regs[rt];
				break;
			}
	case 41:{
				// sh
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				memory[(regs[rs] + sImm ) - memory_Address] = regs[rt]>>8;
				memory[(regs[rs] +1 + sImm ) - memory_Address] = regs[rt]& 0x000000ff;
				break;
			}
	case 43:{
				// sw
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				memory[(regs[rs] + sImm) - memory_Address] = regs[rt]>>24;
				memory[(regs[rs] + sImm)+1 - memory_Address] = (regs[rt]>>16) & 0x000000ff;
				memory[(regs[rs] + sImm)+2 - memory_Address] = (regs[rt]>>8)  & 0x000000ff;
				memory[(regs[rs] + sImm)+3 - memory_Address] = regs[rt] & 0x000000ff;
				break;
			}
	default:{
				// Unkown I-Format Instruction
			}
	}
}
