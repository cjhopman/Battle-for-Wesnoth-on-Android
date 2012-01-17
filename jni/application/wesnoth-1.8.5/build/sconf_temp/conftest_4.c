

#include <assert.h>

#ifdef __cplusplus
extern "C"
#endif
char sendfile();

int main() {
#if defined (__stub_sendfile) || defined (__stub___sendfile)
  fail fail fail
#else
  sendfile();
#endif

  return 0;
}
