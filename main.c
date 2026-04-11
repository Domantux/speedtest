#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "options.h"
#include "location.h"
#include "serverlist.h"
#include "download.h"

int main(int argc, char *argv[]) {

    parse_options(argc, argv);

    curl_global_init(CURL_GLOBAL_ALL);

    if (find_location() != 0) {
        fprintf(stderr, "Failed to get location\n");
        curl_global_cleanup();
        return 1;
    }
    printf("Location: %s, %s\n", location.city, location.country);

    Server server = {0};
    if (parse_server_by_location(location.country, location.city, &server) == 0) {
        fprintf(stderr, "No server found for location\n");
        curl_global_cleanup();
        return 1;
    }
    printf("Server: %s (%s)\n", server.provider, server.host);

    double speed = 0;
    if (download_speed(server.host, &speed) != 0) {
        fprintf(stderr, "Download test failed\n");
        curl_global_cleanup();
        return 1;
    }
    printf("Download speed: %.2f Mbps\n", (speed * 8) / (1024 * 1024));

    curl_global_cleanup();
    return 0;
}
