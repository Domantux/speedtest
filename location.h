#ifndef LOCATION_H
#define LOCATION_H

struct loc {
    char *city;
    char *country;
};

extern struct loc location;

int find_location(void);

#endif