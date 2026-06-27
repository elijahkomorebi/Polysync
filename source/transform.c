#include <string.h>
#include "polysync/transform.h"
#include "polysync/memory.h"
#include "polysync/io.h"
#include "polysync/error.h"
#include "polysync/string.h"
#include "polysync/transform/require.h"

PolysyncTransformContext *polysync_transform_context_create(void) {
    string *project_file_contents = polysync_io_file_read("polysync.json");
    string *luaurc_file_contents = polysync_io_file_read(".luaurc");

    if (!project_file_contents) {
        polysync_error("Failed to read polysync.json");
    }

    if (!luaurc_file_contents) {
        polysync_error("Failed to read .luaurc");
    }

    cJSON *project_object = cJSON_Parse(project_file_contents);
    cJSON *luaurc_object = cJSON_Parse(luaurc_file_contents);

    if (!project_object) {
        polysync_error("Failed to parse polysync.json");
    }

    if (!luaurc_object) {
        polysync_error("Failed to parse .luaurc");
    }

    polysync_memory_free(project_file_contents);
    polysync_memory_free(luaurc_file_contents);

    PolysyncTransformContext *transform_context = polysync_memory_allocate(sizeof(PolysyncTransformContext));
    transform_context->project_object = project_object;
    transform_context->luaurc_object = luaurc_object;
    transform_context->tree = polysync_tree_create();

    cJSON *tree_object = cJSON_GetObjectItem(project_object, "tree");

    if (!tree_object || !cJSON_IsObject(tree_object)) {
        polysync_error("Failed to parse polysync.json tree");
    }

    polysync_tree_load(transform_context->tree, tree_object, NULL);

    return transform_context;
}

void polysync_transform_context_destroy(PolysyncTransformContext *transform_context) {
    cJSON_Delete(transform_context->project_object);
    cJSON_Delete(transform_context->luaurc_object);
    polysync_tree_destroy(transform_context->tree);
    polysync_memory_free(transform_context);
}

void polysync_transform(PolysyncTransformContext *transform_context, cstring *source_file_path, cstring *build_file_path) {
    string *source_file_contents = polysync_io_file_read(source_file_path);

    if (!source_file_contents) {
        polysync_error("Failed to transform '%s'", source_file_path);
    }

    string *source_file_transformed_contents = polysync_transform_require(transform_context, source_file_contents);

    if (!polysync_io_file_write(build_file_path, source_file_transformed_contents)) {
        polysync_error("Failed to write transform '%s'", source_file_path);
    }

    polysync_memory_free(source_file_contents);
    polysync_memory_free(source_file_transformed_contents);
}

string *polysync_transform_path_tree(PolysyncTransformContext *transform_context, cstring *path) {
    PolysyncTreeEntry *tree_entry = polysync_tree_find(transform_context->tree, path);

    if (!tree_entry) {
        polysync_error("Failed to get tree entry for '%s'", path);
    }

    u32 source_path_length = polysync_string_length(tree_entry->source_path);
    cstring *remaining_path = path + source_path_length;

    if (*remaining_path == '/') {
        remaining_path++;
    }

    string *tree_path = polysync_string_concatenate(tree_entry->target_path, "/", remaining_path, null);

    return tree_path;
}

string *polysync_transform_path_module(cstring *path) {
    string *module_path = polysync_string_copy(path);

    for (string *current = module_path; *current; current++) {
        if (*current != '/') {
            continue;
        }

        *current = '.';
    }

    return module_path;
}

string *polysync_transform_path_alias(PolysyncTransformContext *transform_context, cstring *path) {
    if (*path != '@') {
        return polysync_string_copy(path);
    }

    cJSON *aliases_object = cJSON_GetObjectItem(transform_context->luaurc_object, "aliases");

    if (!aliases_object) {
        polysync_error("Failed to get aliases");
    }

    path++;
    cstring *slash = strchr(path, '/');

    if (!slash) {
        return polysync_string_copy(path);
    }

    u32 alias_length = slash - path;
    string *alias = polysync_string_copy_dynamic(path, alias_length);
    cJSON *alias_map_object = cJSON_GetObjectItem(aliases_object, alias);

    if (!alias_map_object || !cJSON_IsString(alias_map_object)) {
        polysync_error("Failed to get alias '%s'", alias);
    }

    string *alias_path = polysync_string_concatenate(alias_map_object->valuestring, "/", slash + 1, null);

    polysync_memory_free(alias);

    return alias_path;
}
