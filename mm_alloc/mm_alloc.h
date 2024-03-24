/*
 * mm_alloc.h
 *
 * Exports a clone of the interface documented in "man 3 malloc".
 */

#pragma once

#ifndef _malloc_H_
#define _malloc_H_
#include <stdbool.h>

#include <stdlib.h>
struct metadata { 
  size_t size;
  struct metadata* prev;
  struct metadata* next;
  bool free;
};
void* mm_malloc(size_t size);
void* mm_realloc(void* ptr, size_t size);
void mm_free(void* ptr);

#endif
