#include "BuddyAllocator.h"
#include <iostream>

using namespace std;

int main() {
    BlockHeader* b1 = new BlockHeader(1);
    BlockHeader* b2 = new BlockHeader(2);
    BlockHeader* b3 = new BlockHeader(3);
    BlockHeader* b4 = new BlockHeader(4);
    
    LinkedList* list = new LinkedList();
    list->insert(b2);
    cout << list->_size << endl;
    // list.insert(b2);
    cout << b1->block_size << endl;
    cout << list->head->block_size << endl;
}