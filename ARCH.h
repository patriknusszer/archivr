#ifndef ARCH_h
#define ARCH_h

#include "unordered_map.h"

typedef struct ARCH ARCH;
ARCH* archparse(char* path, char* key);
// bool archwrite(ARCH* a, char* path, char* apath);
void archprint(ARCH* a);
bool archdincorp(ARCH* a, char* path);
bool archfincorp_e(ARCH* a, char* path, char* apath);
bool archfincorp(ARCH* a, char* fpath);
bool archdincorp_e(ARCH* a, char* fpath, char* tpath);
bool archsetcrypto(ARCH* a, bool on, char* key, bool xor);
bool archextract(ARCH* a, char* fpath, char* tpath);
char archfeof(ARCH* a);
size_t archfsz(ARCH* a);
int archfread(ARCH* a, char* buff, int n);
bool archfseek(ARCH* a, char* path);
ARCH* archcreate(char* path, char* key, bool xor);
void archrelease(ARCH* a);

#endif
