#include "polysync/cli.h"
#include "polysync/log.h"
#include "polysync/polysync.h"
#include "polysync/memory.h"
#include "polysync/string.h"
#include "polysync/error.h"
#include "polysync/setup.h"
#include "polysync/definitions.h"
#include "polysync/build.h"
#include "polysync/sync.h"

PolysyncCLIContext *polysync_cli_context_create(string **arguments, u8 argument_count) {
    PolysyncCLIContext *cli_context = polysync_memory_allocate(sizeof(PolysyncCLIContext));
    cli_context->arguments = arguments;
    cli_context->argument_count = argument_count;

    return cli_context;
}

void polysync_cli_context_destroy(PolysyncCLIContext *cli_context) {
    polysync_memory_free(cli_context);
}

void polysync_cli_context_run(PolysyncCLIContext *cli_context) {
    string *command = polysync_cli_context_get_command(cli_context);

    if (!command) {
        polysync_error("Invalid usage. Run 'polysync help' for more info");
    }

    if (polysync_string_matches(command, "help")) {
        polysync_cli_print_usage();
    } else if (polysync_string_matches(command, "version")) {
        polysync_cli_print_version();
    } else if (polysync_string_matches(command, "setup")) {
        polysync_setup();
        polysync_log_info("Successfully setup project");
    } else if (polysync_string_matches(command, "definitions")) {
        polysync_definitions_download();
        polysync_log_info("Successfully downloaded definitions");
    } else if (polysync_string_matches(command, "build")) {
        PolysyncTransformContext *transform_context = polysync_transform_context_create();

        polysync_build(transform_context, "source", "build");
        polysync_log_info("Successfully built project");
        polysync_transform_context_destroy(transform_context);
    } else if (polysync_string_matches(command, "sync")) {
        polysync_log_info("Syncing project");
        polysync_sync();
    } else {
        polysync_error("Unknown command '%s'", command);
    }
}

string *polysync_cli_context_get_command(PolysyncCLIContext *cli_context) {
    if (cli_context->argument_count < 2) {
        return null;
    }

    return cli_context->arguments[1];
}

void polysync_cli_print_usage(void) {
    printf(
        "Usage:\n"
        "    polysync <command> [options]\n"
        "Commands:\n"
        "    help        - Prints this help message\n"
        "    version     - Prints the current version\n"
        "    setup       - Initialize a new project\n"
        "    definitions - Download polytoria definitions\n"
        "    build       - Build the current project\n"
        "    sync        - Start syncing the current project\n"
    );
}

void polysync_cli_print_version(void) {
    printf("Polysync %s\n", POLYSYNC_VERSION);
}
