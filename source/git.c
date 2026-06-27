#include <git2.h>
#include "polysync/git.h"
#include "polysync/error.h"

void polysync_git_initialize_repository(void) {
    git_libgit2_init();

    git_repository *repository = NULL;
    i32 error = git_repository_init(&repository, ".", false);

    if (error < 0) {
        polysync_error("Failed to initialize git repository");
    }

    git_repository_free(repository);
    git_libgit2_shutdown();
}
