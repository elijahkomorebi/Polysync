#include <stdio.h>
#include <curl/curl.h>
#include "polysync/http.h"

static u32 polysync_http_curl_write_callback(void *file_contents, u32 file_size, u32 file_count, void *file) {
    return fwrite(file_contents, file_size, file_count, file);
}

bool polysync_http_download_file(cstring *url, cstring *file_path) {
    FILE *file = fopen(file_path, "w");

    if (!file) {
        return false;
    }

    CURL *curl = curl_easy_init();

    if (!curl) {
        fclose(file);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, polysync_http_curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    CURLcode result = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    fclose(file);

    return result == CURLE_OK;
}
