#ifndef info_h
#define info_h

#include "stdbool.h"
#include "unordered_map.h"

typedef struct info info;
size_t iaddr(info* i);
unordered_map* ium(info* i);
size_t ifsz(info* i);
bool ifile(info* i);
info* icreate(unordered_map* um, bool file, size_t addr, size_t fsz);

#endif
