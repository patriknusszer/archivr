#include "util.h"
#include "node.h"
#include "info.h"
#include "stdbool.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

char* _strcat(char* to, char* str) {
    int tosz = (int)strlen(to);
    char* ret = (char*)malloc(sizeof(char) * (tosz + strlen(str) + 1));
    strcpy(ret, to);
    strcpy(ret + tosz, str);
    return ret;
}

char* combine(char* path, char* with) {
    int tosz = (int)strlen(path);
    char* ret = (char*)malloc(sizeof(char) * (tosz + strlen(with) + 2));
    strcpy(ret, path);
    ret[tosz] = '/';
    strcpy(ret + tosz + 1, with);
    return ret;
}

size_t naddr(node* n) { return iaddr(((info*)ndat(n))); }
size_t nfsz(node* n) { return ifsz(((info*)ndat(n))); }
bool nfile(node* n) { return ifile(((info*)ndat(n))); }

int* kmptbl(char* pattern) {
    int psz = (int)strlen(pattern);
    int* tbl = (int*)malloc(sizeof(int) * psz);
    int lps = 0, i = 0;
    tbl[i++] = lps;
    
    while (i < psz) {
        if (pattern[i] == pattern[lps])
            tbl[i++] = ++lps;
        else if (lps == 0)
            tbl[i++] = 0;
        else
            lps = tbl[lps - 1];
    }
    
    return tbl;
}

char* kmpfi(int* tbl, char* str, char* pat) {
    int psz = (int)strlen(pat);
    int si = 0, pi = 0;
    
    while (str[si] != '\0') {
        if (str[si] == pat[pi]) {
            if (pi == psz - 1)
                return str + si - pi;
            else {
                si++;
                pi++;
            }
        }
        else if (pi != 0)
            pi = tbl[pi - 1];
        else
            si++;
    }
    
    return 0;
}

char* ppnext(char* from) {
    for (; *from != '\0'; from++)
        if (*from == '/')
            return from;
    
    return 0;
}

char** ppsplit(char* path, uint* splitsz) {
    int psz = (int)strlen(path);
    if (psz == 0) return 0;
    
    int cap = 10, sz = 0;
    int* vec = (int*)malloc(sizeof(int) * cap);
    
    if (vec == 0) return 0;
    
    for (char* occ = path; (occ = ppnext(occ)) != 0; occ++) {
        vec[sz++] = (int)(occ - path);
        
        if (sz == 1 && vec[sz - 1] == 0) {
            sz = 0;
            path++;
        }

        
        if (sz == cap) {
            vec = (int*)realloc(vec, sizeof(int) * (cap *= 2));
            if (vec == 0) return 0;
        }
    }
    
    char** mat = (char**)malloc(sizeof(char*) * (sz + 1));
    
    if (mat == 0) {
        free(vec);
        return 0;
    }
    
    int from = 0;
    
    for (int i = 0; i < sz; i++) {
        int ssz = vec[i] - from;
        mat[i] = (char*)malloc(sizeof(char) * (ssz + 1));
        memcpy(mat[i], path + from, sizeof(char) * ssz);
        mat[i][ssz] = '\0';
        from = vec[i] + 1;
    }
    
    free(vec);
    mat[sz] = (char*)malloc(sizeof(char) * (psz - from + 1));
    strcpy(mat[sz], path + from);
    *splitsz = sz + 1;
    return mat;
}

unordered_map* num(node* n) { return ium(((info*)ndat(n))); }

void ssrelease(char** mat, int sz) {
    for (int i = 0; i < sz; i++)
        free(mat[i]);
    
    free(mat);
}

uint fgetsz(FILE* f) {
    uint pos = (uint)ftell(f);
    fseek(f, 0, SEEK_END);
    uint fsz = (uint)ftell(f);
    fseek(f, pos, SEEK_SET);
    return fsz;
}

int mkd(const char* name)
{
#ifdef _WIN32
    return _mkdir(name);
#else
    return mkdir(name, 0777);
#endif
}

node* nsub(node* n, char* key) {
    if (nfile(n)) return 0;
    return umget(num(n), key);
}

node* ncdsub(node* n, char* key) {
    if (nsub(n, key) == 0 && !nfile(n))
        return umput(num(n), key, icreate(umcreate(), false, 0, 0));
    
    return 0;
}

node* ncfsub(node* n, char* key, size_t addr, size_t fsz) {
    if (nsub(n, key) == 0 && !nfile(n))
        return umput(num(n), key, icreate(0, true, addr, fsz));
    return 0;
}

char* lastcomp(char* path) {
    char* prev = path, *next;
    
    while ((next = strchr(prev, '/')) != 0)
        prev = next + 1;
    
    return prev;
}

bool getbit(char field, int i) {
    return ((field >> i) & 1);
}

void setbit(char* field, int i) {
    *field |= (int)pow(2, i);
}

char* cwd(char* buff, size_t max) {
#ifdef _WIN32
    return _getcwd(buff, max);
#else
    return getcwd(buff, max);
#endif
}
