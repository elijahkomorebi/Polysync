#ifndef POLYSYNC_CLI_H
#define POLYSYNC_CLI_H

#include "polysync/types.h"

typedef struct PolysyncCLIContext {
    string **arguments;
    u8 argument_count;
} PolysyncCLIContext;

PolysyncCLIContext *polysync_cli_context_create(string **arguments, u8 argument_count);
void polysync_cli_context_destroy(PolysyncCLIContext *cli_context);
void polysync_cli_context_run(PolysyncCLIContext *cli_context);
string *polysync_cli_context_get_command(PolysyncCLIContext *cli_context);
void polysync_cli_print_usage(void);
void polysync_cli_print_version(void);

#endif
