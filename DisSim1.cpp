#include "DisSim.h"
#include "stdlib.h"
#include <string>
#include <sstream>
#include <iomanip>


using namespace std;


DisSim::DisSim(char * in , char* in2)
{
	regs[0]=0;
	j = false;
	current_Instr_Address = 0x00400000;	
	memory_Address = 0x10010000;
	exitFlag=false;
	labelCount = 1;
	bLabel = 0;
	bLI = 0;

	cout<<Cwhite;

	ifstream datafile;
	datafile.open(in2 , ios::in | ios::binary);
	if( !datafile.fail() )
	{
		char singlebyte;
		for(int i=0 ; i< (8*1024) ; i++ )
		{
			datafile.read ((char *)&singlebyte, 1);
			memory[i]=singlebyte;
		}
	

		// mapping the register numbers to names
		char* tempr[32] = { "zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"};
		for( int i=0 ; i<32 ; i++ )
			regNames[i]=tempr[i];

		inFile.open(in , ios::in | ios::binary);
		outFile[0].open("standard output file.txt");
		regTrace.open("register trace.txt");
		outFile[1].open("Label file.txt");
	
		if(inFile.fail())
			emitError("Cannot access input file\n");
		else
		{
			unsigned int instWord=0;
			cout<< Cgreen << "# Disassembled code:\n" << Cwhite;
			cout<< Cred << ".text\n";
			cout<<".globl " << Cwhite << "main\n";
			cout<<"main:\n";
			for( int i=0 ; i<2 ; i++ )
			{
				while(inFile.read ((char *)&instWord, 4))
				{
					if( !i )
						Instr_Addresses.insert(pair<int, unsigned int>( current_Instr_Address , instWord ));
					char* tempstr = decodeInst(instWord);
					if( !bLI )
					{	
						outFile[i]<<tempstr<<endl;
						if(bLabel)
							//cout<<tempstr<<endl;
							DisplayColor(tempstr);
					}
					current_Instr_Address += 4;
				}
				current_Instr_Address = 0x00400000;		// reseting the instruction address to start execution
				bLabel=true;
				inFile.close();
				inFile.open(in , ios::in | ios::binary);
			}

			if(j)
				free(char_type1);
			if(i)
				free(operation_I);
			if(r)
				free(operation_R);

			int dashes=80;	while(dashes--) cout<<"-";
			cout<< Cgreen << "Excution: \n" << Cwhite;

			for( int i=0 ; i<32 ; i++ )
					regTrace<< setw( 14 ) << regNames[i];
				regTrace << endl;

			do
			{
				for( int i=0 ; i<32 ; i++ )
					regTrace<< setw( 14 ) << hex << regs[i];
				regTrace << endl;
				ExecuteInst(Instr_Addresses.at(current_Instr_Address));
				current_Instr_Address += 4;
			}while(!exitFlag);
				cout<<endl;
			dashes=80;	while(dashes--) cout<<"-";
		}
	}
	else
		emitError("Data File Corrupted!");
}
void DisSim::emitError(char *s)
{
	cout<< s;
	exit(0);
}
char* DisSim::decodeInst( unsigned int instWord)
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
void DisSim::ExecuteInst( unsigned int instWord)
{
	regs[0] = 0;
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

char* DisSim::decodeR( unsigned int instWord)
{
	if(R)
		free(operation_R);
	unsigned int rd, rs, rt, func, shamt;
	std::ostringstream strstream;
	std::string str;
	func = instWord & 0x3F;
	shamt = (instWord>>6) & 0x1f;
	rd    = (instWord>>11) & 0x1f;
	rt    = (instWord>>16) & 0x1f;
	rs    = (instWord>>21) & 0x1f;
	switch(func)
	{
	case 0x20: // needs exception handling
		{
		if( !bLabel )
			strstream<< "0x" << hex << current_Instr_Address<< "\tadd\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		else
		{
			if( Labels.find(current_Instr_Address) != Labels.end() )
				strstream<< "label" << Labels.at(current_Instr_Address) << ":";
			if(rt == 0)
			{
				strstream<< "\tmove\t$" << regNames[rd] <<",\t$" << regNames[rs];
			}
			else if(rs == 0)
			{
				strstream<< "\tmove\t$" << regNames[rd] <<",\t$" << regNames[rt];
			}
			else
				strstream<< "\tadd\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
			}
		}
		break;
	case 0x21:
		{
		if( !bLabel )
			strstream<< "0x" << hex << current_Instr_Address<< "\taddu\t$" <<regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		else
		{
			if( Labels.find(current_Instr_Address) != Labels.end() )
				strstream<< "label" << Labels.at(current_Instr_Address) << ":";
			if(rt == 0)
			{
				strstream<< "\tmove\t$" << regNames[rd] <<",\t$" << regNames[rs];
			}
			else if(rs == 0)
			{
				strstream<< "\tmove\t$" << regNames[rd] <<",\t$" << regNames[rs];
			}
			else
				strstream<< "\taddu\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
			}
		}
		break;
	case 0x22:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\tsub\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		}
		break;
	case 0x24:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\tand\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		}
		break;
	case 0x25:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\tor\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		}
		break;
	case 0x26:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\txor\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		}
		break;
	case 0x2A:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\tslt\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t$" << regNames[rt];
		}
		break;
	case 0x2:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
        strstream<< "\tsrl\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t" << dec << shamt;
		}
		break;
	case 0x0:
		{
		if( !bLabel )
		strstream<< "0x" << hex << current_Instr_Address;
		else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strstream<< "label" << Labels.at(current_Instr_Address) << ":";
				}
		strstream<< "\tsll\t$" << regNames[rd] <<",\t$" << regNames[rs] <<",\t" << dec << shamt;
		}
		break;
	case 0xC:
		{
			if(bLabel)
			return "\tSYSCALL";
			else
				return "\t\tSYSCALL";
		}
		break;
	default:
		{
			return "Unkown R-Format Instruction";
		}
	}

	r = true;
	str = strstream.str();
	char * operation_R = (char *)malloc(str.size() + 1);
    memcpy(operation_R, str.c_str(), str.size() + 1);
	return operation_R;
}
char* DisSim::decodeI( unsigned int instWord)
{
	if(i)
		free(operation_I);
	unsigned int opcode, rs, rt, imm;

	opcode	= (instWord>>26);
	rt    	= (instWord>>16)& 0x1f;
	rs    	= (instWord>>21)& 0x1f;
	imm	  	= (instWord    )& 0xffff;

	stringstream strs;
	int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;

	switch(opcode)
	{
	case 4: {
				// beq
				imm = imm << 2;
				sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				if ( !bLabel )
				{
					strs<< "0x" << hex << current_Instr_Address; 
					if( Labels.find(current_Instr_Address + 4 + sImm) == Labels.end() )
						Labels[current_Instr_Address + 4 + sImm] = labelCount++;
				}
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tbeq\t$" << regNames[rs] << ",\t$" << regNames[rt] << ",\t" ;
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address + 4 + sImm;
				else
					strs<< "label" <<Labels[current_Instr_Address + 4 + sImm];
				break;
			}
	case 5: {
				// bne
				imm = imm << 2;
				sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				if ( !bLabel )
				{
					strs<< "0x" << hex << current_Instr_Address; 
					if( Labels.find(current_Instr_Address + 4 + sImm) == Labels.end() )
						Labels[current_Instr_Address + 4 + sImm] = labelCount++;
				}
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tbne\t$" << regNames[rs] << ",\t$" << regNames[rt] << ",\t" ;
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address + 4 + sImm;
				else
					strs<< "label" <<Labels[current_Instr_Address + 4 + sImm];
				break;
			}
	case 8:	{
				// addi
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address<< "\taddi\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				
					if( !rs )
					{
						strs<< "\tli\t$" << regNames[rt] << ",\t" << dec << imm;
					}
					else
					{
						if( sImm > 0 )
							strs<< "\taddi\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
						else
							strs<< "\tsubi\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << -sImm;
					}
				}
				break;
			}
	case 9:	{
				// addiu
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address<< "\taddiu\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				
					if( !rs )
						strs<< "\tli\t$" << regNames[rt] << ",\t" << dec << imm;
					else
						strs<< "\taddiu\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
				}
				break;
			}
	case 10:{
				// slti
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tslti\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
				break;
			}
	case 11:{
				// sltiu
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tsltiu\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t" << dec << imm;
				break;
			}
	case 12:{
				// andi
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tandi\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t0x" << hex << imm;
				break;
			}
	case 13:{
				// ori
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address<< "\tori\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t0x" << hex << imm;
				else
				{
					if( !bLI )
					{
						if( Labels.find(current_Instr_Address) != Labels.end() )
							strs<< "label" << Labels.at(current_Instr_Address) << ":";
						strs<< "\tori\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t0x" << hex << imm;
					}
					else
					{
						if( Labels.find(current_Instr_Address-4) != Labels.end() )
							strs<< "label" << Labels.at(current_Instr_Address-4) << ":";
						unsigned int myTempVal = ((upperHalfWord<<16)|imm);
						if( myTempVal >= memory_Address && myTempVal<= (8*1024 + memory_Address))
							strs<< "\tla\t$";
						else
							strs<< "\tli\t$";
						strs<< regNames[rt] << ",\t0x" << hex << myTempVal;
						bLI = 0;
					}
				}
				break;
			}
	case 14:{
				// xori
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\txori\t$" << regNames[rt] << ",\t$" << regNames[rs] << ",\t0x" << hex << imm;
				break;
			}
	case 15:{
				// lui
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				if( !bLabel || rt != 1 )
					strs<< "\tlui\t$" << regNames[rt] << ",\t0x" << hex << imm;
				else
				{
					upperHalfWord = imm;
					bLI= 1;
				}
				break;
			}
	case 32:{
				// lb
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tlb\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	case 33:{
				// lh
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tlh\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	case 35:{
				// lw
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tlw\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	case 40:{
				// sb
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tsb\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	case 41:{
				// sh
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tsh\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	case 43:{
				// sw
				if( !bLabel )
					strs<< "0x" << hex << current_Instr_Address;
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tsw\t$" << regNames[rt] << ",\t" << dec << imm << "($" << regNames[rs] << ")";
				break;
			}
	default:{
				strs<< "#Unknown I-Format Instruction";
			}
	}


	i = true;
	string temp_str = strs.str();
	char * operation_I  = (char *)malloc(temp_str.size() + 1);
	memcpy(operation_I, temp_str.c_str(), temp_str.size() + 1);
	return ( operation_I );
}
char* DisSim::decodeJ( unsigned int instWord)
{
	 if(j)
		 free(char_type1);
	 unsigned int opcode, address;
	 opcode	 = (instWord>>26);
	 address = ((instWord & 0x03ffffff) << 2) | (current_Instr_Address & 0xf0000000);
	 stringstream strs;

	 switch( opcode)
	 {
	 case 2:{
				// j
		 		if( !bLabel )
				{
					if( Labels.find(address) == Labels.end() )
						Labels.insert(pair<unsigned int,int>(address,labelCount++));
					strs<< "0x" << hex << current_Instr_Address;
				}
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tj\t";
				if( !bLabel )
					strs<< "0x" << hex << address;
				else
					strs<< "label" << Labels.at(address);
				break;
			}
	 case 3:{
				// jal
		 		if( !bLabel )
				{
					if( Labels.find(address) == Labels.end() )
						Labels[address] = labelCount++;
					strs<< "0x" << hex << current_Instr_Address;
				}
				else
				{
					if( Labels.find(current_Instr_Address) != Labels.end() )
						strs<< "label" << Labels.at(current_Instr_Address) << ":";
				}
				strs<< "\tjal\t";
				if( !bLabel )
					strs<< "0x" << hex << address;
				else
					strs<< "label" << Labels[address];
				break;
			}
	 }

	j = true;
	string temp_str = strs.str();
	char_type1 = (char *)malloc(temp_str.size() + 1);
	memcpy(char_type1, temp_str.c_str(), temp_str.size() + 1);
	return ( char_type1 );
}

void DisSim::ExecuteR( unsigned int instWord)
{
	unsigned int rd, rs, rt, func, shamt;
	std::ostringstream strstream;
	std::string str;
	func = instWord & 0x3F;
	shamt = (instWord>>6) & 0x1f;
	rd    = (instWord>>11) & 0x1f;
	rt    = (instWord>>16) & 0x1f;
	rs    = (instWord>>21) & 0x1f;
	switch(func)
	{
	case 0x20:
		{	//add
			unsigned long expchk = regs[rs] + regs[rt];
			if(!(expchk & 0x1111111100000000))
				regs[rd] = regs[rs] + regs[rt];
			else
				emitError("Overflow!");
		}
		break;
	case 0x21:
		{	//addu
			regs[rd] = ( (unsigned int)regs[rs] + (unsigned int)regs[rt]);
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
				int i = 0;
				while ( memory[regs[4] + i - memory_Address] )
				{
					cout<< memory[regs[4] + i++ - memory_Address];
				}
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
				imm = imm <<2;
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				if( regs[rs] == regs[rt] )
					current_Instr_Address += sImm;
				break;
			}
	case 5: {
				// bne
				imm = imm <<2;
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
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;
				regs[rt] = (regs[rs] < sImm);
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
				if( IsBigEndian() )
				{
					regs[rt] = memory[(regs[rs] + sImm) - memory_Address] << 8;
					regs[rt] |= memory[(regs[rs] + sImm + 1) - memory_Address];
				}
				else
				{
					regs[rt] = memory[(regs[rs] + sImm + 1) - memory_Address] << 8;
					regs[rt] |= memory[(regs[rs] + sImm) - memory_Address];
				}
				break;
			}
	case 35:{
				// lw
				int sImm  = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				if( IsBigEndian() )
				{
					regs[rt]  = memory[(regs[rs] + sImm) - memory_Address] << 24;
					regs[rt] |= memory[(regs[rs] + sImm + 1) - memory_Address] << 16;
					regs[rt] |= memory[(regs[rs] + sImm + 2) - memory_Address] << 8;
					regs[rt] |= memory[(regs[rs] + sImm + 3) - memory_Address];
				}
				else
				{
					regs[rt]  = memory[(regs[rs] + sImm + 3) - memory_Address] << 24;
					regs[rt] |= memory[(regs[rs] + sImm + 2) - memory_Address] << 16;
					regs[rt] |= memory[(regs[rs] + sImm + 1) - memory_Address] << 8;
					regs[rt] |= memory[(regs[rs] + sImm ) - memory_Address];
				}
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
				if( IsBigEndian() )
				{
					memory[(regs[rs] + sImm ) - memory_Address] = regs[rt]>>8;
					memory[(regs[rs] +1 + sImm ) - memory_Address] = regs[rt]& 0x000000ff;
				}
				else
				{
					memory[(regs[rs] +1 + sImm ) - memory_Address] = regs[rt]>>8;
					memory[(regs[rs] + sImm ) - memory_Address] = regs[rt]& 0x000000ff;
				}
				break;
			}
	case 43:{
				// sw
				int sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				if( IsBigEndian() )
				{
					memory[(regs[rs] + sImm) - memory_Address] = regs[rt]>>24;
					memory[(regs[rs] + sImm)+1 - memory_Address] = (regs[rt]>>16) & 0x000000ff;
					memory[(regs[rs] + sImm)+2 - memory_Address] = (regs[rt]>>8)  & 0x000000ff;
					memory[(regs[rs] + sImm)+3 - memory_Address] = regs[rt] & 0x000000ff;
				}
				else
				{
					memory[(regs[rs] + sImm)+3 - memory_Address] = regs[rt]>>24;
					memory[(regs[rs] + sImm)+2 - memory_Address] = (regs[rt]>>16) & 0x000000ff;
					memory[(regs[rs] + sImm)+1 - memory_Address] = (regs[rt]>>8)  & 0x000000ff;
					memory[(regs[rs] + sImm) - memory_Address] = regs[rt] & 0x000000ff;
				}
				break;
			}
	default:{
				// Unkown I-Format Instruction
			}
	}
}
void DisSim::ExecuteJ( unsigned int instWord)
{
	unsigned int opcode, address;
	opcode	 = (instWord>>26);
	address = ((instWord & 0x03ffffff) << 2) | (current_Instr_Address & 0xf0000000);

	switch( opcode )
	{
	case 2:{
				// j
				current_Instr_Address = address - 4;
				break;
			}
	case 3:{
				// jal
				regs[31] = current_Instr_Address + 4;
				current_Instr_Address = address - 4;
				break;
			}
	}
}

DisSim::~DisSim()
{
	inFile.close();
	outFile[0].close();
	outFile[1].close();
	regTrace.close();
}

void DisSim::DisplayColor( char*& buff )
{
	char* e = (char *)malloc(40);
	memcpy(e, buff, 40);
	string strbuff( e );
	if( strbuff.find(":")!=-1 )
	{
		cout<<strbuff.substr(0,strbuff.find(":")+1);
		strbuff=strbuff.substr(strbuff.find(":")+1,100);
	}
	if( strbuff.find("$")!=-1 )
	{
		cout<< Cblue <<strbuff.substr(0,strbuff.find("$")) << Cwhite;
		strbuff=strbuff.substr(strbuff.find("$"),100);
		cout<< Cred <<strbuff.substr(0,strbuff.find(",")) << Cwhite << ',';
		strbuff=strbuff.substr(strbuff.find(",")+1,100);
	}
	else
	{
		if( strbuff.find("label")!=-1 )
		{
			cout<< Cblue <<strbuff.substr(0,strbuff.find("label")) << Cwhite << strbuff.substr(strbuff.find("label"),100);
			strbuff="";
		}
		else if( strbuff.find("SYSCALL")!=-1 )
		{
			cout<< Cblue <<strbuff;
			strbuff="";
		}
		else
		{
			cout<< Cgreen <<strbuff;
			strbuff="";
		}
	}
	if( strbuff.find("(")!=-1 )
	{
		cout<< strbuff.substr(0,strbuff.find("$"));
		strbuff.erase(0,strbuff.find("$"));
		cout<< Cred << strbuff.substr(0,strbuff.find(")")) << Cwhite <<')';
		strbuff = "";
	}
	if( strbuff.find("$")!=-1 )
	{
		if( strbuff.find(",")!=-1 )
		{
			cout<< Cred <<strbuff.substr(0,strbuff.find(",")) << Cwhite << ',';
			strbuff=strbuff.substr(strbuff.find(",")+1,100);
		}
		else
		{
			cout<< Cred <<strbuff.substr(0,strbuff.size()) << Cwhite;
			strbuff="";
		}
	}
	if( strbuff.find("$")!=-1 )
	{
		cout<< Cred <<strbuff.substr(0,strbuff.size()) << Cwhite;
		strbuff="";
	}
	cout<< Cwhite << strbuff << endl;
}
bool DisSim::IsBigEndian()
{
	int x = 1;
	return !(*(char *)&x);
}