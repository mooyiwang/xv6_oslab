#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

//inspired by user/ls.c
//close a doc after using, otherwise it will occupy system recourse
//没考虑没有找到的情况

//get doc's name from it's path
char*
fmtname(char *path)
{

  char *p;
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void find(char *path, char *target){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    //robust
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        if(!strcmp(fmtname(path), target)){
            printf("%s\n", path);
        }
        close(fd);
        return;

    case T_DIR:

        if(!strcmp(fmtname(path), target)){
            printf("%s\n", path);
            close(fd);
            return;
        }

        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
        break;
        }

        //append '/' to path
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        //read directory like a real document
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            
            if(de.inum == 0)
                continue;
            //append sub-directory(inner content)'s name to path, ie, "path/sub-directory"
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            //do not run into . or ..
            if(strcmp(p, ".") != 0 && strcmp(p, "..") != 0){
                find(buf, target);
            }
        }
    }
}

int main(int argc,char* argv[]){
    if(argc < 2 ){
        fprintf(2, "find: input \"find [path] [keyword]\" \n");
        exit(-1);
    }
    else if(argc == 2){
        if(!strcmp(".", argv[1])){
            fprintf(2, "find: input \"find [path] [keyword]\" \n");
            exit(-1);
        }
        else{
            find(".", argv[1]);
            exit(0);
        }
    }
    else{
        find(argv[1], argv[2]);
        exit(0);
    }
}