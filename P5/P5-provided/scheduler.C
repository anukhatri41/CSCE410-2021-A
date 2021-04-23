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

    // Reduce count because dequeued
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
  // Dequeue from front of queue until reached desired thread.
  for(int i = 1; i <= readyQueueCount; i++){
    Thread* removed_thread = readyQueue.dequeue();

    int removed_thread_ID = removed_thread->ThreadId();
    int _thread_ID = _thread->ThreadId();

    if(removed_thread_ID == _thread_ID){
      // If successfully dequeued correct thread, reduce count 
      readyQueueCount--;
    } else{
      // Add back to the end of queue if it wasn't the thread we were looking for (due to FIFO)
      readyQueue.enqueue(removed_thread);
    }
  }
}
