#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

void easytest(BuddyAllocator* ba){
  // be creative here
  // know what to expect after every allocation/deallocation cycle

  // here are a few examples
  ba->printlist();
  // allocating a byte
  char * mem = ba->alloc (1);
  // now print again, how should the list look now
  ba->printlist ();

  ba->free (mem); // give back the memory you just allocated
  ba->printlist(); // shouldn't the list now look like as in the beginning

}

int main(int argc, char ** argv) {
  // 512 kb = 524288b, 64mb = 67108864
  int basic_block_size = 128, memory_length = 524288;
  
  int opt;
  while((opt = getopt(argc, argv, "b:s:")) != -1) { 
    switch(opt) { 
      case 'b':
        if (optarg != nullptr) {
          basic_block_size = atoi(optarg);
        }
        break;
      case 's':
        if (optarg != nullptr) {
          memory_length = atoi(optarg);
        }
        break;
      default:
        cout << "Incorrect option\n" << endl;
        return 1;
    } 
  }

  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);


  // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
  /*
  easytest (allocator);
  */
  
  // stress-test the memory manager, do this only after you are done with small test cases
  allocator->printlist();
  char* addr = allocator->alloc(1);
  allocator->printlist();
  allocator->free(addr);
  allocator->printlist();


  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test. 
  
  // destroy memory manager
  delete allocator;
}
