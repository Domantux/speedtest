#ifndef OPTIONS_H
#define OPTIONS_H 

#include <stdbool.h>

typedef struct {
    int srv; 
    bool sflag;
    bool lflag;
    bool uflag; 
    bool dflag; 
    bool bflag;
    bool aflag;
} Options;

extern Options option;

void print_usage(char *argv0);

void parse_options(int argc, char* argv[]);

#endif