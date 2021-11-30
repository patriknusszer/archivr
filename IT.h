//
//  IT.h
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 22..
//

#ifndef IT_h
#define IT_h

#include "node.h"

typedef struct IT IT;
IT* itcreate(node** buckets, int bsz);
void itrelease(IT* i);
node* itnext(IT* i);

#endif
