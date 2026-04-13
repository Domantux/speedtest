#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "location.h"

#define API_URL "http://ip-api.com/json"

struct memory {
  char *response;
  size_t size;
};

struct memory chunk = {0};
struct loc location = {0}; 

static int parse_location(char *raw);

static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)clientp;
 
  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if(!ptr) {
    free(mem->response);
    mem->response = NULL;
    mem->size = 0;
    return 0; 
  }

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;
 
  return realsize;
}

int find_location(void)
{
  int status = 0;

  CURL *curl = curl_easy_init();
   
  if (!curl) {
      fprintf(stderr, "curl init failed\n");
      return 1;
    }

  curl_easy_setopt(curl, CURLOPT_URL, API_URL);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  CURLcode result = curl_easy_perform(curl);
  if (result != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform failed: %s\n", curl_easy_strerror(result));
    status = 1;
    goto cleanup;
  }

  if (chunk.response == NULL || parse_location(chunk.response)) {
    status = 1;
    goto cleanup;
  }

  cleanup:
    free(chunk.response);
    curl_easy_cleanup(curl);
    chunk.response = NULL;
    chunk.size = 0;
    return status;
}

static int parse_location(char *raw)
{

  char *new_city = NULL;
  char *new_country = NULL;

  cJSON *json = cJSON_Parse(raw);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      printf("Error: %s\n", error_ptr);
    }
    return 1;
  }

  cJSON *Country = cJSON_GetObjectItemCaseSensitive(json, "country");
  cJSON *City = cJSON_GetObjectItemCaseSensitive(json, "city");
  if (!cJSON_IsString(Country) || (Country->valuestring == NULL) || 
  !cJSON_IsString(City) || (City->valuestring == NULL)) {
    printf("Unable to find the correct objects\n");
    cJSON_Delete(json);
    return 1;
  }
  
  new_country = (char *)malloc((strlen(Country->valuestring) + 1) * sizeof(char));
  new_city = (char *)malloc((strlen(City->valuestring) + 1) * sizeof(char));
  if (new_country == NULL || new_city == NULL) {
    printf("Error: Unable to alocate the memory.\n");
    free(new_country);
    free(new_city);
    cJSON_Delete(json);
    return 1;
  }

  strcpy(new_country, Country -> valuestring);
  strcpy(new_city, City -> valuestring);
  free(location.country);
  free(location.city);
  location.country = new_country;
  location.city = new_city;

  cJSON_Delete(json);
  return 0;
}