#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
using namespace std;

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
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  /*
    1. Calculate smallest block size (sbs) needed for provided length
    2. Use FreeList to find available block of sbs or larger
    3. If block is larger than sbs, call split function until we reach sbs
    4. Once sbs blocks are available on the FreeList, remove one from the FreeList
    5. Shift the address down sizeof(BlockHeader) bytes and return it
  */

  
  return new char [_length];
}

int BuddyAllocator::free(char* _a) {
  /* Same here! */
  delete _a;
  return 0;
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

