#include "polysync/setup.h"
#include "polysync/io.h"
#include "polysync/error.h"
#include "polysync/git.h"

static cstring *PROJECT_FILE_CONTENTS =
"{\n"
"    \"name\": \"Project\",\n"
"    \"tree\": {\n"
"        \"source\": {\n"
"            \"server\": \"ScriptService\",\n"
"            \"client\": \"Hidden\",\n"
"            \"shared\": \"Hidden\"\n"
"        },\n"
"        \"build\": {\n"
"            \"server\": \"ScriptService\",\n"
"            \"client\": \"Hidden\",\n"
"            \"shared\": \"Hidden\"\n"
"        }\n"
"    }\n"
"}";
static cstring *SERVER_EXAMPLE_FILE_CONTENTS = "print(\"Hello, Polytoria!\")";
static cstring *CLIENT_EXAMPLE_FILE_CONTENTS = "print(\"Hello, Polytoria!\")";
static cstring *SHARED_EXAMPLE_FILE_CONTENTS = "local function foo(bar)\n    print(bar)\nend\n\nreturn foo";
static cstring *GIT_IGNORE_FILE_CONTENTS = "build/\ntypes/";
static cstring *LUAURC_FILE_CONTENTS =
"{\n"
"    \"languageMode\": \"nocheck\",\n"
"    \"aliases\": {\n"
"        \"server\": \"source/server\",\n"
"        \"client\": \"source/client\",\n"
"        \"shared\": \"source/shared\"\n"
"    }\n"
"}";
static string *PT_PROJECT_FILE_CONTENTS =
"{\n"
"  \"ProjectName\": \"Project\",\n"
"  \"MainWorld\": \"world.poly\",\n"
"  \"IconID\": null\n"
"}";

void polysync_setup(void) {
    polysync_setup_project();
    polysync_setup_git();
    polysync_setup_directories();
    polysync_setup_examples();
}

void polysync_setup_project(void) {
    if (!polysync_io_file_write(".luaurc", LUAURC_FILE_CONTENTS)) {
        polysync_error("Failed to write .luaurc");
    }

    if (polysync_io_file_exists("polysync.json")) {
        polysync_error("project.json already exists");
    }

    if (!polysync_io_file_write("polysync.json", PROJECT_FILE_CONTENTS)) {
        polysync_error("Failed to write project.json");
    }

    if (!polysync_io_file_write("project.ptproj", PT_PROJECT_FILE_CONTENTS)) {
        polysync_error("Failed to write project.ptproj");
    }

    if (!polysync_io_file_write("world.poly", "")) {
        polysync_error("Failed to write world.poly");
    }
}

void polysync_setup_git(void) {
    polysync_git_initialize_repository();

    if (!polysync_io_file_write(".gitignore", GIT_IGNORE_FILE_CONTENTS)) {
        polysync_error("Failed to write .gitignore");
    }
}

void polysync_setup_directories(void) {
    if (!polysync_io_directory_create("source")) {
        polysync_error("Failed to create source");
    }

    if (!polysync_io_directory_create("source/client")) {
        polysync_error("Failed to create source/client");
    }

    if (!polysync_io_directory_create("source/server")) {
        polysync_error("Failed to create source/server");
    }

    if (!polysync_io_directory_create("source/shared")) {
        polysync_error("Failed to create source/shared");
    }

    if (!polysync_io_directory_create("build")) {
        polysync_error("Failed to create build");
    }

    if (!polysync_io_directory_create("types")) {
        polysync_error("Failed to create types");
    }
}

void polysync_setup_examples(void) {
    if (!polysync_io_file_write("source/server/Server.server.luau", SERVER_EXAMPLE_FILE_CONTENTS)) {
        polysync_error("Failed to write source/server/server.Example.luau");
    }
    
    if (!polysync_io_file_write("source/client/Client.client.luau", CLIENT_EXAMPLE_FILE_CONTENTS)) {
        polysync_error("Failed to write source/client/client.Example.luau");
    }

    if (!polysync_io_file_write("source/shared/Shared.luau", SHARED_EXAMPLE_FILE_CONTENTS)) {
        polysync_error("Failed to write source/shared/Example.luau");
    }
}
