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
}

unsigned long VMPool::allocate(unsigned long _size) {
    // Can't exceed limit and can't allocate size of 0
    if(_size == 0 || mem_region_count >= mem_region_limit){
        return 0;
    }

    unsigned logical_addr;
    if(mem_region_count == 0){
        logical_addr = base_address;
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
    unsigned int index;
    
    // Find index of regions where _start_address is
    for(index = 0; index < mem_region_count; index++){
        if(mem_regions[index].address == _start_address){
            // Found which region _start_address is in
            break;
        }
    }

    unsigned int j;
    unsigned long address = _start_address;

    // Free page by page in region
    for(j = index; j < (mem_regions[index].size / Machine::PAGE_SIZE); j++){
        page_table->free_page(address);
        
        // Increment for next page to be freed
        address += Machine::PAGE_SIZE;
    }
    // Region should be cleared, so decrement number of regions with the data
    mem_region_count--;
    
    // Fix up mem_regions array so there are no holes in the middle
    for(j = index; j < mem_region_count; j++){
        mem_regions[j] = mem_regions[j + 1];
    }
    
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
            Console::puts("Legitimate address. \n");
            return true;
        }
    }
    Console::puts("Illegitimate address. \n");
    return false;
}

