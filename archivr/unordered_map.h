#ifndef unordered_map_h
#define unordered_map_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "node.h"
#include "IT.h"

typedef struct unordered_map unordered_map;

// bu = bucket
// ll = linked list
// um = unordered map

IT* umiterator(unordered_map* um);
void umwalk(unordered_map* um, void (*iterator)(node* i));
unordered_map* umcreate(void);
node* umget(unordered_map* um, char* key);
node* umput(unordered_map* um, char* key, void* dat);
void enummap(node* n);
void umrelease(unordered_map* um);

#endif
