#include "dirmapper.h"
#include <stdlib.h>
#include "string.h"
#include <stdio.h>
#include "debugmalloc.h"

typedef struct SUBS {
    char** files;
    int fcnt;
    char** dirs;
    int dcnt;
} SUBS;

SUBS* subscreate(char** files, int fcnt, char** dirs, int dcnt) {
    SUBS* subs = (SUBS*)malloc(sizeof(SUBS));
    *subs = (SUBS){files, fcnt, dirs, dcnt };
    return subs;
}

char** subsdirs(SUBS* subs, int* dcnt) {
    *dcnt = subs->dcnt;
    return subs->dirs;
}

char** subsfiles(SUBS* subs, int* fcnt) {
    *fcnt = subs->fcnt;
    return subs->files;
}

#if (defined(WIN32) || defined(_WIN32) ||defined (__WIN32__))
char* toWindowsPath(char* path) {
    if (path[strlen(path) - 1] == '/')
        return path.append("*");
    else
        return path.append("/*");
}
#endif

SUBS* subsof(char* path) {
    int fcap = 10, fcnt = 0;
    int dcap = 10, dcnt = 0;
    char** files = (char**)malloc(sizeof(char*) * fcap);
    char** dirs = (char**)malloc(sizeof(char*) * dcap);
    
#if (defined(WIN32) || defined(_WIN32) ||defined (__WIN32__))
    HANDLE hFind;
    WIN32_FIND_DATA data;
    hFind = FindFirstFile(toWindowsPath(path), &data);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (data.cFileName[0] != '.') {
                char* entryName = data.cFileName;
                
                if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    strcpy(dirs[dcnt++] = (char*)malloc(sizeof(char) * (strlen(entryName) + 1)), entryName);
                    
                    if (dcnt == dcap)
                        dirs = (char**)realloc(dirs, sizeof(char*) * (dcap *= 2));
                } else {
                    strcpy(files[fcnt++] = (char*)malloc(sizeof(char) * (strlen(entryName) + 1)), entryName);
                    
                    if (fcnt == fcap)
                        files = (char**)realloc(files, sizeof(char*) * (fcap *= 2));
                }
            }
        } while (FindNextFile(hFind, &data));
        
        FindClose(hFind);
    }
    
#else
    DIR* directory = opendir(path);
    struct dirent* entry;
    
    if (directory != NULL) {
        while ((entry = readdir(directory)) != NULL) {
            char* entryName = entry->d_name;
            
            if (entryName[0] != '.') {
                /* struct stat st;
                fstatat(dirfd(directory), entry->d_name, &st, 0);
                if (S_ISDIR(st.st_mode))
                    printf("dir!!!!"); */
                
                if (entry->d_type == DT_DIR) {
                    dirs[dcnt] = (char*)malloc(sizeof(char) * (strlen(entryName) + 1));
                    strcpy(dirs[dcnt++], entryName);
                    
                    if (dcnt == dcap)
                        dirs = (char**)realloc(dirs, sizeof(char*) * (dcap *= 2));
                } else {
                    files[fcnt] = (char*)malloc(sizeof(char) * (strlen(entryName) + 1));
                    strcpy(files[fcnt++], entryName);
                    
                    if (fcnt == fcap)
                        files = (char**)realloc(files, sizeof(char*) * (fcap *= 2));
                }
            }
        }
        closedir(directory);
    } else
        return 0;
    
#endif
    return subscreate(files, fcnt, dirs, dcnt);
}
