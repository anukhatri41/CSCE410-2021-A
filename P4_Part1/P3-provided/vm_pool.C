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
    assert(false);
    Console::puts("Constructed VMPool object.\n");
#endif
}

unsigned long VMPool::allocate(unsigned long _size) {
    // You need to implement this for P4 part III
    assert(false);
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address) {
    // You need to implement this for P4 part III
    assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
#ifdef P4partII
    // You are working on P4 part II, so we give you this
    // fake implementation so that you can test your functionality
    // in page_table.C that checks addresses
    Console::puts("Fake checking ... always returning true.\n");
    return true;
else
    // You need to implement this for P4 part III
    assert(false);
#endif
}

