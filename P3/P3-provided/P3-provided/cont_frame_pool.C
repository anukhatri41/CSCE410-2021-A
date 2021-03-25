/*
 File: ContFramePool.C
 
 Author: Anu Khatri	
 Date  : February 12,2021
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:

 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we release a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
// Linked list of all ContFramePool
PoolNode* ContFramePool::pool_list;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    //assert(_n_frames <= FRAME_SIZE);
    
    base_frame_no = _base_frame_no;
    n_frames = _n_frames;
    n_free_frames = _n_frames;
    info_frame_no = _info_frame_no;
    n_info_frames = _n_info_frames;
    
    if(n_info_frames == 0){
	n_info_frames = needed_info_frames(n_frames);
	
    }

    // Mark all bits in the bitmap
    for(int i = 0; i < n_frames; i++){
        bitmap[i] = FRAME_FREE;
    }

    // Mark the frame(s) as being used if it is being used
    bitmap[info_frame_no] = FRAME_HEAD; 
    n_free_frames--; 
    for(int i = info_frame_no+1; i < info_frame_no + n_info_frames; i++){
            bitmap[i] = FRAME_ALLOCATED;
            n_free_frames--;
    }

    if(pool_list == NULL){
        pool_list->pool = this;
    } else{
        pool_list->next = this;
        pool_list->prev = pool_list->pool;
        
    }

    Console::puts("Frame Pool Initialized\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // Make sure there are still free frames available
    assert(n_free_frames >= _n_frames);
   
    // Keeping track of frame number when making head/allocated once range is found
    unsigned long frame_no = base_frame_no;
    bool found = false; // determines if full length of _n_frames found yet

    // index in bitmap
    unsigned long i = 0;
    unsigned long frame_index = 0;
    
    // Finding the range
    while(!found){
        if(i > n_frames){
            // Extended beyond the pool range
            // Return 0 to indicate no feasible range found
            frame_no = 0;
            return frame_no;
        } else {
            // Traversing until hits a free or end of pool
            while(bitmap[i] != FRAME_FREE && i < n_frames){ 
                i += 1;
            } 

            // Found first free frame - update frame_no
            frame_no += i;
	    // Catches if the index+range is longer than the total number of frames
            if((i + _n_frames) < n_frames){
                // See if there is the full length of _n_frames after the i
                for(unsigned long j = i; j < (i + _n_frames); j++){
                    // Last frame of _n_frames
                    if(j == (i + _n_frames - 1)){
                        // If last frame is also free, then full length found
                        // Will update found (then end outer while loop)
                        if(bitmap[j] == FRAME_FREE){
                            frame_index = i;
                            found = true;
                            break;
                        }
                    } else{
                        if(bitmap[j] == FRAME_FREE){
                            continue;
                        } else{
                            // Frame was either head or allocated
                            // Start over what frame_no is (since i will take care of numbers above)
                            frame_no = base_frame_no;
			                i += j;
                            // exit for loop
                            break;
                        }
                    }
                }
            }
        }
    }
	
    // Range was found - need to now change those frames to head//allocated
    for(int j = frame_index; j < (frame_index + _n_frames); j++){
        // First frame - make HEAD 
        if(j == frame_index){
            bitmap[j] = FRAME_HEAD;
        } else{
            // Making rest of frames ALLOCATED 
            bitmap[j] = FRAME_ALLOCATED;
        }
        
        // Reducing num frames available in pool
        n_free_frames--;
    }
    // Give infinite loop/long loop at the Need to start over since head/allocated found step if adding the below return statement - but need this statement
    return frame_no;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // Mark all frames in the range as being used
    for(unsigned long i = _base_frame_no; i < _base_frame_no; i++){
        mark_inaccessible(i);
    }
}

// Help mark_inaccessible function
void ContFramePool::mark_inaccessible(unsigned long _frame_no){
    // Range check
    assert((_frame_no >= base_frame_no) && (_frame_no < base_frame_no + n_frames));

    unsigned long bitmap_index = _frame_no - base_frame_no;
    
    // Making sure not already head/allocated
    assert(bitmap[bitmap_index] != (FRAME_HEAD || FRAME_ALLOCATED));

    // Update bitmap
    bitmap[bitmap_index] == FRAME_ALLOCATED;
    n_free_frames--;
}

void ContFramePool::release_frames_help(unsigned long _first_frame_no){
    unsigned long bitmap_index = _first_frame_no - base_frame_no;
    assert(bitmap[bitmap_index] == FRAME_HEAD);
    if(bitmap[bitmap_index] == FRAME_FREE){
        Console::puts("Error: Frame being released is not used\n");
        assert(false);
    }

    while(bitmap[bitmap_index] != FRAME_FREE){
        bitmap[bitmap_index] = FRAME_FREE;
        n_free_frames++;
        bitmap_index++;
    }

}
void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    PoolNode* temp = pool_list;
    while(temp != NULL){
        if((_first_frame_no >= temp->pool->base_frame_no) && (_first_frame_no < (temp->pool->base_frame_no + temp->pool->n_frames))){
            temp->pool->release_frames_help(_first_frame_no);
        }
    }
}


// Done
unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    unsigned long n_bytes = FRAME_SIZE;
    unsigned long n_info_frames = (_n_frames / FRAME_SIZE) + (_n_frames % FRAME_SIZE > 0 ? 1 : 0);
    return n_info_frames;
}
