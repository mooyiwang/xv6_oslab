//
// Support functions for system call SYSINFO
//
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stat.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "sysinfo.h"

void
_info(struct sysinfo *info){
    
    info->freemem = freemem();
    info->nproc = nproc();
    info->freefd = freefd();
}

int
sysinfo(uint64 addr){
    
    struct proc *p = myproc();
    struct sysinfo info;

    _info(&info);

    if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0 )
        return -1;
    return 0;

}