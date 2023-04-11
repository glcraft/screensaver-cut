#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

void cpp_init(int32_t width, int32_t height);

void cpp_update();

void cpp_draw();

void cpp_terminate();

#ifdef __cplusplus
}
#endif