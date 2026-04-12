#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <sys/time.h>

#include <curl/curl.h>

#include "utils.h"

#define LATENCY_TXT_URL "/hello"
#define URL_LENGTH_MAX 255

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *data)
{
  (void)ptr;
  (void)data;
  return size * nmemb;
}

static double calc_past_time(struct timeval* start, struct timeval* end)
{
    return (end->tv_sec - start->tv_sec) * 1000 + (end->tv_usec - start->tv_usec)/1000;
}


static int measure_latency(CURL *curl, char *url)
{
    char latency_url[URL_LENGTH_MAX + sizeof(LATENCY_TXT_URL)] = {0};
    CURLcode result;
    long response_code;

    snprintf(latency_url, sizeof(latency_url), "%s%s", url, LATENCY_TXT_URL);
    curl_easy_setopt(curl, CURLOPT_URL, latency_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

    result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (result != CURLE_OK || response_code != 200) {
        printf("curl result: %s, response: %ld\n", curl_easy_strerror(result), response_code);
        return -1;
    }

    return 0;
}

int check_latency(char *url, double *elapsed)
{
    CURL *curl;
    struct timeval s_time, e_time;

    curl = curl_easy_init();
    if(!curl) {
        printf("Failed to initialize curl.\n");
        return -1;
    }

    gettimeofday(&s_time, NULL);
    if (measure_latency(curl, url) != 0) {
        curl_easy_cleanup(curl);
        return -1;
    }
    
    gettimeofday(&e_time, NULL);

    *elapsed = calc_past_time(&s_time, &e_time);
    curl_easy_cleanup(curl);
    return 0;
}


