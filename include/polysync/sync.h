#ifndef POLYSYNC_SYNC_H
#define POLYSYNC_SYNC_H

#include "polysync/types.h"

typedef enum PolysyncScriptType {
    POLYSYNC_SCRIPT_TYPE_SERVER,
    POLYSYNC_SCRIPT_TYPE_CLIENT,
    POLYSYNC_SCRIPT_TYPE_MODULE
} PolysyncScriptType;

void polysync_sync(void);
PolysyncScriptType polysync_sync_get_script_type(cstring *file_path);
string *polysync_sync_strip_suffix(cstring *file_path);

#endif
