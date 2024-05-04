/* Tests multiple pages of sbrk'd data. */

#include <string.h>
#include "tests/lib.h"
#include "tests/main.h"

#define LARGE_AMOUNT 4097

void test_main(void) {
  void* heap = sbrk(LARGE_AMOUNT);
  memset(heap, 162, LARGE_AMOUNT);
  printf("increment in test: %d\n", 4097);
  printf("negated increment in test: %d\n", -4097);

  void* heap2 = sbrk(-1 * LARGE_AMOUNT);
  // printf("before: %d\n", heap);
  // printf("after: %d\n", sbrk(0));
  
}

int main(int argc UNUSED, char* argv[] UNUSED) {
  test_name = "sbrk-large";
  msg("begin");
  test_main();
  msg("end");
  return 0;
}
