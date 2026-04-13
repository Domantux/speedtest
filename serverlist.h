#ifndef SERVERLIST_H
#define SERVERLIST_H 

typedef struct {
    char country[64];
    char city[64];
    char provider[128];
    char host[256];
    int id;
} Server;

int parse_server_by_id(const int server_number, Server *results);
int parse_server_by_location(const char *country, const char *city, Server *results, int max, int *count);
   

#endif