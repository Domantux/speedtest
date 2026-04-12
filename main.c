#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "options.h"
#include "location.h"
#include "serverlist.h"
#include "download.h"
#include "upload.h"
#include "utils.h"

int find_and_print_loc(void);
int parse_by_loc_and_print(void);

Server server = {0};

int main(int argc, char *argv[]) {

    parse_options(argc, argv);

    curl_global_init(CURL_GLOBAL_ALL);

    if (option.aflag) {
        double latency_nr = 0;
        double down_speed = 0;
        double up_speed = 0;

        if (find_and_print_loc() != 0) goto fail_cleanup;
        if (parse_by_loc_and_print() != 0) goto fail_cleanup;

        if (check_latency(server.host, &latency_nr) != 0) {
            fprintf(stderr, "Server is not active\n");
            goto fail_cleanup;
        }
        printf("Latency: %.2f ms\n", latency_nr);

        if (download_speed(server.host, &down_speed) != 0) {
            fprintf(stderr, "Download test failed\n");
            goto fail_cleanup;
        }
        printf("Download speed: %.2f Mbps\n", (down_speed * 8) / (1024 * 1024));

        if (upload_speed(server.host, &up_speed) != 0) {
            fprintf(stderr, "Upload test failed\n");
            goto fail_cleanup;
        }
        printf("Upload speed: %.2f Mbps\n", (up_speed * 8) / (1024 * 1024));

        goto cleanup;
    }

    if (option.bflag) {
        if (find_and_print_loc() != 0) goto fail_cleanup;
        if (parse_by_loc_and_print() != 0) goto fail_cleanup;

    }

    if (option.lflag) {
        if (find_and_print_loc() != 0) goto fail_cleanup;

        goto cleanup;
    }

    if (option.sflag) {
        if (parse_server_by_id(option.srv, &server) == 0) {
            fprintf(stderr, "Server ID %d not found\n", option.srv);
            goto fail_cleanup;
        }

        printf("Server:\n Country: %s\n City: %s\n Provider: %s(%s)\n", server.country, server.city, server.provider, server.host);
        if (!option.dflag && !option.uflag) {
            goto cleanup;
        }

    }

    if ((option.dflag || option.uflag) && !option.sflag) {
        fprintf(stderr, "For -d and -u options, server selection (-s) is required\n");
        goto fail_cleanup;
    }

    double latency_nr = 0;
    if (check_latency(server.host, &latency_nr) != 0) {
        fprintf(stderr, "Server is not active\n");
        goto fail_cleanup;
    }    
    printf("Latency: %.2f ms\n", latency_nr);

    if (option.dflag) {
        double down_speed = 0;

        if (download_speed(server.host, &down_speed) != 0) {
            fprintf(stderr, "Download test failed\n");
            goto fail_cleanup;
        }
        printf("Download speed: %.2f Mbps\n", (down_speed * 8) / (1024 * 1024));
    }

     if (option.uflag) {
        double up_speed = 0;
        
        if (upload_speed(server.host, &up_speed) != 0) {
            fprintf(stderr, "Upload test failed\n");
            goto fail_cleanup;
        }
        printf("Upload speed: %.2f Mbps\n", (up_speed * 8) / (1024 * 1024));
    }

    cleanup:
        curl_global_cleanup();
        return 0;

    fail_cleanup:
        curl_global_cleanup();
        return 1;
}

int find_and_print_loc(void) {
    if (find_location() != 0) {
            fprintf(stderr, "Failed to get location\n");
            return -1;
    }
    printf("Location: %s, %s\n", location.city, location.country);
    return 0;
}

int parse_by_loc_and_print(void) {
    if (parse_server_by_location(location.country, location.city, &server) == 0) {
        fprintf(stderr, "No server found for location\n");
        return -1;
    }
    printf("Server: %s (%s)\n", server.provider, server.host);
    return 0;
}



