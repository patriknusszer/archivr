#include "node.h"
#include "unordered_map.h"
#include "info.h"

typedef unsigned int uint;

typedef struct node {
    struct node* next;
    void* data;
    char* key;
    uint hash;
} node;

node** nnextp(node* n) { return &n->next; }
void* ndat(node* n) { return n->data; }
node* nnext(node* n) { return n->next; }
char* nkey(node* n) { return n->key; }
uint nhash(node* n) { return n->hash; }

node* ncreate(node* next, void* data, char* key, uint hash) {
    node* n = (node*)malloc(sizeof(node));
    n->data = data;
    n->next = next;
    n->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(n->key, key);
    return  n;
}

void llrelease(node* n) {
    if (n != 0) {
        node* next = nnext(n);
        free(ndat(n));
        free(n);
        llrelease(next);
    }
}

void llwalk(node* n, void (*iterator)(node* i)) {
    if (n != 0) {
        iterator(n);
        llwalk(nnext(n), iterator);
    }
    
    return;
}
