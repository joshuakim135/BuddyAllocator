#include "BuddyAllocator.h"
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <math.h>

using namespace std;

BlockHeader* BuddyAllocator::split (BlockHeader* b) {
  int bs = b->block_size;
  b->block_size /= 2;
  b->next = nullptr;

  BlockHeader* sh = (BlockHeader*)((char*)b + b->block_size);
  // sh->next = nullptr, sh->block_size = b->block_size;
  BlockHeader* temp = new (sh) BlockHeader (b->block_size);
  return sh;
}

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
  total_memory_size = _total_memory_length, basic_block_size = _basic_block_size;
  start = new char[total_memory_size];
  int l = log2 (total_memory_size/basic_block_size);
  for (int i=0; i<l; i++) {
    FreeList.push_back(LinkedList());
  }
  FreeList.push_back (LinkedList((BlockHeader*)start));
  BlockHeader* h = new (start) BlockHeader (total_memory_size);
}

BuddyAllocator::~BuddyAllocator (){
	delete [] start;
}

// length = number of usable bytes [block - header]
// return address of first byte of usable memory not block header
char* BuddyAllocator::alloc(int _length) {
  /*
    1. Calculate smallest block size (sbs) needed for provided length
    2. Use FreeList to find available block of sbs or larger
    3. If block is larger than sbs, call split function until we reach sbs
    4. Once sbs blocks are available on the FreeList, remove one from the FreeList
    5. Shift the address down sizeof(BlockHeader) bytes and return it
  */

  int x = _length + sizeof(BlockHeader);
  cout << x << endl;
  int index = ceil(log2(ceil((double) x / basic_block_size)));
  cout << index << endl;
  int blockSizeReturn = (1 << index) * basic_block_size;

  if (FreeList[index].head != nullptr) { // found a block of correct size
    // return FreeList[index].remove();
    BlockHeader* b = FreeList[index].remove();
    b->isFree = 0;
    return (char*)(b + 1);
  }

  int indexCorrect = index;
  for (; index < FreeList.size(); index++) {
    if (FreeList[index].head) {
      break;
    }
  }

  if (index >= FreeList.size()) { // no bigger block found
    return nullptr;
  }

  // a bigger block found
  while (index > indexCorrect) {
    BlockHeader* b = FreeList[index].remove();
    //b->isFree = 0;
    BlockHeader* shb = split(b);
    --index;
    FreeList[index].insert (b);
    FreeList[index].insert (shb);
  }
  BlockHeader* block = FreeList[index].remove();
  block->isFree = 0;
  return (char*)(block + 1);
}

BlockHeader* BuddyAllocator::merge(BlockHeader* block1, BlockHeader* block2) {
    if (block1 > block2) {
      swap(block1, block2);
    }

    block1->block_size *= 2;
    return block1;
}

void LinkedList::insert(BlockHeader *block) {
		// if head node is null

		if (head == nullptr) {
			head = block;
			_size++;
			return;
		}

		// else traverse till last node
		BlockHeader* current = head;
		while (current->next != nullptr) {
			current = current -> next;
		}

		current -> next = block;
		_size++;
}

void LinkedList::remove(BlockHeader *block) {
  if (head == nullptr) {
			return;
		} else if (head == block) {
			BlockHeader* temp = head;
			cout << head->block_size << endl;
			cout << block->block_size << endl;
			head = head->next;
			_size--;
			delete temp;
			return;
		}

    // else traverse to find match and remove it
		BlockHeader* current = head;
		BlockHeader* prev = nullptr;
		while (current != nullptr) {
			if (current == block) {
				// remove b
				prev->next = current->next;
				delete current;
				return;
			}
			prev = current;
			current = current -> next;
		}
}

int BuddyAllocator::free(char* _a) {
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
  BlockHeader* b = (BlockHeader*)(_a - sizeof(BlockHeader));
  int index = getIndex(b->block_size);
  BlockHeader* header = FreeList[index].head;
  while(header != nullptr) {
    if (header == b) {
      return -1;
    }
    header = header->next;
  }
  FreeList[index].insert(b);
  while(true) {
    if (index >= FreeList.size() - 1) {
      break;
    }
    index = getIndex(b->block_size);
    BlockHeader* buddy = getbuddy(b);
    if ((buddy->isFree == 1)&&(b->block_size == buddy->block_size)) {
      b = merge(b, buddy);
      FreeList[index].remove(b);
      FreeList[index].remove(buddy);
      FreeList[getIndex(b->block_size)].insert(b);
    } else {
      break;
    }
  }
  return 0;
  /*
  BlockHeader* b = (BlockHeader*)(_a - sizeof(BlockHeader));
  while (true) {
    b->isFree = 1;
    BlockHeader* buddy = getbuddy(b);
    int index = getIndex(b->block_size);
    if (index == FreeList.size()-1) {
      FreeList[index].insert(b);
      break;
    }
    
    if ((buddy->isFree) && (b->block_size == buddy->block_size)) {
      FreeList[index].remove(buddy);
      b = merge(b, buddy);
    } else {
      FreeList[index].insert(b);
      break;
    }
  }
  return 0;*/
}

BlockHeader* BuddyAllocator::getbuddy(BlockHeader* b) {
  // buddy address = ((blockaddress - start) XOR blockSize) + start
  int offset = (int)((char*)b - start);
  int buddy_offset = offset^b->block_size;
  char* buddy = (buddy_offset + start);
  return (BlockHeader*)buddy;
  // return (BlockHeader*)(((int)((char*)b - start)^b->block_size) + start);
}


void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  for (int i=0; i<FreeList.size(); i++){
    cout << "[" << i <<"] (" << ((1<<i) * basic_block_size) << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != (1<<i) * basic_block_size){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;  
  }
}

