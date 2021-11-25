//
//  IT.c
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 22..
//

#include "IT.h"
#include "node.h"
#include "stdlib.h"

typedef struct IT {
    node** buckets;
    int curb, bsz;
    node* next;
} IT;

IT* itcreate(node** buckets, int bsz) {
    IT* i = (IT*)malloc(sizeof(IT));
    i->buckets = buckets;
    i->bsz = bsz;
    i->curb = 0;
    i->next = buckets == 0 ? 0 : buckets[0];
    return i;
}

void itrelease(IT* i) {
    free(i);
}

node* itnext(IT* i) {
    if (i->next == 0) {
        if (i->curb + 1 < i->bsz) {
            i->next = i->buckets[++(i->curb)];
            return itnext(i);
        }
        else return 0;
    }
    else {
        node* cpy = i->next;
        i->next = nnext(i->next);
        return cpy;
    }
}
