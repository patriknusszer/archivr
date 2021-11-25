//
//  NIT.h
//  archivr
//
//  Created by Patrik Nusszer on 2021. 11. 23..
//

#ifndef NIT_h
#define NIT_h

#include "node.h"
#include "IT.h"

typedef struct NIT NIT;
node* nitnext(NIT* n);
NIT* nitcreate(node* n);
void nitrelease(NIT* n);

#endif
