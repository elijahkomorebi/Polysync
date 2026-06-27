#include <stdio.h>
#include <errno.h>
#include "polysync/io.h"
#include "polysync/memory.h"
#include "polysync/string.h"

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <windows.h>

struct PolysyncDirectory {
    PolysyncDirectoryEntry entry;
    HANDLE w_handle;
    WIN32_FIND_DATAA w_find_data;
    bool w_first;
    string w_search_path[1024];
};
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

struct PolysyncDirectory {
    PolysyncDirectoryEntry entry;
    DIR *l_directory;
};
#endif

bool polysync_io_file_exists(cstring *file_path) {
#ifdef _WIN32
    return _access(file_path, 0) == 0;
#else
    return access(file_path, F_OK) == 0;
#endif
}

string *polysync_io_file_read(cstring *file_path) {
    FILE *file = fopen(file_path, "r");

    if (!file) {
        return null;
    }

    fseek(file, 0, SEEK_END);

    u32 file_size = ftell(file);
    string *file_contents = polysync_memory_allocate(file_size + 1);
    file_contents[file_size] = 0;

    rewind(file);
    fread(file_contents, 1, file_size, file);

    return file_contents;
}

bool polysync_io_file_write(cstring *file_path, cstring *file_contents) {
    FILE *file = fopen(file_path, "w");

    if (!file) {
        return false;
    }

    i32 result = fputs(file_contents, file);

    fclose(file);

    if (result < 0) {
        return false;
    }

    return true;
}

u32 polysync_io_file_get_modification_time(cstring *file_path) {
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA w_attributes;

    if (!GetFileAttributesExA(file_path, GetFileExInfoStandard, &w_attributes)) {
        return 0;
    }

    ULARGE_INTEGER w_time;
    w_time.LowPart = attributes.ftLastWriteTime.dwLowDateTime;
    w_time.HighPart = attributes.ftLastWriteTime.dwHighDateTime;

    return w_time.QuadPart;
#else
    struct stat l_file_info;

    if (stat(file_path, &l_file_info)) {
        return 0;
    }

    return l_file_info.st_mtim.tv_sec;
#endif
}

bool polysync_io_directory_create(cstring *directory_path) {
#ifdef _WIN32
    if (_mkdir(directory_path) == 0) {
        return true;
    }

    return errno == EEXIST;
#else
    if (mkdir(directory_path, 0755) == 0) {
        return true;
    }

    return errno == EEXIST;
#endif
}

PolysyncDirectory *polysync_io_directory_open(cstring *directory_path) {
    PolysyncDirectory *directory = polysync_memory_allocate(sizeof(PolysyncDirectory));

#ifdef _WIN32
    snprintf(directory->w_search_path, 1024, "%s\\*", directory_path);

    directory->w_handle = FindFirstFileA(directory->w_search_path, &directory->w_find_data);
    directory->w_first = true;

    if (directory->w_handle == INVALID_HANDLE_VALUE) {
        polysync_error("Failed to open directory '%s'", directory_path);
    }
#else
    directory->l_directory = opendir(directory_path);
#endif

    return directory;
}

void polysync_io_directory_close(PolysyncDirectory *directory) {
#ifdef _WIN32
    FindClose(directory->w_handle);
#else
    closedir(directory->l_directory);
#endif

    polysync_memory_free(directory);
}

PolysyncDirectoryEntry *polysync_io_directory_read(PolysyncDirectory *directory) {
#ifdef _WIN32
    WIN32_FIND_DATAA *w_find_data = &directory->w_find_data;

    if (directory->w_first) {
        directory->w_first = false;
    } else {
        if (!FindNextFileA(directory->w_handle, w_find_data)) {
            return null;
        }
    }

    if (polysync_string_matches(w_find_data->cFileName, ".") || polysync_string_matches(w_find_data->cFileName, "..")) {
        return polysync_io_directory_read(directory);
    }

    PolysyncDirectoryEntry = *directory_entry = &directory->entry;
    directory_entry->name = w_find_data->cFileName;

    if (w_find_data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        directory_entry->type = POLYSYNC_DIRECTORY_ENTRY_TYPE_DIRECTORY;
    } else {
        directory_entry->type = POLYSYNC_DIRECTORY_ENTRY_TYPE_FILE;
    }

    return directory_entry;
#else
    PolysyncDirectoryEntry *directory_entry = &directory->entry;
    struct dirent *l_entry = readdir(directory->l_directory);

    if (!l_entry) {
        return null;
    }

    if (polysync_string_matches(l_entry->d_name, ".") || polysync_string_matches(l_entry->d_name, "..")) {
        return polysync_io_directory_read(directory);
    }

    directory_entry->name = l_entry->d_name;

    if (l_entry->d_type == DT_REG) {
        directory_entry->type = POLYSYNC_DIRECTORY_ENTRY_TYPE_FILE;
    } else if (l_entry->d_type == DT_DIR) {
        directory_entry->type = POLYSYNC_DIRECTORY_ENTRY_TYPE_DIRECTORY;
    } else {
        directory_entry->type = POLYSYNC_DIRECTORY_ENTRY_TYPE_OTHER;
    }

    return directory_entry;
#endif
}
