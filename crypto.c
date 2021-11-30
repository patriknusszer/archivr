#include "crypto.h"
#include "debugmalloc.h"

typedef unsigned int uint;

void xorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos) {
    for (uint i = 0; i < bsz; i++)
        buff[i] ^= key[(*pos)++ % ksz];
}

void xnorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos) {
    for (uint i = 0; i < bsz; i++)
        buff[i] = ~(buff[i] ^ key[(*pos)++ % ksz]);
}
