#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>

#include "options.h"

Options option = {0};

void print_usage(char *argv0)
{
    printf("Usage %s [-s server] [-u] [-d] [-b] [-l] [-h] [-v]\n", argv0);
    printf("    -s <server_number>: choose particular server\n");
    printf("    -u:                 measure upload speed\n");
    printf("    -d:                 measure download speed\n");
    printf("    -b:                 best server chosen according to the location\n");
    printf("    -l:                 prints out your current location\n");
    printf("    -h:                 prints out help window (current window)\n");
    printf("    -v:                 prints out program's version\n");    
}

void parse_options(int argc, char* argv[]) 
{
        int opt;
        char *endptr;
        errno = 0;

    while((opt = getopt(argc, argv, "s:udblhv")) != -1)
    {
        switch(opt)
        {
            case 's':
                long val = strtol(optarg, &endptr, 10);
                if(errno != 0 || endptr == optarg || *endptr != '\0') {
                        printf("Invalid server number");
                        exit(1);
                }
                option.srv = (int)val; 
                option.sflag = true;
                break;

            case 'l':
                option.lflag = true;
                break;

            case 'u':
                option.uflag = true;
                break;

            case 'd':
                option.dflag = true;
                break;

            case 'b':
                option.bflag = true;
                break;

            case 'h':
                print_usage(argv[0]);
                exit(0);

            case 'v':
                printf("speedtest 1.0\n");
                exit(0);

            case '?':
                printf("Unknown option %c\n", optopt);
                break;

            case ':':
                printf("Missing args %c\n", optopt);
                break;
        }
    }
}
