#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);
struct metadata { 
  size_t size;
  struct metadata* prev;
  struct metadata* next;
  bool free;
};
#define METADATA_SIZE sizeof(struct metadata)

static void* try_dlsym(void* handle, const char* symbol) {
  char* error;
  void* function = dlsym(handle, symbol);
  if ((error = dlerror())) {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }
  return function;
}

static void load_alloc_functions() {
  void* handle = dlopen("hw3lib.so", RTLD_NOW);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  mm_malloc = try_dlsym(handle, "mm_malloc");
  mm_realloc = try_dlsym(handle, "mm_realloc");
  mm_free = try_dlsym(handle, "mm_free");
}
static void test_free() { 
  char* block1 = mm_malloc(1);
  char* block2 = mm_malloc(2);
  char* block3 = mm_malloc(3);
  char* block4 = mm_malloc(4);
  assert(block1 != NULL);
  assert(block2 != NULL);
  assert(block3 != NULL);
  assert(block4 != NULL);
  
  mm_free((void*) block1);
  mm_free((void*) block2);
  mm_free((void*) block3);
  mm_free((void*) block4);
  // mm_free((void*) small1);
  // mm_free((void*) small2);






  
  // for (int i = 0; i < 4095; i++) { 
  //   printf("i %d\n", i);
  //   printf("beginning[i] %d\n", beginnning[i]);
  // }
  
  // int* data2 = mm_malloc(200);
  // assert(mdata2 != NULL);
  // mm_free((void*) data);
  // mm_free((void *) data2);
  // mm_free(data2);

}
// static void test_realloc() { 
//   int* data = mm_malloc(100);
//   assert(data != NULL);
//   int* data2 = mm_malloc(200);
//   assert(data2 != NULL);
//   assert(mm_realloc(data, 50) == (void*)1);
//   // mm_free(data2);
// }
int main() {
  load_alloc_functions();
  // test_realloc();
  test_free();
  puts("malloc test successful!");
}
