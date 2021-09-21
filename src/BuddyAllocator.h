#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <assert.h>
#include <math.h>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
	int block_size;  // size of the block
	BlockHeader* next; // pointer to the next block
	char isFree; // 1 means free, 0 is not free
	BlockHeader(int bSize = 0) {
		block_size = bSize;
		next = nullptr;
		isFree = 1;
	};
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
public:
	BlockHeader* head;		// you need a head of the list
	int _size;
public:
	LinkedList(BlockHeader* h = nullptr) {
		head = h;
		if (head != nullptr) {
			_size = 1;
		} else {
			_size = 0;
		}
	}
	int size() {return _size;};
	void insert(BlockHeader* block);
	void remove(BlockHeader* block);

	BlockHeader* remove () {  // removes and returns first item from the list
		assert (head != nullptr);
		BlockHeader* b = head;
		head = head->next;
		_size--;
		return b;
	};
};

class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;
	char* start;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	BlockHeader* getbuddy (BlockHeader * addr); 
		// buddy address = ((blockaddress - start) XOR blockSize) + start
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2) {
		BlockHeader* b1Buddy = getbuddy(block1);
		if (b1Buddy == block2) {
			return true;
		}

		return false;
	}
	// checks whether the two blocks are buddies are not

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around
	// return the address of the BlockHeader at the front of the merged block
	/*
		1. Check that the two blocks are buddies
		2. Remove both blocks from the FreeList
		3. Update the BlockHeader of the left block
			- Size of the block
		4. Add new, bigger block to FreeList
		5. Return the left block's BlockHeader
	*/



	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected
	/*
		return first  block address of second block
		1. Remove provided block from FreeList
		2. Update size in provided BlockHeader* to be half of original value
		3. Find the buddy of the provided BlockHeader*
			- Use getbuddy() with the new block size
		4. Fill the first sizeof(BlockHeader) bytes of the buddy with relevant info from the BlockH.
			- Use typecast
		5. Add both blocks to FreeList at new size
		6. Return buddy's pointer
	*/
	int getIndex(int size) {
		return log2 (size/basic_block_size);
	}

	int freeListINdexForSize(int size, bool checkValidity = true) {
		int basicPowerOf2 = this->basic_block_size;
		int sizePowerOf2 = size;

		int index = (sizePowerOf2 - basicPowerOf2);

		if (checkValidity && index > FreeList.size()) {
			return 0;
		}

		return index;
	}


public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	// address of usable memory is _a

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok and -1 if it failed. */ 
	/*
		1. Shift address from usable memory to BlockH
		2. Check if block is on FreeList
			- if so return non-zero
		3. Add block to FreeList
		4. Check if buddy is free
			- Use getbuddy() and check free bool if you choose to use one in BlockHeader
			- Otherwise, iterate through FreeList of same size and call arebuddies() for each address until true
		5. If buddy is free, call merge and go back to 4
			- Continue until we reaach total memory size or buddy is not free
		6. If everything is smooth, return 0
	*/



	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 
