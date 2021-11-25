//
//  crypto.c
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 24..
//

#include "crypto.h"
typedef unsigned int uint;

void xorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos) {
    for (uint i = 0; i < bsz; i++)
        buff[i] ^= key[(*pos)++ % ksz];
}

void xnorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos) {
    for (uint i = 0; i < bsz; i++)
        buff[i] = ~(buff[i] ^ key[(*pos)++ % ksz]);
}
