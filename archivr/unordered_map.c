#include "unordered_map.h"
#include "node.h"
#include "util.h"
#include "IT.h"

typedef unsigned long size_t;
typedef unsigned int uint;

typedef struct unordered_map {
    uint sz;
    uint bsz;
    node** buckets;
} unordered_map;

// bu = bucket
// ll = linked list
// um = unordered map

IT* umiterator(unordered_map* um) {
    return itcreate(um->buckets, um->bsz);
}

void umwalk(unordered_map* um, void (*iterator)(node* i)) {
    for (uint i = 0; i < um->bsz; i++)
        llwalk(um->buckets[i], iterator);
}

uint rollhash32(char* key, int b, int m) {
    uint hash = 0;
    uint coeff = 1;
    
    for (; *key != '\0'; key++) {
        hash = (hash + *key * coeff) % m;
        coeff *= b;
        coeff %= m;
    }

    return hash;
}

uint hashof(char* key) { return rollhash32(key, 257, 1761471161); }

unordered_map* umcreate() {
    unordered_map* um = (unordered_map*)malloc(sizeof(unordered_map));

    if (um == 0)
        return 0;

    *um = (unordered_map){ 0, 5, (node**)calloc(5, sizeof(node*)) };
    return um;
}

node* busearch(node* bucket, char* key) {
    if (bucket == 0)
        return 0;

    if (!strcmp(nkey(bucket), key))
        return bucket;

    return busearch(nnext(bucket), key);
}

node* buinsert(void* dat, char* key, uint hash, node** bucket) {
    if (*bucket == 0)
        return *bucket = ncreate(0, dat, key, hash);

    if (!strcmp(nkey(*bucket), key))
        return 0;

    return buinsert(dat, key, hash, nnextp(*bucket));
}

node** umgetbucket(unordered_map* um, char* key, uint* hash) {
    uint h = hashof(key);
    if (hash != 0) *hash = h;
    return um->buckets + (h % um->bsz);
}

node* umget(unordered_map* um, char* key) {
    node** bucket = umgetbucket(um, key, 0);
    return busearch(*bucket, key);
}

bool umresize(unordered_map* um) {
    size_t poolsz = um->bsz * 2;
    node** pool = (node**)calloc(poolsz, sizeof(node*));
    
    if (pool == 0)
        return false;
    
    for (size_t i = 0; i < um->bsz; i++) {
        node* next = um->buckets[i];
        
        while (next != 0) {
            buinsert(ndat(next), nkey(next), nhash(next), pool + (nhash(next) % poolsz));
            next = nnext(next);
        }
    }
    
    free(um->buckets);
    um->buckets = pool;
    return true;
}

node* umput(unordered_map* um, char* key, void* dat) {
    uint hash;
    node** bucket = umgetbucket(um, key, &hash);
    node* res = buinsert(dat, key, hash, bucket);

    if (res) {
        um->sz++;

        if (um->sz == 2 * um->bsz)
            umresize(um);
    }

    return res;
}

void umrelease(unordered_map* um) {
    for (uint i = 0; i < um->bsz; i++)
        llrelease(um->buckets[i]);
    
    free(um->buckets);
}
