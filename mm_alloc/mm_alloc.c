/*
 * mm_alloc.c
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define METADATA_SIZE sizeof(struct metadata)
#include <assert.h>

void* heap_start = NULL;

void* mm_malloc(size_t size) {
  if (size == 0) { 
    return NULL;
  }

  if (heap_start == NULL) { 
    heap_start = sbrk(0);
  }
  
  int leftover;
  void* curr_ptr = heap_start;
  void* seg_break = sbrk(0);
  unsigned int inc = 0;
  struct metadata *curr_metadata = NULL;
  // if (curr_ptr != seg_break) {
  //   primntf("curr_ptr %p\n", curr_ptr);
  //   printf("seg break %p\n", seg_break);
  // }
  // printf("curr_ptr %p\n", curr_ptr);
  // printf("seg break %p\n", seg_break);
  
  while ((curr_ptr + inc) < seg_break) { 
    curr_metadata = (struct metadata*)curr_ptr;
    if (curr_metadata->free && curr_metadata->size >= size) {

      
      //if we can split it into two nodes
      leftover = curr_metadata->size - (size + METADATA_SIZE);
      //zero out the entire block first, regardless of whether we are adding in a new one or not
    
      memset(curr_metadata + METADATA_SIZE, 0, curr_metadata->size);
     
      if (leftover >= 0) { 
        //create new metadata node
        struct metadata new_metadata;
        new_metadata.size = leftover;
        new_metadata.prev = curr_metadata;
        new_metadata.next = curr_metadata->next;
        new_metadata.free = true;
        void* new_addr = (void*)curr_metadata + METADATA_SIZE + size;
        //add node into memory
        memcpy(new_addr, &new_metadata, METADATA_SIZE);
      
        // //set all data to zero, redundant
        // memset(new_addr + METADATA_SIZE, 0, leftover);

        //change pointers for the current and next nodes
        if (curr_metadata->next != NULL) {
          curr_metadata->next->prev = new_addr;
        }
       
        curr_metadata->next = new_addr;
    
      } 
   
      curr_metadata->size = size;
      curr_metadata->free = false;
      return (void*)curr_metadata + METADATA_SIZE;
    }
    inc += (METADATA_SIZE + curr_metadata->size);
  } 
  void *new_metadata_addr = sbrk(size + METADATA_SIZE);
  if (new_metadata_addr == (void*)-1) { 
    return NULL;
  }

  struct metadata new_metadata;
  new_metadata.size = size;
  new_metadata.prev = NULL; //gets overwritten if this isn't tjhe first block
  if (curr_metadata != NULL) { //if this isnt the first block to be added
    curr_metadata->next = new_metadata_addr;
    new_metadata.prev = curr_metadata;
  }
  new_metadata.next = NULL;
  new_metadata.free = false;

  //add node into memory
  memcpy(new_metadata_addr, &new_metadata, METADATA_SIZE);
  //set all data to zero 
  memset(new_metadata_addr + METADATA_SIZE, 0, size);
 

  return new_metadata_addr + METADATA_SIZE;
}

void* mm_realloc(void* ptr, size_t size) {
  if (ptr == NULL) { 
    return mm_malloc(size);
  }
  if (size == 0) { 
    mm_free(ptr);
    return NULL;
  }
  /*
  create a temporoary buffer to hold the value at the previous block. reason why, is that in the case where we re-use the same block, 
  within malloc, we actually zero out all the contents of the block so we technically lose all that data. 
  */
  char buf[size];
  memcpy(&buf, ptr, size);
  void *metadata_addr = ptr - METADATA_SIZE;
  struct metadata *prev_metadata = (struct metadata*)metadata_addr;
  
  //set it equal to free temporarily just for mm_alloc
  prev_metadata->free = true;
  void *new_addr = mm_malloc(size);
  if (new_addr == NULL) {
    prev_metadata->free = false; 
    return NULL;
  }
    //in the case where the same block gets used (such as in the case where smaller size reallocation)
  if (new_addr == ptr) { 
    //if requested size is smaller 
    prev_metadata->free = false;
    //unneccsarry cleaning
    // if (size < prev_metadata->size) {
    //   memset(new_addr + size, 0, prev_metadata->size - size);
    // }
    memcpy(new_addr, &buf, size);
    return new_addr;
  }
  // void *new_metadata_addr = new_addr - METADATA_SIZE;
  // struct metadata *new_metadata = (struct metadata*)new_metadata_addr;

  //otherwise, we found a new block or created a new block!!!! 
  //copy over contents of previous block to new block
  if (size < prev_metadata->size) {
    memcpy(new_addr, ptr, size); // we copy only up to the min(size, prev block size) number of bytes
  } else { 
    memcpy(new_addr, ptr, prev_metadata->size); //otherwise if the requested size is bigger, we should copy over the entire thing. 
  }
  
  // if (size < new_metadata->size) {
  //   //zero out the leftover bytes since the new block is actually bigger than what we need 
  //   memset(new_addr + size, 0, new_metadata->size - size);
  // } 
  mm_free(ptr);
  return new_addr;
}

void free_block(struct metadata *leftmost, struct metadata *rightmost, size_t total_size_to_free) { 
  leftmost->size = leftmost->size + total_size_to_free;
  leftmost->free = true;
  leftmost->next = rightmost->next; //we are freeing all struct right of the leftmost struct up to and including the rightmost struct
  memset((void*)leftmost + METADATA_SIZE, 0, leftmost->size);
}
void mm_free(void* ptr) {
  if (ptr == NULL) { 
    return;
  }
  void *metadata_addr = ptr - METADATA_SIZE;
  struct metadata *m = (struct metadata*)metadata_addr;

  struct metadata* leftmost = m;
  struct metadata* rightmost = m;
  size_t total_size_to_free = 0;
  bool left = m->prev != NULL && m->prev->free;
  bool right = m->next != NULL && m->next->free;
  if (left) { 
    total_size_to_free += (METADATA_SIZE + m->size);
    leftmost = m->prev;
  }
  if (right) { 
    rightmost = m->next;
    total_size_to_free += (METADATA_SIZE + rightmost->size);
  }
  free_block(leftmost, rightmost, total_size_to_free);
  
}
  


