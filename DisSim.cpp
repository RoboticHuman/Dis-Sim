#include "DisSim.h"
#include "stdlib.h"


void DisSim::emitError(char *s)
{
	cout<< s;
	exit(0);
}

char* DisSim::decodeInst(unsigned int instWord)
{
	unsigned int rd, rs, rt, func, shamt, imm, opcode;
	unsigned int adress;
	opcode = instWord >> 26;

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