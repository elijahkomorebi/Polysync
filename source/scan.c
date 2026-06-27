#include <string.h>
#include <ctype.h>
#include "polysync/scan.h"
#include "polysync/memory.h"
#include "polysync/string.h"

PolysyncScanner *polysync_scanner_create(cstring *source) {
    PolysyncScanner *scanner = polysync_memory_allocate(sizeof(PolysyncScanner));
    scanner->start = source;
    scanner->current = source;
    scanner->end = source + polysync_string_length(source);

    return scanner;
}

void polysync_scanner_destroy(PolysyncScanner *scanner) {
    polysync_memory_free(scanner);
}

bool polysync_scanner_is_at_end(PolysyncScanner *scanner) {
    return scanner->current >= scanner->end;
}

string polysync_scanner_peek(PolysyncScanner *scanner) {
    if (polysync_scanner_is_at_end(scanner)) {
        return 0;
    }

    return *scanner->current;
}

string polysync_scanner_peek_next(PolysyncScanner *scanner) {
    if (scanner->current + 1 >= scanner->end) {
        return 0;
    }

    return scanner->current[1];
}

string polysync_scanner_advance(PolysyncScanner *scanner) {
    if (polysync_scanner_is_at_end(scanner)) {
        return 0;
    }

    return *scanner->current++;
}

bool polysync_scanner_match_character(PolysyncScanner *scanner, string character) {
    if (polysync_scanner_peek(scanner) != character) {
        return false;
    }

    scanner->current++;

    return true;
}

bool polysync_scanner_match_string(PolysyncScanner *scanner, string *string) {
    u32 string_length = polysync_string_length(string);

    if (scanner->end - scanner->current < string_length) {
        return false;
    }

    if (strncmp(scanner->current, string, string_length)) {
        return false;
    }

    scanner->current += string_length;

    return true;
}

void polysync_scanner_skip_whitespace(PolysyncScanner *scanner) {
    while (!polysync_scanner_is_at_end(scanner)) {
        string character = polysync_scanner_peek(scanner);

        if (!isspace(character)) {
            break;
        }

        scanner->current++;
    }
}
