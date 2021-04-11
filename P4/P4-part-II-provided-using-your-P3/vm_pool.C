/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {

#ifdef P4partII // You are working on P4 part II 
    Console::puts("Constructed a fake VMPool object that does nothing\n");
#else // you are now working on P4 part III
    // replace the assertion with your constructor code
    
    // Taking care of parameters
    base_address = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;

    // None because allocate isn't called yet
    mem_region_count = 0;

    mem_region_limit = Machine::PAGE_SIZE / sizeof(mem_region);

    // Put mem_regions in a frame of frame_pool
    mem_regions = (mem_region*)((frame_pool->get_frames(1)) * Machine::PAGE_SIZE);

    // Adding newly made VM pool to vm_pools array in page table
    page_table->register_pool(this);

    Console::puts("Constructed VMPool object.\n");
#endif
}

unsigned long VMPool::allocate(unsigned long _size) {
    // You need to implement this for P4 part III
    assert(false);
    } else{
        unsigned long prev_index = mem_region_count - 1;

        // Need to calculate from previous mem region since not base address
        unsigned long built_addr = mem_regions[prev_index].address + mem_regions[prev_index].size;
        logical_addr = built_addr;
    }

    // Assign the correct address and given sign to the correct mem_region in the array
    mem_regions[mem_region_count].address = logical_addr;
    mem_regions[mem_region_count].size = _size;

    // Increment for next allocate (or release)
    mem_region_count++;

    // Print before returning so it still prints
    Console::puts("Allocated region of memory.\n");

    return logical_addr;
}

void VMPool::release(unsigned long _start_address) {
    // You need to implement this for P4 part III
    assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    Console::puts("Checking legitimacy of given address: ");
    Console::putui(_address);
    Console::puts("\n");

    for(unsigned long i = 0; i < mem_region_count; i++){
        unsigned long range_start = mem_regions[i].address;
        unsigned long range_end = mem_regions[i].address + mem_regions[i].size;
        // Within given region of memory
        if((_address >= range_start) && (_address < range_end)){
            return true;
        }
    }

    return false;
}

