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

static char wd[200];

int main(int argc, const char * argv[]) {
    cwd(wd, 100);
    printf("archivr v1.0 - Patrik Nusszer\n");
    printf("cmd: ");
    char* s = "alma gyar";
    char t[50];
    sscanf(s, "%s", t);
    FILE* f = fopen("/Users/patrik.nusszer/Desktop/out", "wb");
    /*int bytes = 1;
    fwrite(&bytes, sizeof(char), 1, f);
    bytes = 5;
    fwrite(&bytes, sizeof(int), 1, f);
    char buf[] = {34, 53, 12, 44, 99};
    fwrite(buf, sizeof(char), 5, f);
    char* s = "this/is/one/path";
    fwrite(s, sizeof(char), strlen(s) + 1, f);
    fwrite(&bytes, sizeof(int), 1, f);
    fwrite(buf, sizeof(char), 5, f);
    s = "this/is/other/path";
    fwrite(s, sizeof(char), strlen(s) + 1, f);
    fflush(f);
    fclose(f); */
    //ARCH* a = archparse("/Users/patrik.nusszer/Desktop/out", "annettewhite");
    //bool b = archsetcrypto(a, true, "annettewhite", true);
    // archdincorp(a, "/Users/patrik.nusszer/Desktop/kepek");
    /* archdincorp_e(a, "/Users/patrik.nusszer/Desktop/kepek", "this/well/hello");
    bool b = archsetcrypto(a, true, "annettewhite", false);
    archextract(a, "this", "/Users/patrik.nusszer/Desktop/tst");
    archfincorp(a, "/Users/patrik.nusszer/Desktop/aw.PNG", "aw.PNG");
    archextract(a, "aw.PNG", "/Users/patrik.nusszer/Desktop/maw");
    bool b2 = archfseek(a, "aw.PNG");
    char buff[1024];
    int read;
    while ((read = archfread(a, buff, 1024)) == 1024);
    bool eof = archfeof(a); */
    ARCH* a = archcreate("/Users/patrik.nusszer/Desktop/out", 0, false);
    archdincorp_e(a, "/Users/patrik.nusszer/Desktop/kepek", "this/well/hello");
    bool b = archsetcrypto(a, true, "annettewhite", true);
    archfincorp(a, "/Users/patrik.nusszer/Desktop/aw.PNG", "aw.PNG");
    archextract(a, "this/well/hello/kepek", "/Users/patrik.nusszer/Desktop/tst");
    
    
    
    return 0;
}
