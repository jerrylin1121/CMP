#include "virtualmemory.h"
bool page_fault;
Cache *I_cache, *D_cache;
TLB *I_TLB, *D_TLB;
Page_Table *I_pagetable, *D_pagetable;
Memory *I_memory, *D_memory;
fstream report("_report.rpt", fstream::out);
void Instruction_Memory(int memory_size, int page_size, int cache_size, int block_size, int associative)
{
	I_cache = new Cache(cache_size, block_size, associative, page_size);
	I_TLB = new TLB(page_size);
	I_pagetable = new Page_Table(page_size);
	I_memory = new Memory(memory_size, page_size);
}

void Data_Memory(int memory_size, int page_size, int cache_size, int block_size, int associative)
{
	D_cache = new Cache(cache_size, block_size, associative, page_size);
	D_TLB = new TLB(page_size);
	D_pagetable = new Page_Table(page_size);
	D_memory = new Memory(memory_size, page_size);
}

void Access_IMemory(int address)
{
	if(I_TLB->is_hit(address)){
		int PPN = I_TLB->get_PPN(address);
		PPN = PPN*I_memory->page_size + address%I_memory->page_size;
		if(I_cache->is_hit(PPN)){
		}else{
			I_cache->update(PPN);
		}
	}else{
		if(I_pagetable->is_hit(address)){
			int PPN = I_pagetable->get_PPN(address);
			PPN = PPN*I_memory->page_size + address%I_memory->page_size;
			I_TLB->update(address, PPN);
			if(I_cache->is_hit(PPN)){
			}else{
				I_cache->update(PPN);
			}
		}else{
			int PPN = I_memory->swap_from_disk();
			I_pagetable->update(address,PPN);
			I_TLB->update(address, PPN);
			PPN = PPN*I_memory->page_size + address%I_memory->page_size;
			I_cache->update(PPN);
		}
	}
}

void Access_DMemory(int address)
{
	if(D_TLB->is_hit(address)){
		int PPN = D_TLB->get_PPN(address);
		PPN = PPN*D_memory->page_size + address%D_memory->page_size;
		if(D_cache->is_hit(PPN)){
		}else{
			D_cache->update(PPN);
		}
	}else{
		if(D_pagetable->is_hit(address)){
			int PPN = D_pagetable->get_PPN(address);
			PPN = PPN*D_memory->page_size + address%D_memory->page_size;
			D_TLB->update(address, PPN);
			if(D_cache->is_hit(PPN)){
			}else{
				D_cache->update(PPN);
			}
		}else{
			int PPN = D_memory->swap_from_disk();
			D_pagetable->update(address,PPN);
			D_TLB->update(address, PPN);
			PPN = PPN*D_memory->page_size + address%D_memory->page_size;
			D_cache->update(PPN);
		}
	}
}
