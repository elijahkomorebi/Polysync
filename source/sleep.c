#include "polysync/sleep.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void polysync_sleep(f32 miliseconds) {
#ifdef _WIN32
    Sleep(miliseconds);
#else
    sleep(miliseconds / 1000);
#endif
}
