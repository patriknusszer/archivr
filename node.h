#ifndef node_h
#define node_h

typedef unsigned int uint;
typedef struct node node;
void* ndat(node* n);
char* nkey(node* n);
node* nnext(node* n);
node** nnextp(node* n);
node* ncreate(node* next, void* data, char* key, uint hash);
uint nhash(node* n);
void llrelease(node* n, void (*relfunc)(void*));
void llwalk(node* n, void (*iterator)(node* i));

#endif
