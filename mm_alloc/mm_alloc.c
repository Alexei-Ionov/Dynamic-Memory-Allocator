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
  int inc = 0;
  struct metadata *curr_metadata = NULL;
  while ((curr_ptr + inc) < seg_break) { 
    curr_metadata = (struct metadata*)curr_ptr;
    if (curr_metadata->free && curr_metadata->size >= size) { 
      //if we can split it into two nodes
      leftover = curr_metadata->size - (size + METADATA_SIZE);
      if (leftover >= 0) { 
        //create new metadata node
        struct metadata new_metadata;
        new_metadata.size = leftover;
        new_metadata.prev = curr_metadata;
        new_metadata.next = curr_metadata->next;
        new_metadata.free = true;
        void* new_addr = (void*)curr_metadata + METADATA_SIZE + curr_metadata->size;
        //add node into memory
        memcpy(new_addr, &new_metadata, METADATA_SIZE);
        //set all data to zero 
        memset(new_addr + METADATA_SIZE, 0, leftover);

        //change pointers for the current and next nodes
        curr_metadata->next->prev = new_addr;
        curr_metadata->next = new_addr;
        memset((void*)curr_metadata + METADATA_SIZE, 0, size); //zero out the block taking into account the new additionoal block
      } else { 
        memset((void*)curr_metadata + METADATA_SIZE, 0, curr_metadata->size); //zero out the entire new block to be used
      }
      curr_metadata->size = size;
      curr_metadata->free = false;
      return (void*)curr_metadata + METADATA_SIZE;
    }
    inc += (METADATA_SIZE + curr_metadata->size);
  } 
  void *addr = sbrk(size + METADATA_SIZE);
  if (addr == (void*)-1) { 
    return NULL;
  }
  struct metadata new_metadata;
  new_metadata.size = size;
  new_metadata.prev = (curr_metadata == NULL) ? NULL : curr_metadata; //if its the first node then prev is NULL
  new_metadata.next = NULL;
  new_metadata.free = false;

  //add node into memory
  memcpy(addr, &new_metadata, METADATA_SIZE);
  //set all data to zero 
  memset(addr + METADATA_SIZE, 0, size);
 

  return addr + METADATA_SIZE;
}

void* mm_realloc(void* ptr, size_t size) {
  if (ptr == NULL) { 
    return mm_malloc(size);
  }
  if (size == 0) { 
    mm_free(ptr);
    return NULL;
  }

  void *metadata_addr = ptr - METADATA_SIZE;
  struct metadata *m = (struct metadata*)metadata_addr;
  
  //set it equal to free temporarily just for mm_alloc
  m->free = true;
  void *new_addr = mm_malloc(size);
  if (new_addr == NULL) {
    m->free = false; 
    return NULL;
  }
  //if smaller block, zero out the rest
  if (m->size >= size) {
    memset(new_addr + size, 0, m->size - size);
  }
  //in the case where the same block gets used (such as in the case where smaller size reallocation)
  if (new_addr == ptr) { 
    return new_addr;
  }
  //otherwise, we found a new block. 
  //copy over contents of previous block to new block
  memcpy(new_addr, ptr, size);
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
  
  //can't free memory that's already been freed!
  if (m->free) { 
    return;
  }

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
  


