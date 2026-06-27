#ifndef POLYSYNC_STRING_H
#define POLYSYNC_STRING_H

#include "polysync/types.h"

typedef struct PolysyncStringBuilder {
    string *string;
    u32 string_length;
    u32 string_capacity;
} PolysyncStringBuilder;

u32 polysync_string_length(cstring *string);
bool polysync_string_matches(cstring *string_a, cstring *string_b);
bool polysync_string_matches_dynamic(cstring *string_a, cstring *string_b, u32 compare_length);
bool polysync_string_starts_with(cstring *string, cstring *prefix);
bool polysync_string_ends_with(cstring *string, cstring *suffix);
u32 polysync_string_character_count(cstring *string, cstring character);
string *polysync_string_concatenate(cstring *first_string, ...);
string *polysync_string_copy(cstring *original_string);
string *polysync_string_copy_dynamic(cstring *original_string, u32 original_string_length);
PolysyncStringBuilder *polysync_string_builder_create(void);
void polysync_string_builder_destroy(PolysyncStringBuilder *string_builder);
string *polysync_string_builder_get(PolysyncStringBuilder *string_builder);
void polysync_string_builder_append(PolysyncStringBuilder *string_builder, cstring *append_string);
void polysync_string_builder_append_dynamic(PolysyncStringBuilder *string_builder, cstring *append_string, u32 append_string_length);
void polysync_string_builder_append_format(PolysyncStringBuilder *string_builder, cstring *format, ...);

#endif
