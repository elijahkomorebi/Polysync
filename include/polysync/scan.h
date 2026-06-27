#ifndef POLYSYNC_SCAN_H
#define POLYSYNC_SCAN_H

#include "polysync/types.h"

typedef struct PolysyncScanner {
    cstring *start;
    cstring *current;
    cstring *end;
} PolysyncScanner;

PolysyncScanner *polysync_scanner_create(cstring *source);
void polysync_scanner_destroy(PolysyncScanner *scanner);
bool polysync_scanner_is_at_end(PolysyncScanner *scanner);
string polysync_scanner_peek(PolysyncScanner *scanner);
string polysync_scanner_peek_next(PolysyncScanner *scanner);
string polysync_scanner_advance(PolysyncScanner *scanner);
bool polysync_scanner_match_character(PolysyncScanner *scanner, string character);
bool polysync_scanner_match_string(PolysyncScanner *scanner, string *string);
void polysync_scanner_skip_whitespace(PolysyncScanner *scanner);

#endif
