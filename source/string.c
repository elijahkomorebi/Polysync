#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "polysync/string.h"
#include "polysync/memory.h"

u32 polysync_string_length(cstring *string) {
    return strlen(string);
}

bool polysync_string_matches(cstring *string_a, cstring *string_b) {
    return strcmp(string_a, string_b) == 0;
}

bool polysync_string_matches_dynamic(cstring *string_a, cstring *string_b, u32 compare_length) {
    return strncmp(string_a, string_b, compare_length) == 0;
}

bool polysync_string_starts_with(cstring *string, cstring *prefix) {
    u32 prefix_length = strlen(prefix);

    return strncmp(string, prefix, prefix_length) == 0;
}

bool polysync_string_ends_with(cstring *string, cstring *suffix) {
    u32 string_length = strlen(string);
    u32 suffix_length = strlen(suffix);

    if (suffix_length > string_length) {
        return false;
    }

    return strcmp(string + string_length - suffix_length, suffix) == 0;
}

u32 polysync_string_character_count(cstring *string, cstring character) {
    u32 string_length = strlen(string);
    u32 character_count = 0;

    for (u32 character_index = 0; character_index < string_length; character_index++) {
        if (string[character_index] != character) {
            continue;
        }

        character_count++;
    }

    return character_count;
}

string *polysync_string_concatenate(cstring *first_string, ...) {
    va_list arguments;
    u32 total_string_length = strlen(first_string);

    va_start(arguments, first_string);

    while (true) {
        cstring *current_string = va_arg(arguments, cstring *);

        if (!current_string) {
            break;
        }

        total_string_length += strlen(current_string);
    }

    va_end(arguments);

    string *final_string = polysync_memory_allocate(sizeof(cstring) * total_string_length + 1);
    final_string[0] = 0;

    strcat(final_string, first_string);
    va_start(arguments, first_string);

    while (true) {
        cstring *current_string = va_arg(arguments, string *);

        if (!current_string) {
            break;
        }

        strcat(final_string, current_string);
    }

    va_end(arguments);

    return final_string;
}

string *polysync_string_copy(cstring *original_string) {
    u32 string_size = strlen(original_string);
    string *copy_string = polysync_memory_allocate(string_size + 1);
    copy_string[string_size] = 0;

    memcpy(copy_string, original_string, string_size);

    return copy_string;
}

string *polysync_string_copy_dynamic(cstring *original_string, u32 original_string_length) {
    string *copy_string = polysync_memory_allocate(original_string_length + 1);
    copy_string[original_string_length] = 0;

    memcpy(copy_string, original_string, original_string_length);

    return copy_string;
}

PolysyncStringBuilder *polysync_string_builder_create(void) {
    PolysyncStringBuilder *string_builder = polysync_memory_allocate(sizeof(PolysyncStringBuilder));
    string_builder->string = polysync_memory_allocate(sizeof(string) * 32);
    string_builder->string_length = 0;
    string_builder->string_capacity = 32;

    return string_builder;
}

void polysync_string_builder_destroy(PolysyncStringBuilder *string_builder) {
    polysync_memory_free(string_builder->string);
    polysync_memory_free(string_builder);
}

string *polysync_string_builder_get(PolysyncStringBuilder *string_builder) {
    return strdup(string_builder->string);
}

void polysync_string_builder_append(PolysyncStringBuilder *string_builder, cstring *append_string) {
    u32 append_string_length = strlen(append_string);

    polysync_string_builder_append_dynamic(string_builder, append_string, append_string_length);
}

void polysync_string_builder_append_dynamic(PolysyncStringBuilder *string_builder, cstring *append_string, u32 append_string_length) {
    while (string_builder->string_length + append_string_length > string_builder->string_capacity) {
        string_builder->string_capacity *= 2;
        string_builder->string = polysync_memory_reallocate(string_builder->string, sizeof(string) * string_builder->string_capacity);
    }

    memcpy(string_builder->string + string_builder->string_length, append_string, append_string_length);

    string_builder->string_length += append_string_length;
    string_builder->string[string_builder->string_length] = 0;
}

void polysync_string_builder_append_format(PolysyncStringBuilder *string_builder, cstring *format, ...) {
    va_list arguments;
    va_list copy;

    va_start(arguments, format);
    va_copy(copy, arguments);

    i32 format_length = vsnprintf(null, 0, format, copy);

    va_end(copy);

    if (format_length < 0) {
        va_end(arguments);

        return;
    }

    string *buffer = polysync_memory_allocate(format_length + 1);

    vsnprintf(buffer, format_length + 1, format, arguments);
    va_end(arguments);
    polysync_string_builder_append(string_builder, buffer);
    polysync_memory_free(buffer);
}
