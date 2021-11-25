//
//  NIT.c
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 23..
//

#include "NIT.h"
#include "node.h"
#include "util.h"
#include "IT.h"
#include "unordered_map.h"

typedef struct NIT {
    IT* it;
} NIT;

node* nitnext(NIT* n) { return itnext(n->it); }

void nitrelease(NIT* n) {
    itrelease(n->it);
    free(n);
}

NIT* nitcreate(node* n) {
    NIT* nit = (NIT*)malloc(sizeof(NIT));
    nit->it = umiterator(num(n));
    return nit;
}
