#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <fstream>
extern bool page_fault;
using namespace std;
extern fstream report;
class Cache{
	int associative;
	int numOfBlock;
	int numOfSet;
	int block_size;
	int page_size;
	bool* valid;
	bool* MRU;
	int* tag;
	int* PPN;
public:
	int hit, miss;
	Cache() {};
	Cache(int totalsize, int blocksize, int associative, int _page_size):associative(associative), page_size(_page_size), block_size(blocksize) {
		numOfBlock = totalsize/blocksize;
		numOfSet = numOfBlock/associative;
		valid = new bool[numOfBlock];
		MRU = new bool[numOfBlock];
		tag = new int[numOfBlock];
		PPN = new int[numOfBlock];
		for(int i=0; i<numOfBlock; ++i){
			valid[i] = false;
		}
		hit = 0;
		miss = 0;
	}
	bool is_hit(int address){
		int _tag = (address/block_size)/numOfSet;
		int index = (address/block_size)%numOfSet;
//		cout << "hit     index : " << index << " tag : " << _tag << endl;
		for(int i=0; i<associative; ++i){
			if(valid[index*associative+i] && _tag == tag[index*associative+i]){
				++hit;
				MRU[index*associative+i] = true;
					if(is_full(index)){
						for(int j=0; j<associative; ++j){
							MRU[index*associative+j] = false;
						}
						MRU[index*associative+i] = true;
					}
				return true;
			}
		}
		return false;
	}
	void update(int address){
		++miss;
		int _PPN = address / page_size;
		int _tag = (address/block_size)/numOfSet;
		int index = (address/block_size)%numOfSet;
//		cout << "update  index : " << index << " tag : " << _tag << " PPN : " << _PPN << endl;
		if(page_fault){
//			cout << "PAGE FAULT!!!" << endl;
			for(int i=0; i<numOfBlock; ++i){
				if(PPN[i]==_PPN){
//					cout << i << endl;
					valid[i] = false;
					MRU[i] = false;
				}
			}
		}
		for(int i=0; i<associative; ++i){
			if(!valid[index*associative+i] || associative==1){
				valid[index*associative+i] = true;
				MRU[index*associative+i] = true;
				tag[index*associative+i] = _tag;
				PPN[index*associative+i] = _PPN;
				if(is_full(index)){
					for(int j=0; j<associative; ++j){
						MRU[index*associative+j] = false;
					}
					MRU[index*associative+i] = true;
				}
				return;
			}
		}		
		for(int i=0; i<associative; ++i){
			if(!MRU[index*associative+i]){
				valid[index*associative+i] = true;
				MRU[index*associative+i] = true;
				tag[index*associative+i] = _tag;
				PPN[index*associative+i] = _PPN;
				if(is_full(index)){
					for(int j=0; j<associative; ++j){
						MRU[index*associative+j] = false;
					}
					MRU[index*associative+i] = true;
				}
				return;
			}
		}
	}
	bool is_full(int index){
		for(int i=0; i<associative; ++i){
			if(!MRU[index*associative+i]){
				return false;
			}
		}
		return true;
	}
	void print(){
/*		cout << "Cache :" << endl;
		for(int i=0; i<numOfBlock; ++i){
			cout << "index : " << i << " valid : " << valid[i] << " MRU : " << MRU[i] << " tag : " << tag[i] << " PPN : " << PPN[i] << endl;
		}
		cout << "# hits: " << hit << endl;
		cout << "# misses: " << miss << endl << endl;
*/		report << "# hits: " << hit << endl;
		report << "# misses: " << miss << endl << endl;
	}
};

class TLB{
	int numOfentry, page_size;
	bool *valid;
	int *tag, *PPN;
	list<int> LRU;
	map<int, int> useage;
public:
	int hit, miss;
	TLB() {};
	TLB(int _page_size):page_size(_page_size) {
		numOfentry = 1024/(page_size*4);
		valid = new bool[numOfentry];
		tag = new int[numOfentry];
		PPN = new int[numOfentry];
		for(int i=0; i<numOfentry; ++i){
			valid[i] = false;
		}
		hit = 0;
		miss = 0;
	}
	bool is_hit(int address){
		page_fault = false;
		int _tag = address / page_size;
		for(int i=0; i<numOfentry; ++i){
			if(valid[i] && _tag == tag[i]){
				++hit;
				auto f = find(LRU.begin(), LRU.end(), i);
				LRU.erase(f);
				LRU.push_back(i);
				return true;
			}
		}
		++miss;
		return false;
	}
	void update(int address, int _PPN){
		if(useage.find(_PPN) != useage.end()){
			valid[useage[_PPN]] = false;
		}
		int i;
		for(i=0; i<numOfentry; ++i){
			if(!valid[i]){
				valid[i] = true;
				tag[i] = address / page_size;
				PPN[i] = _PPN;
				useage[_PPN] = i;
				LRU.push_back(i);
				break;
			}
		}
		if(i==numOfentry){
			int f = LRU.front();
			LRU.pop_front();
			tag[f] = address / page_size;
			PPN[f] = _PPN;
			LRU.push_back(f);
		}
	}
	int get_PPN(int address){
		int _tag = address / page_size;
		for(int i=0; i<numOfentry; ++i){
			if(valid[i] && _tag == tag[i]){
				return PPN[i];
			}
		}
		return 0;
	}
	void print(){
/*		cout << "TLB :" << endl;
		for(int i=0; i<numOfentry; ++i){
			cout << "index : " << i << " valid : " << valid[i] << " tag : " << tag[i] << " PPN : " << PPN[i] << endl;
		}
		cout << "# hits: " << hit << endl;
		cout << "# misses: " << miss << endl << endl;
*/		report << "# hits: " << hit << endl;
		report << "# misses: " << miss << endl << endl;
	}
};

class Page_Table{
	int numOfentry;
	int page_size;
	bool *valid;
	int *PPN;
	map<int, int> useage;
public:
	int hit, miss;
	Page_Table() {};
	Page_Table(int _page_size):page_size(_page_size) {
		numOfentry = 1024/page_size;
		valid = new bool[numOfentry];
		PPN = new int[numOfentry];
		for(int i=0; i<numOfentry; ++i){
			valid[i] = false;
		}
		hit = 0;
		miss = 0;
	}
	bool is_hit(int address){
		int tag = address / page_size;
		if(valid[tag]){
			++hit;
			return true;
		}
		++miss;
		return false;
	}
	void update(int address, int _PPN){
		if(useage.find(_PPN) != useage.end()){
			valid[useage[_PPN]] = false;
		}
		int tag = address / page_size;
		valid[tag] = true;
		PPN[tag] = _PPN;
		useage[_PPN] = tag;
	}
	int get_PPN(int address){
		int tag = address / page_size;
		if(valid[tag]){
			return PPN[tag];
		}
		return 0;
	}
	void print(){
/*		cout << "Pagetable :" << endl;
		for(int i=0; i<numOfentry; ++i){
			cout << "index : " << i << " valid : " << valid[i] << " PPN : " << PPN[i] << endl;
		}
		cout << "# hits: " << hit << endl;
		cout << "# misses: " << miss << endl << endl;
*/		report << "# hits: " << hit << endl;
		report << "# misses: " << miss << endl << endl;
	}
};

class Memory{
	bool *valid;
	list<int> LRU;
public:
	int numOfPage;
	int page_size;
	Memory() {};
	Memory(int memory_size, int _page_size):page_size(_page_size) {
		numOfPage = memory_size / page_size;
		valid = new bool[numOfPage];
	}
	int swap_from_disk(void){
		page_fault = true;
		for(int i=0; i<numOfPage; ++i){
			if(!valid[i]){
				valid[i] = true;
				LRU.push_back(i);
				return i;
			}
		}
		int f = LRU.front();
		LRU.pop_front();
		LRU.push_back(f);
		return f;
	}
	void print(){
/*		cout << "Memory :" << endl;
		for(int i=0; i<numOfPage; ++i){
			cout << "index : " << i << " valid : " << valid[i] << endl;
		}
		cout << endl;
*/	}
};

extern Cache *I_cache, *D_cache;
extern TLB *I_TLB, *D_TLB;
extern Page_Table *I_pagetable, *D_pagetable;
extern Memory *I_memory, *D_memory;
void Instruction_Memory(int,int,int,int,int);
void Access_IMemory(int);
void Data_Memory(int,int,int,int,int);
void Access_DMemory(int);
