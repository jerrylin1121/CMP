#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "instruction.h"
#include "memory.h"
#include "regfile.h"
#include "virtualmemory.h"

using namespace std;
fstream snap("_snapshot.rpt", fstream::out);
int cycle = 0;
bool halt = false;

int main(int argc, char *argv[])
{
	int MemorySize[] = {64,32,8,16,16,4,4,16,4,1};
    ifstream iin("./iimage.bin", ios::in | ios::binary);
    ifstream din("./dimage.bin", ios::in | ios::binary);
	load_instruction(&iin);
	load_data(&din);
	for(int i=0; i<argc-1 & i<10; ++i){
		MemorySize[i] = atoi(argv[i+1]);
	}
	Instruction_Memory(MemorySize[0], MemorySize[2], MemorySize[4], MemorySize[5], MemorySize[6]);
	Data_Memory(MemorySize[1], MemorySize[3], MemorySize[7], MemorySize[8], MemorySize[9]);
	for(int i=0; i<34; ++i){
		show_set.insert(i);
	}
	while(!halt){
		snap << "cycle " << dec << cycle << endl;
		show_reg();
		++cycle;
		Access_IMemory(reg_value[PC]);
		reg_value[PC] += 4;
		decode_instructions(ins_mem[(reg_value[(PC)]/4)-1]);
	}
	report << "ICache :" << endl;
	I_cache->print();
	report << "DCache :" << endl;
	D_cache->print();
	report << "ITLB :" << endl;
	I_TLB->print();
	report << "DTLB :" << endl;
	D_TLB->print();
	report << "IPageTable :" << endl;
	I_pagetable->print();
	report << "DPageTable :" << endl;
	D_pagetable->print();
    return 0;
}
