#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stdbool.h>

#include "serverlist.h"
#include "location.h"

static const char SERVER_FILE[] = "speedtest_server_list.json";

static int copy_arguments_to_struct(char *dest, size_t dest_size, cJSON *src);

static cJSON *load_servers(const char *filename) {

    const char *error_msg = NULL;
    FILE *fp = NULL;
    char *buffer = NULL;
    cJSON *json = NULL;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        error_msg = "Unable to open the file.";
        goto cleanup;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        error_msg = "Unable to find the end of the file.";
        goto cleanup;
    }

    long file_size = ftell(fp);
    if (file_size < 0) {
        error_msg = "Unable to get the file size.";
        goto cleanup;
    }

    rewind(fp);

    buffer = (char *) malloc(((size_t)file_size + 1)* sizeof(char));
    if (buffer == NULL) {
        error_msg = "Error: Unable to alocate the memory.";
        goto cleanup;
    }

    size_t len = fread(buffer, sizeof(char), (size_t)file_size, fp);
    if (len != (size_t)file_size) {
        error_msg = "Trouble reading the file.";
        goto cleanup;
    }
    buffer[len] = '\0';
    fclose(fp);
    fp = NULL;

    json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        goto cleanup;
    }

    if (!cJSON_IsArray(json)) {
        error_msg = "The loaded list is not an array.\n";
        goto cleanup;
    }
    
    free(buffer);
    buffer = NULL;
    return json;

    cleanup:
        if (error_msg != NULL) {
            printf("Error: %s\n", error_msg);
        }
        if (fp != NULL) {
            fclose(fp);
            fp = NULL;
        }
        if (buffer != NULL) {
            free(buffer);
            buffer = NULL;
        }
        if (json != NULL) {
            cJSON_Delete(json);
            json = NULL;
        }
        return NULL;
}

static int parse_server(const char *country, const char *city, const int server_number, bool search_by_id, Server *result) {

    cJSON *json = NULL;
    const char *error_msg = NULL;

    if (result == NULL) 
    {
        return 0;
    }

    if ( !search_by_id && (country == NULL || city == NULL)) {
            error_msg = "The city or country is NULL. Server not chosen.";
            goto cleanup;
        }

    json = load_servers(SERVER_FILE);
    if (json == NULL) {
        error_msg = "Failed to load server list.";
        goto cleanup;
    }

    int size = cJSON_GetArraySize(json);

    for (int i = 0; i < size; i++) {
        cJSON *server_info = cJSON_GetArrayItem(json, i);
        if (!cJSON_IsObject(server_info)) {
            continue;
        }

        cJSON *Country = cJSON_GetObjectItemCaseSensitive(server_info, "country");
        cJSON *City = cJSON_GetObjectItemCaseSensitive(server_info, "city");
        cJSON *Provider = cJSON_GetObjectItemCaseSensitive(server_info, "provider");
        cJSON *Host = cJSON_GetObjectItemCaseSensitive(server_info, "host");
        cJSON *Id = cJSON_GetObjectItemCaseSensitive(server_info, "id");

        if (!cJSON_IsString(Country) || (Country -> valuestring == NULL) || 
            !cJSON_IsString(City) || (City -> valuestring == NULL) ||
            !cJSON_IsString(Provider) || (Provider -> valuestring == NULL) ||
            !cJSON_IsString(Host) || (Host -> valuestring == NULL) ||
            !cJSON_IsNumber(Id)) {
                continue;
        }

        if (search_by_id) {
            if (Id -> valueint != server_number) {
                continue;
            } 
        } else {
            if (strcmp(Country -> valuestring, country) != 0 || 
            strcmp(City -> valuestring, city) != 0) {
                continue;
            }
        }

        if (!copy_arguments_to_struct(result -> country, sizeof(result -> country), Country) ||
        !copy_arguments_to_struct(result -> city, sizeof(result -> city), City) ||
        !copy_arguments_to_struct(result -> provider, sizeof(result ->  provider), Provider) ||
        !copy_arguments_to_struct(result -> host, sizeof(result -> host), Host))
        {
            error_msg = "Failed to copy info to the struct";
            goto cleanup;
        }
        result -> id = Id -> valueint;

        cJSON_Delete(json);
        return 1;
    }

    error_msg = "Matching server was not found.";

    cleanup:
        if (json != NULL) {
            cJSON_Delete(json);
            json = NULL;
        }

        if (error_msg != NULL) {
            printf("Error: %s\n", error_msg);
        }

        return 0;
}

static int copy_arguments_to_struct(char *dest, size_t dest_size, cJSON *src) {
    if (dest == NULL || src == NULL || dest_size == 0)
    {
        return 0;
    }

    if (!cJSON_IsString(src) || src -> valuestring == NULL)
    {
        return 0;
    }

    strncpy(dest, src -> valuestring, dest_size - 1);
    dest[dest_size - 1] = '\0';

    return 1;
}

int parse_server_by_id(const int server_number, Server *result) {
    return parse_server(NULL, NULL, server_number, true, result);
}

int parse_server_by_location(const char *country, const char *city, Server *result) {
    return parse_server(country, city, 0, false, result);
}

    



