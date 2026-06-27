#include "polysync/cli.h"
#include "polysync/exit.h"

int main(int argument_count, char **arguments) {
    PolysyncCLIContext *cli_context = polysync_cli_context_create(arguments, argument_count);

    polysync_cli_context_run(cli_context);
    polysync_cli_context_destroy(cli_context);

    return POLYSYNC_EXIT_SUCCESS;
}
