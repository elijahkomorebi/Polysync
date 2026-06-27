#include <cJSON.h>
#include "polysync/transform/require.h"
#include "polysync/scan.h"
#include "polysync/string.h"
#include "polysync/memory.h"

string *polysync_transform_require(PolysyncTransformContext *transform_context, cstring *source_file_contents) {
    PolysyncStringBuilder *string_builder = polysync_string_builder_create();
    PolysyncScanner *scanner = polysync_scanner_create(source_file_contents);
    cstring *copy_start = source_file_contents;

    while (!polysync_scanner_is_at_end(scanner)) {
        if (polysync_scanner_match_string(scanner, "require")) {
            polysync_scanner_skip_whitespace(scanner);

            if (polysync_scanner_match_character(scanner, '(')) {
                polysync_scanner_skip_whitespace(scanner);

                string quote = polysync_scanner_peek(scanner);

                if (quote == '"' || quote == '\'') {
                    polysync_scanner_advance(scanner);

                    cstring *require_path_start = scanner->current;

                    while (!polysync_scanner_is_at_end(scanner) && polysync_scanner_peek(scanner) != quote) {
                        polysync_scanner_advance(scanner);
                    }

                    polysync_scanner_match_character(scanner, quote);

                    cstring *require_path_end = scanner->current - 1;
                    u32 require_path_length = require_path_end - require_path_start;
                    string *require_path = polysync_string_copy_dynamic(require_path_start, require_path_length);
                    string *alias_require_path = polysync_transform_path_alias(transform_context, require_path);
                    string *tree_require_path = polysync_transform_path_tree(transform_context, alias_require_path);
                    string *module_require_path = polysync_transform_path_module(tree_require_path);

                    polysync_string_builder_append_dynamic(string_builder, copy_start, require_path_start - copy_start - 1);
                    polysync_string_builder_append(string_builder, module_require_path);
                    polysync_memory_free(module_require_path);
                    polysync_memory_free(tree_require_path);
                    polysync_memory_free(alias_require_path);
                    polysync_memory_free(require_path);

                    copy_start = require_path_end + 1;
                }
            }
        } else {
            polysync_scanner_advance(scanner);
        }
    }

    polysync_string_builder_append_dynamic(string_builder, copy_start, scanner->end - copy_start);

    string *transformed_file_contents = polysync_string_builder_get(string_builder);

    polysync_scanner_destroy(scanner);
    polysync_string_builder_destroy(string_builder);

    return transformed_file_contents;
}
