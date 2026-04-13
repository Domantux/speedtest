#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <signal.h>

#include "options.h"
#include "location.h"
#include "serverlist.h"
#include "download.h"
#include "upload.h"
#include "utils.h"

int find_and_print_loc(void);

void signal_handler(int signum) {
    (void)signum;
    curl_global_cleanup();
    exit(1);
}

int main(int argc, char *argv[]) {

    parse_options(argc, argv);

    curl_global_init(CURL_GLOBAL_ALL);

    Server server = {0};

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGHUP, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);


    // Auto test
    if (option.aflag) {
        Server servers[10];

        int count = 0;
        double down_speed = 0;
        double up_speed = 0;

        if (find_and_print_loc() != 0) goto fail_cleanup;
        if (parse_server_by_location(location.country, location.city, servers, 10, &count) != 1) goto fail_cleanup;

        for (int i = 0; i < count; i++) {
            double latency_nr = 0;

            if (check_latency(servers[i].host, &latency_nr) != 0) {
               continue;
            }
            printf("Server: %s (%s)\n", servers[i].provider, servers[i].host);
            printf("Latency: %.2f ms\n", latency_nr);

            if (download_speed(servers[i].host, &down_speed) != 0) {
                fprintf(stderr, "Download test failed\n");
                goto fail_cleanup;
            }
            printf("Download speed: %.2f Mbps\n", (down_speed * 8) / (1024 * 1024));

            if (upload_speed(servers[i].host, &up_speed) != 0) {
                fprintf(stderr, "Upload test failed\n");
                goto fail_cleanup;
            }
            printf("Upload speed: %.2f Mbps\n", (up_speed * 8) / (1024 * 1024));

            goto cleanup;
        }

        fprintf(stderr, "No reachable server found\n");
        goto fail_cleanup;
    }

    // Find best server
    if (option.bflag) {
        Server servers[10];

        int count = 0;

        if (find_and_print_loc() != 0) goto fail_cleanup;
        
        if (parse_server_by_location(location.country, location.city, servers, 1, &count) != 1) {
            fprintf(stderr, "No server found for location\n");
            goto fail_cleanup;
        }
        printf("Server: %s (%s)\n", servers[0].provider, servers[0].host);
        goto cleanup;
    }

    // Location test
    if (option.lflag) {
        if (find_and_print_loc() != 0) goto fail_cleanup;

        goto cleanup;
    }

    // Chose particular server
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

    // Download and upload test only available with server(s) option
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

    // Download test
    if (option.dflag) {
        double down_speed = 0;

        if (download_speed(server.host, &down_speed) != 0) {
            fprintf(stderr, "Download test failed\n");
            goto fail_cleanup;
        }
        printf("Download speed: %.2f Mbps\n", (down_speed * 8) / (1024 * 1024));
    }

    // Upload test
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

// Helper functions

int find_and_print_loc(void) {
    if (find_location() != 0) {
            fprintf(stderr, "Failed to get location\n");
            return -1;
    }
    printf("Location: %s, %s\n", location.city, location.country);
    return 0;
}




