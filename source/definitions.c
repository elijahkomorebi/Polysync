#include "polysync/definitions.h"
#include "polysync/http.h"
#include "polysync/error.h"

static cstring *DEFINITIONS_URL = "https://raw.githubusercontent.com/elijahkomorebi/Polysync-Definitions/refs/heads/master/definitions.d.luau";
static string *DEFINITIONS_FILE_PATH = "types/definitions.d.luau";

void polysync_definitions_download(void) {
    if (!polysync_http_download_file(DEFINITIONS_URL, DEFINITIONS_FILE_PATH)) {
        polysync_error("Failed to download definitions");
    }
}
