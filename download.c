#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <curl/curl.h>

#include "download.h"
 
#define JPG_350   "random350x350.jpg"
#define JPG_500   "random500x500.jpg"
#define JPG_750   "random750x750.jpg"
#define JPG_1000  "random1000x1000.jpg"
#define JPG_1500  "random1500x1500.jpg"
#define JPG_2000  "random2000x2000.jpg"
#define JPG_3000  "random3000x3000.jpg"
#define JPG_3500  "random3500x3500.jpg"
#define JPG_4000  "random4000x4000.jpg"

static const curl_off_t total_down_size = 2000000000;

static int xferinfo_callback(void *clientp,
                            curl_off_t dltotal,
                            curl_off_t dlnow,
                            curl_off_t ultotal, 
                            curl_off_t ulnow)
{
    (void)dltotal; (void)ultotal; (void)ulnow;

    struct timeval *start = (struct timeval *)clientp;
    struct timeval now;
    gettimeofday(&now, NULL);

    double elapsed = (now.tv_sec - start->tv_sec) + (now.tv_usec - start->tv_usec) / 1000000.0;
    double total = 15.0;
    int bar_width = 20;
    int filled = (int)(elapsed / total * bar_width);
    if (filled > bar_width) filled = bar_width;

    if (elapsed <= 0) return 0;

    double speed_mbps = (dlnow * 8.0) / (elapsed * 1024 * 1024);

    printf("\r[");
    for (int i = 0; i < bar_width; i++)
        printf(i < filled ? "=" : " ");
    printf("] %.0f/15s  %.1f Mbps", elapsed, speed_mbps);
    fflush(stdout);

    return 0;
}

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *data)
{
  (void)ptr;
  (void)data;
  return size * nmemb;
}

static int measure_download(CURL *curl, const char *url, double *speed) 
{
    struct timeval start;
    gettimeofday(&start, NULL);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &start);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo_callback);

    printf("Download test has started. Please wait\n");
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK && result != CURLE_OPERATION_TIMEDOUT) {
        return -1;
    }
    printf("\n");

    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code != 200) {
        fprintf(stderr, "curl result: %s, response: %ld\n", curl_easy_strerror(result), response_code);
        return -1;
    }

    curl_off_t raw_speed;
    curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD_T, &raw_speed);
    *speed = (double)raw_speed;
    return 0;
}

int download_speed(const char url[256], double *speed) {

    char full_url[512];
    CURL *curl;

    curl = curl_easy_init();
    if(!curl) {
        printf("Failed to initialize curl.\n");
        return -1;
    }

    snprintf(full_url, sizeof(full_url), "http://%s/download?size=%ld", url, total_down_size);
    if (measure_download(curl, full_url, speed) == 0) {
        curl_easy_cleanup(curl);
        return 0;
    }

    const char *files[] = {JPG_4000, JPG_3500, JPG_3000, JPG_2000, JPG_1500, JPG_1000, JPG_750, JPG_500, JPG_350};
    int n = sizeof(files)/sizeof(files[0]);
        

    for (int i = 0; i < n; i++) {
        snprintf(full_url, sizeof(full_url),  "http://%s/speedtest/%s", url, files[i]);
        if (measure_download(curl, full_url, speed) == 0) {
            curl_easy_cleanup(curl);
            return 0;
        }    
    }
     
    curl_easy_cleanup(curl);
    return -1;    
}

