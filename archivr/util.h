#ifndef util_h
#define util_h

#include "stdbool.h"
#include "node.h"
#include "unordered_map.h"

bool getbit(char field, int i);
void setbit(char* field, int i);
size_t naddr(node* n);
bool nfile(node* n);
unordered_map* num(node* n);
size_t nfsz(node* n);
node* nsub(node* n, char* key);
node* ncdsub(node* n, char* key);
node* ncfsub(node* n, char* key, size_t addr, size_t fsz);

int* kmptbl(char* pattern);
char* kmpfi(int* tbl, char* str, char* pat);

void ssrelease(char** mat, int sz);

char* lastcomp(char* path);
char* ppnext(char* from);
char** ppsplit(char* path, uint* splitsz);

uint fgetsz(FILE* f);
int mkd(const char* dir);
char* cwd(char* buff, size_t max);
char* _strcat(char* to, char* str);
char* combine(char* path, char* with);

#endif
