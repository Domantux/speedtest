#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "upload.h"

static size_t read_cb(char *ptr, size_t size, size_t nmemb, void *data)
{

    curl_off_t *remaining = (curl_off_t *)data;
    size_t len = size * nmemb;

    if (*remaining <= 0) {
        return 0;
    }

    if ((curl_off_t)len > *remaining) {
        len = (size_t)*remaining;
    }

    memset(ptr, 'A', len);
    *remaining -= len;
    return len;
}

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *data)
{
  (void)ptr;
  (void)data;
  return size * nmemb;
}

static int measure_upload(CURL *curl, const char *url, double *speed) 
{
    curl_off_t remaining = 50000000;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Expect:");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, remaining);
    curl_easy_setopt(curl, CURLOPT_READDATA, &remaining);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK && result != CURLE_OPERATION_TIMEDOUT) {
        curl_slist_free_all(headers);
        return -1;
    }

    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code != 200) {
        printf("curl result: %s, response: %ld\n", curl_easy_strerror(result), response_code);
        curl_slist_free_all(headers);
        return -1;
    }

    curl_off_t raw_speed;
    curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &raw_speed);
    *speed = (double)raw_speed;
    curl_slist_free_all(headers);
    return 0;
}

int upload_speed(const char url[256], double *speed) {

    char full_url[512];
    CURL *curl;

    curl = curl_easy_init();
    if(!curl) {
        printf("Failed to initialize curl.\n");
        return -1;
    }

    snprintf(full_url, sizeof(full_url), "http://%s/upload", url);
    if (measure_upload(curl, full_url, speed) == 0) {
        goto cleanup;
    }

    snprintf(full_url, sizeof(full_url),  "http://%s/speedtest/upload.php", url);
    if (measure_upload(curl, full_url, speed) == 0) {
        goto cleanup;
    }
    
    snprintf(full_url, sizeof(full_url),  "http://%s/speedtest/upload.asp", url);
    if (measure_upload(curl, full_url, speed) == 0) {
        goto cleanup;
    }
     
    curl_easy_cleanup(curl);
    return -1;

    cleanup :
        curl_easy_cleanup(curl);
        return 0;
}
