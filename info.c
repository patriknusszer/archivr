#include "info.h"
#include "stdbool.h"
#include "unordered_map.h"
#include "node.h"
#include "debugmalloc.h"

typedef struct info {
    unordered_map* um;
    bool file;
    size_t addr;
    size_t fsz;
} info;

info* icreate(unordered_map* um, bool file, size_t addr, size_t fsz) {
    info* i = (info*)malloc(sizeof(info));
    i->addr = addr;
    i->fsz = fsz;
    i->file = file;
    i->um = um;
    return i;
}

size_t iaddr(info* i) { return i->addr; }
size_t ifsz(info* i) { return i->fsz; }
bool ifile(info* i) { return i->file; }
unordered_map* ium(info* i) { return i->um; }