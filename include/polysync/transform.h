#ifndef POLYSYNC_TRANSFORM_H
#define POLYSYNC_TRANSFORM_H

#include <cJSON.h>
#include "polysync/tree.h"

typedef struct PolysyncTransformContext {
    cJSON *project_object;
    cJSON *luaurc_object;
    PolysyncTree *tree;
} PolysyncTransformContext;

PolysyncTransformContext *polysync_transform_context_create(void);
void polysync_transform_context_destroy(PolysyncTransformContext *transform_context);
void polysync_transform(PolysyncTransformContext *transform_context, cstring *source_file_path, cstring *build_file_path);
string *polysync_transform_path_tree(PolysyncTransformContext *transform_context, cstring *path);
string *polysync_transform_path_module(cstring *path);
string *polysync_transform_path_alias(PolysyncTransformContext *transform_context, cstring *path);

#endif
