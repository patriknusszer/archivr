#include "ARCH.h"
#include "unordered_map.h"
#include <stdbool.h>
#include "util.h"
#include "info.h"
#include "IT.h"
#include "node.h"
#include "dirmapper.h"
#include "crypto.h"
#include "NIT.h"
#include "debugmalloc.h"

typedef struct ARCH {
    FILE* f;
    node* root;
    char* key;
    int ksz;
    bool xor;
    node* sel;
    size_t selpos;
} ARCH;

char* archreadpath(ARCH* a) {
    int sz = 0, cap = 20;
    char* path = (char*)malloc(sizeof(char) * cap);
    if (path == 0) return 0;
    
    char c;
    
    while ((c = fgetc(a->f)) != '\0') {
        path[sz++] = c;
        
        if (sz == cap) {
            path = (char*)realloc(path, sizeof(char) * (cap *= 2));
            if (path == 0) return 0;
        }
    }
    
    path[sz] = '\0';
    
    if (cap != sz)
        path = realloc(path, sizeof(char) * (sz + 1));
    
    return path;
}

node* archrinsert(node* n, size_t pos, size_t fsz, char** mat, int msz, int i) {
    if (i == msz - 1) {
        if (fsz == 0)
            return ncdsub(n, mat[i]);
        else
            return ncfsub(n, mat[i], pos, fsz);
    }
    node* next = nsub(n, mat[i]);
    
    if (next == 0)
        next = ncdsub(n, mat[i]);
        
    return archrinsert(next, pos, fsz, mat, msz, i + 1);
}

char archreadbitfield(ARCH* a) { return fgetc(a->f); }

void archsetbitfield(ARCH* a, bool crypt, bool xor) {
    size_t mem = ftell(a->f);
    fseek(a->f, 0, SEEK_SET);
    char field = 0;
    
    if (crypt) {
        setbit(&field, 0);
        
        if (!xor)
            setbit(&field, 1);
    }
    
    fwrite(&field, sizeof(char), 1, a->f);
    fseek(a->f, mem, SEEK_SET);
}

bool archreadblock(ARCH* a) {
    uint fsz;
    if (fread(&fsz, sizeof(uint), 1, a->f) == 0) return false;
    size_t pos = ftell(a->f);
    fseek(a->f, fsz, SEEK_CUR);
    char* path = archreadpath(a);
    uint sz;
    char** mat = ppsplit(path, &sz);
    free(path);
    bool ret = archrinsert(a->root, pos, fsz, mat, sz, 0) != 0;
    ssrelease(mat, sz);
    return ret;
}

int archreader(ARCH* a, char* buff, int n, size_t* pos) {
    int read = (int)fread(buff, sizeof(char), n, a->f);
    
    if (a->key != 0) {
        if (a->xor) {
            for (int i = 0; i < read; i++)
                buff[i] ^= a->key[(*pos)++ % a->ksz];
        }
        else {
            for (int i = 0; i < read; i++)
                buff[i] = ~(buff[i] ^ a->key[(*pos)++ % a->ksz]);
        }
    }
    
    return read;
}

void archwriter(ARCH* a, char* buff, int n, size_t* pos) {
    if (a->key != 0) {
        if (a->xor) {
            for (int i = 0; i < n; i++)
                buff[i] ^= a->key[(*pos)++ % a->ksz];
        }
        else {
            for (int i = 0; i < n; i++)
                buff[i] = ~(buff[i] ^ a->key[(*pos)++ % a->ksz]);
        }
    }
    
    fwrite(buff, sizeof(char), n, a->f);
}

bool archforceappend(ARCH* a, char* path, char* apath, uint* fsz, size_t* pos) {
    FILE* f = fopen(path, "rb");
    if (f == 0) return false;
    *fsz = fgetsz(f);
    uint sz;
    char** mat;
    mat = ppsplit(path, &sz);
    if (mat == 0) return false;
    fseek(a->f, 0, SEEK_END);
    fwrite(fsz, sizeof(uint), 1, a->f);
    *pos = ftell(a->f);
    char chunk[1024];
    uint read = 0;
    size_t fpos = 0;
    
    while ((read = (uint)fread(chunk, sizeof(char), 1024, f)) != 0)
        archwriter(a, chunk, read, &fpos);
    
    // fwrite(chunk, sizeof(char), read, a->f);
    
    fclose(f);
    fwrite(apath, sizeof(char), strlen(apath) + 1, a->f);
    fflush(a->f);
    ssrelease(mat, sz);
    return true;
}

void archrdincorp(ARCH* a, node* n, char* path, char* apath) {
    SUBS* subs = subsof(path);
    int fcnt, dcnt;
    char** files = subsfiles(subs, &fcnt);
    char** dirs = subsdirs(subs, &dcnt);
    
    for (int i = 0; i < fcnt; i++) {
        node* f = nsub(n, files[i]);
        
        if (f == 0) {
            char* fname = combine(path, files[i]);
            char* faname = combine(apath, files[i]);
            uint fsz;
            size_t pos;
            
            if (archforceappend(a, fname, faname, &fsz, &pos))
                ncfsub(n, files[i], pos, fsz);
            
            free(fname);
            free(faname);
        }
    }
    
    ssrelease(files, fcnt);

    for (int i = 0; i < dcnt; i++) {
        node* next = nsub(n, dirs[i]);
        
        if (next == 0)
            next = ncdsub(n, dirs[i]);
        
        char* dname = combine(path, dirs[i]);
        char* daname = combine(apath, dirs[i]);
        archrdincorp(a, next, dname, daname);
        free(dname);
        free(daname);
    }
    
    ssrelease(dirs, dcnt);
    free(subs);
}

bool archdincorp_e(ARCH* a, char* fpath, char* tpath) {
    if (fpath == 0 || tpath == 0)
        return false;

    char* final = combine(tpath, lastcomp(fpath));
    node* n = nmakepath(a->root, final);
    archrdincorp(a, n, fpath, final);
    free(final);
    return true;
}

bool archdincorp(ARCH* a, char* path) {
    if (a == 0) return false;
    size_t pos = ftell(a->f);
    char* key = lastcomp(path);
    node* root = nsub(a->root, key);
    if (root != 0) return false;
    root = ncdsub(a->root, key);
    archrdincorp(a, root, path, key);
    fseek(a->f, pos, SEEK_SET);
    return true;
}

bool archfincorp(ARCH* a, char* fpath) { return archfincorp_e(a, fpath, lastcomp(fpath)); }

bool archfincorp_e(ARCH* a, char* fpath, char* tpath) {
    size_t mem = ftell(a->f);
    FILE* f = fopen(fpath, "rb");
    uint fsz = fgetsz(f);
    uint sz;
    char** mat;
    
    if (tpath == 0)
        mat = ppsplit(fpath, &sz);
    else
        mat = ppsplit(tpath, &sz);
    
    if (mat == 0) return false;
    
    if (!archrinsert(a->root, ftell(a->f) + 4, fsz, mat, sz, 0))
        return false;
    
    fwrite(&fsz, sizeof(uint), 1, a->f);
    char chunk[1024];
    uint read = 0;
    size_t pos = 0;
    
    while ((read = (uint)fread(chunk, sizeof(char), 1024, f)) != 0)
        archwriter(a, chunk, read, &pos);
    
    fclose(f);
    fwrite(tpath, sizeof(char), strlen(tpath) + 1, a->f);
    fflush(a->f);
    ssrelease(mat, sz);
    fseek(a->f, mem, SEEK_SET);
    return true;
}

void archrcryptthrough(ARCH* a, node* n) {
    if (nfile(n)) {
        size_t addr = naddr(n);
        size_t fsz = nfsz(n);
        fseek(a->f, addr, SEEK_SET);
        char buff[1024];
        size_t fpos = 0;
        
        for (uint i = 0; i < fsz / 1024; i++) {
            fread(buff, sizeof(char), 1024, a->f);
            
            if (a->xor)
                xorcrypt(buff, 1024, a->key, a->ksz, &fpos);
            else
                xnorcrypt(buff, 1024, a->key, a->ksz, &fpos);
            
            fseek(a->f, -1024, SEEK_CUR);
            fwrite(buff, sizeof(char), 1024, a->f);
        }
        
        int r = fsz % 1024;
        fread(buff, sizeof(char), r, a->f);
        
        if (a->xor)
            xorcrypt(buff, r, a->key, a->ksz, &fpos);
        else
            xnorcrypt(buff, r, a->key, a->ksz, &fpos);
        
        fseek(a->f, -1 * r, SEEK_CUR);
        fwrite(buff, sizeof(char), r, a->f);
    }
    else {
        NIT* nit = nitcreate(n);
        node* next;
        
        while ((next = nitnext(nit)) != 0)
            archrcryptthrough(a, next);
        
        nitrelease(nit);
    }
}

void archcryptthrough(ARCH* a) {
    size_t mem = ftell(a->f);
    archrcryptthrough(a, a->root);
    fseek(a->f, mem, SEEK_SET);
}

bool archsetcrypto(ARCH* a, bool on, char* key, bool xor) {
    if (a == 0) return false;
    
    if (on && a->key != 0)
        return false;
    
    if (!on && a->key == 0)
        return false;
    
    if (on && key == 0)
        return false;
    
    if (on) {
        int ksz = (int)strlen(key);
        a->key = (char*)malloc(sizeof(char) * (ksz + 1));
        strcpy(a->key, key);
        a->ksz = ksz;
        a->xor = xor;
        archcryptthrough(a);
        archsetbitfield(a, true, xor);
    }
    else {
        archcryptthrough(a);
        archsetbitfield(a, false, false);
        free(a->key);
        a->key = 0;
    }
    
    return true;
}

ARCH* archparse(char* path, char* key) {
    FILE* f = fopen(path, "rb+");
    ARCH* a = (ARCH*)malloc(sizeof(ARCH));
    a->sel = 0;
    unordered_map* um = umcreate();
    a->root = ncreate(0, icreate(um, false, 0, 0), "", 0);
    a->f = f;
    
    if (f == 0) {
        perror("Error opening archive file!");
        return 0;
    }
    
    char c = archreadbitfield(a);
    
    if (getbit(c, 0)) {
        if (key == 0)
            return 0;
        
        int ksz = (int)strlen(key);
        if (ksz == 0) return 0;
        a->key = (char*)malloc(sizeof(char) * (ksz + 1));
        a->ksz = ksz;
        strcpy(a->key, key);
        a->xor = !getbit(c, 1);
    }
        
    while (archreadblock(a));
    return a;
}

node* archfind(ARCH* a, char* path) {
    if (*path == '\0') return a->root;
    uint sz;
    char** mat = ppsplit(path, &sz);
    node* n = nfind(a->root, 0, sz, mat);
    ssrelease(mat, sz);
    return n == 0 ? 0 : n;
}

void archrextract(ARCH* a, node* n, char* p) {
    if (n == 0) return;
    char* np = combine((char*)p, nkey(n));
    
    if (nfile(n)) {
        FILE* f = fopen(np, "wb");
        fseek(a->f, naddr(n), SEEK_SET);
        size_t fsz = nfsz(n);
        char buf[1024];
        size_t pos = 0;
        
        for (int i = 0; i < fsz / 1024; i++) {
            //fread(buf, sizeof(char), 1024, af->f);
            archreader(a, buf, 1024, &pos);
            fwrite(buf, sizeof(char), 1024, f);
        }
        
        int r = fsz % 1024;
        archreader(a, buf, r, &pos);
        // fread(buf, sizeof(char), r, af->f);
        fwrite(buf, sizeof(char), r, f);
        fflush(f);
        fclose(f);
    }
    else {
        mkd(np);
        NIT* nit = nitcreate(n);
        node* next;
        
        while ((next = nitnext(nit)) != 0)
            archrextract(a, next, np);
        
        nitrelease(nit);
    }
    
    free(np);
}

bool archextract(ARCH* a, char* fpath, char* tpath) {
    size_t mem = ftell(a->f);
    node* n = archfind(a, fpath);
    
    if (n == 0) return false;
    
    if (*nkey(n) == '\0') {
        NIT* nit = nitcreate(n);
        node* next;
        
        while ((next = nitnext(nit)) != 0)
            archrextract(a, next, tpath);
        
        nitrelease(nit);
    }

    archrextract(a, n, tpath);
    fseek(a->f, mem, SEEK_SET);
    return true;
}

char archfeof(ARCH* a) {
    if (a->sel == 0)
        return -1;
    
    return (nfsz(a->sel) - a->selpos) == 0;
}

size_t archfsz(ARCH* a) {
    if (a->sel == 0)
        return -1;
    
    return nfsz(a->sel);
}

int archfread(ARCH* a, char* buff, int n) {
    if (a->sel == 0) return -1;
    size_t fsz = nfsz(a->sel);
    
    if (n > fsz - a->selpos)
        n = (int)(fsz - a->selpos);
    
    int read = archreader(a, buff, n, &a->selpos);
    return read;
}

bool archfseek(ARCH* a, char* path) {
    uint sz;
    char** mat = ppsplit(path, &sz);
    node* n = nfind(a->root, 0, sz, mat);
    ssrelease(mat, sz);
    if (n == 0 || !nfile(n)) return false;
    fseek(a->f, naddr(n), SEEK_SET);
    a->sel = n;
    a->selpos = 0;
    return true;
}

ARCH* archcreate(char* path, char* key, bool xor) {
    FILE* f = fopen(path, "wb+");
    ARCH* a = (ARCH*)malloc(sizeof(ARCH));
    a->sel = 0;
    a->selpos = 0;
    a->f = f;
    a->root = ncreate(0, icreate(umcreate(), false, 0, 0), "", 0);
    
    if (key == 0 || *key == '\0') {
        archsetbitfield(a, false, false);
        a->key = 0;
    }
    else {
        a->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
        strcpy(a->key, key);
        a->xor = xor;
        
        if (xor)
            archsetbitfield(a, true, true);
        else
            archsetbitfield(a, true, false);
    }
    
    fseek(f, 1, SEEK_SET);
    return a;
}

void archrelease(ARCH* a) {
    if (a == 0) return;
    free(a->key);
    nrelease(a->root);
    fclose(a->f);
    free(a);
}