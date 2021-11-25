//
//  crypto.h
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 24..
//

#ifndef crypto_h
#define crypto_h
typedef unsigned int uint;
typedef unsigned long size_t;

void xorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos);
void xnorcrypt(char* buff, uint bsz, char* key, int ksz, size_t* pos);

#endif /* crypto_h */
