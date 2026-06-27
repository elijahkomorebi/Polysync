#include <stdlib.h>
#include "polysync/exit.h"

void polysync_exit(i32 status_code) {
    exit(status_code);
}
