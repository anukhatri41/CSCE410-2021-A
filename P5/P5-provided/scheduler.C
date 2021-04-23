/*
 File: scheduler.C
 
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

#include "scheduler.H"
#include "thread.H"
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
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  readyQueueCount = 0;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  if(readyQueueCount > 0){
    // There is a thread to dequeue
    Thread* t = readyQueue.dequeue();
    Thread::dispatch_to(t);
    readyQueueCount--;
  }
}

void Scheduler::resume(Thread * _thread) {
  readyQueue.enqueue(_thread);
  readyQueueCount++;
}

void Scheduler::add(Thread * _thread) {
  this->resume(_thread);
}

void Scheduler::terminate(Thread * _thread) {
  assert(false);
}
