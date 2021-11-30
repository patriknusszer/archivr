#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "unordered_map.h"
#include "ARCH.h"
#include "util.h"
#include "IT.h"
#include "dirmapper.h"
#include <wchar.h>
#include <locale.h>
#include "debugmalloc.h"

static char wd[200];
static bool q = false;
static ARCH* a = 0;

void lscmds() {
    printf("please note: use double quotes to escape whitespaces\n");
    printf("open <arch_path> [<enc_key>]\n");
    printf("create <arch_path> [<enc_key> <xor/xnor>]\n");
    printf("extract <node_path> [<custom_path>]\n");
    printf("packdir <directoy_path> [<custom_path>]\n");
    printf("packfile <file_path> [<custom_path>]\n");
    printf("crypto <on/off> <enc_key> [<xor/xnor>]\n");
    printf("quit\n");
}

void unknownmsg() {
    printf("could not recognise command\n");
}

void errormsg(char* inf) {
    printf("error: %s\n", inf);
}

int main(int argc, const char * argv[]) {
    printf("archivr v1.0 - Patrik Nusszer\n");
    cwd(wd, 100);
    
    while (!q) {
        char cmd[50];
        printf("cmd: ");
        scanf(" %[^\n]", cmd);
        int sz;
        char** split = cmdsplit(cmd, &sz);
        
        if (sz == 0) unknownmsg();
        else if (sz == 1) {
            if (!strcmp(split[0], "help"))
                lscmds();
            else if (!strcmp(split[0], "quit"))
                q = true;
            else unknownmsg();
        }
        else if (sz == 2) {
            if (!strcmp(split[0], "extract")) {
                if (a == 0) errormsg("no archive is open");
                if (!archextract(a, split[1], wd)) errormsg("error extracting specified file");
            }
            else if (!strcmp(split[0], "packfile")) {
                if (a == 0) errormsg("no archive is open");
                else if (!archfincorp(a, split[1])) errormsg("file already exists");
            }
            else if (!strcmp(split[0], "packdir")) {
                if (a == 0) errormsg("no archive is open");
                else if (!archdincorp(a, split[1])) errormsg("dir already exists");
            }
            else if (!strcmp(split[0], "open")) {
                ARCH* _a = archparse(split[1], 0);
                if (_a == 0) errormsg("could not open archive");
                else if (a != 0)
                    archrelease(a);
                a = _a;
            }
            else if (!strcmp(split[0], "create")) {
                ARCH* _a = archcreate(split[1], 0, true);
                if (_a == 0) errormsg("could not create archive");
                else if (a != 0)
                    archrelease(a);
                a = _a;
            }
            else if (!strcmp(split[0], "crypto")) {
                if (a == 0) errormsg("no archive is open");
                else if (!strcmp(split[1], "off")) {
                    if (!archsetcrypto(a, false, 0, false)) errormsg("archive was not encrypted");
                }
                else unknownmsg();
            }
            else unknownmsg();
        }
        else if (sz == 3) {
            if (!strcmp(split[0], "open")) {
                ARCH* _a = archparse(split[1], split[2]);
                if (_a == 0) errormsg("could not open archive");
                else if (a != 0)
                    archrelease(a);
                a =_a;
            }
            else if (!strcmp(split[0], "extract")) {
                if (a == 0) errormsg("no archive is open");
                else if (!archextract(a, split[1], split[2])) errormsg("error extracting specified file");
            }
            else if (!strcmp(split[0], "packfile")) {
                if (a == 0) errormsg("no archive is open");
                else if (!archfincorp_e(a, split[1], split[2])) errormsg("file already exists");
            }
            else if (!strcmp(split[0], "packdir")) {
                if (a == 0) errormsg("no archive is open");
                else if (!archdincorp_e(a, split[1], split[2])) errormsg("dir already exists");
            }
            else if (!strcmp(split[0], "crypto")) {
                if (a == 0) errormsg("no archive is open");
                else if (!strcmp(split[1], "off")) {
                    if (archsetcrypto(a, false, split[2], true)) errormsg("archive is not encrypted");
                }
                else unknownmsg();
            }
        }
        else if (sz == 4) {
            if (!strcmp(split[0], "crypto")) {
                if (a == 0) errormsg("no archive is open");
                else if (!strcmp(split[1], "on")) {
                    if (!strcmp(split[3], "xor")) {
                        if (!archsetcrypto(a, true, split[2], true))
                            errormsg("archive is already encrypted");
                    }
                    else if (!strcmp(split[3], "xnor")) {
                        if (!archsetcrypto(a, true, split[2], false))
                            errormsg("archive is already encrypted");
                    }
                    else unknownmsg();
                }
                else unknownmsg();
            }
            else if (!strcmp(split[0], "create")) {
                if (!strcmp(split[3], "xor")) {
                    ARCH* _a;
                    if ((_a = archcreate(split[1], split[2], true)) == 0)
                        errormsg("could not create archive");
                    else if (a != 0)
                        archrelease(a);
                    a = _a;
                }
                else if (!strcmp(split[3], "xnor")) {
                    ARCH* _a;
                    if ((_a = archcreate(split[1], split[2], false)) == 0)
                        errormsg("could not create archive");
                    else if (a != 0)
                        archrelease(a);
                    a = _a;
                }
                else unknownmsg();
            }
            else unknownmsg();
        }
        else unknownmsg();

        ssrelease(split, sz);
    }

    archrelease(a);
    /* ARCH* a = archcreate("/Users/patrik.nusszer/Desktop/out", 0, false);
    archdincorp_e(a, "/Users/patrik.nusszer/Desktop/kepek", "this/well/hello");
    bool b = archsetcrypto(a, true, "annettewhite", true);
    archfincorp_e(a, "/Users/patrik.nusszer/Desktop/aw.PNG", "aw.PNG");
    archextract(a, "this/well/hello/kepek", "/Users/patrik.nusszer/Desktop/tst");
    archrelease(a); */
    return 0;
}
